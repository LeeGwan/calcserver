
#include "MapSchedule.h"
#include "Map.h"
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <chrono>
#include <functional>

#include "../../RoutineServer/UDP/UdpManager.h"
#include "../Object/Object/DB/ObjectDB/ObjectLib.h"
std::atomic<bool> MapSchedule::shutdown_requested{ false };
std::unique_ptr<MapSchedule> G_MapSchedule = std::make_unique<MapSchedule>();

void MapSchedule::signal_handler(int sig) {
	std::cout << "Shutdown signal received: " << sig << std::endl;

	if (G_MapSchedule) {
		G_MapSchedule->Shutdownserver();
	}

	std::exit(0);
}
MapSchedule::MapSchedule()
{
	server_state = true;
	UDP = std::make_unique<UdpManager>();
	UDP->init(true, 2, 9009, 2, 9012);
	std::signal(SIGINT, signal_handler);
	std::signal(SIGSEGV, signal_handler);
}

MapSchedule::~MapSchedule() = default;
void MapSchedule::Make(int in_mapID, std::vector<std::string> in_Name, std::vector<std::string> in_Hash)
{
	// 맵 생성
	MapList[in_mapID] = std::make_unique<Map>(in_mapID, in_Name, in_Hash);

	// 스레드 생성 및 시작
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_In, MapList[in_mapID].get())));
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_Out, MapList[in_mapID].get())));
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_In_CALC, MapList[in_mapID].get())));
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_SkillCalc, MapList[in_mapID].get())));
	// 추가 스레드들 (현재 주석 처리)
//	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_AI, MapList[in_mapID].get())));
	//threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_AI_CALC, MapList[in_mapID].get())));


	// 모든 스레드를 detach
	for (size_t i = 0; i < threadList[in_mapID].size(); i++)
	{
		threadList[in_mapID][i].detach();
	}
}

void MapSchedule::Shutdownserver()
{
	shutdown_requested = true;
	if (!MapList.empty())
	{
		for (auto& [id, map] : MapList)
		{
			// 게임 상태를 false로 설정
			map->SetGameState(false);

			// 스레드가 종료될 시간을 줌
			std::this_thread::sleep_for(std::chrono::seconds(2));

			// 스레드 조인 (detach된 스레드는 join할 수 없으므로 주석 처리)
			/*
			for (size_t i = 0; i < threadList[id].size(); i++)
			{
				if (threadList[id][i].joinable())
					threadList[id][i].join();
			}
			*/

			// 맵 서버 종료
			map->ShutdownServer();
		}

		// 컨테이너 정리
		threadList.clear();
		MapList.clear();
	}

	// UDP 매니저 해제
	if (UDP)
	{
		UDP->Release();
	}

}

Map* MapSchedule::Get_MapList(int find_mapID)
{
	std::shared_lock lock(MapSchedulemutex);  // 읽기 전용이므로 shared_lock 사용

	auto it = MapList.find(find_mapID);
	if (it != MapList.end())
	{
		return it->second.get();
	}
	return nullptr;  // 찾지 못한 경우 nullptr 반환
}

UdpManager* MapSchedule::Get_UDP()
{
	std::shared_lock lock(MapSchedulemutex);  // 읽기 전용이므로 shared_lock 사용
	return UDP.get();
}


bool MapSchedule::This_Game_Finish(int mapID)
{
	std::unique_lock lock(MapSchedulemutex);  // 쓰기 작업이므로 unique_lock 사용

	auto it = MapList.find(mapID);
	if (it == MapList.end())
		return false;

	// 게임 상태를 false로 설정
	it->second->SetGameState(false);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	// 스레드 조인 (detach된 스레드는 join할 수 없으므로 주석 처리)
	/*
	for (auto& th : threadList[mapID])
	{
		if (th.joinable())
			th.join();
	}
	*/

	// 맵 서버 종료
	it->second->ShutdownServer();

	// 컨테이너에서 제거
	MapList.erase(it);
	threadList.erase(mapID);

	return true;
}