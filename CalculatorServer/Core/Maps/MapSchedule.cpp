
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
	// �� ����
	MapList[in_mapID] = std::make_unique<Map>(in_mapID, in_Name, in_Hash);

	// ������ ���� �� ����
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_In, MapList[in_mapID].get())));
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_Out, MapList[in_mapID].get())));
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_In_CALC, MapList[in_mapID].get())));
	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_SkillCalc, MapList[in_mapID].get())));
	// �߰� ������� (���� �ּ� ó��)
//	threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_AI, MapList[in_mapID].get())));
	//threadList[in_mapID].push_back(std::thread(std::bind(&Map::Tick_AI_CALC, MapList[in_mapID].get())));


	// ��� �����带 detach
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
			// ���� ���¸� false�� ����
			map->SetGameState(false);

			// �����尡 ����� �ð��� ��
			std::this_thread::sleep_for(std::chrono::seconds(2));

			// ������ ���� (detach�� ������� join�� �� �����Ƿ� �ּ� ó��)
			/*
			for (size_t i = 0; i < threadList[id].size(); i++)
			{
				if (threadList[id][i].joinable())
					threadList[id][i].join();
			}
			*/

			// �� ���� ����
			map->ShutdownServer();
		}

		// �����̳� ����
		threadList.clear();
		MapList.clear();
	}

	// UDP �Ŵ��� ����
	if (UDP)
	{
		UDP->Release();
	}

}

Map* MapSchedule::Get_MapList(int find_mapID)
{
	std::shared_lock lock(MapSchedulemutex);  // �б� �����̹Ƿ� shared_lock ���

	auto it = MapList.find(find_mapID);
	if (it != MapList.end())
	{
		return it->second.get();
	}
	return nullptr;  // ã�� ���� ��� nullptr ��ȯ
}

UdpManager* MapSchedule::Get_UDP()
{
	std::shared_lock lock(MapSchedulemutex);  // �б� �����̹Ƿ� shared_lock ���
	return UDP.get();
}


bool MapSchedule::This_Game_Finish(int mapID)
{
	std::unique_lock lock(MapSchedulemutex);  // ���� �۾��̹Ƿ� unique_lock ���

	auto it = MapList.find(mapID);
	if (it == MapList.end())
		return false;

	// ���� ���¸� false�� ����
	it->second->SetGameState(false);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	// ������ ���� (detach�� ������� join�� �� �����Ƿ� �ּ� ó��)
	/*
	for (auto& th : threadList[mapID])
	{
		if (th.joinable())
			th.join();
	}
	*/

	// �� ���� ����
	it->second->ShutdownServer();

	// �����̳ʿ��� ����
	MapList.erase(it);
	threadList.erase(mapID);

	return true;
}