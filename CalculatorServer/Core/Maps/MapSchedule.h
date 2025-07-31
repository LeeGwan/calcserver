#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <atomic> 
#include <csignal>
class Map;
class NavMesh;
class UdpManager;
class MapSchedule final
{
public:
    static void signal_handler(int sig);
    MapSchedule();
    ~MapSchedule();
    // �� ����
    void Make(int in_mapID, std::vector<std::string> in_Name, std::vector<std::string> in_Hash);

    // ���� ����
    void Shutdownserver();

    // �� ��� ��������
    Map* Get_MapList(int find_mapID);

    // UDP �Ŵ��� ��������
    UdpManager* Get_UDP();

    // ���� ���� ó��
    bool This_Game_Finish(int mapID);

private:
    mutable std::shared_mutex MapSchedulemutex;
    std::unordered_map<int, std::unique_ptr<Map>> MapList;
    std::unordered_map<int, std::vector<std::thread>> threadList;
    std::unique_ptr<UdpManager> UDP;
    static std::atomic<bool> shutdown_requested;
    bool server_state;
};

extern std::unique_ptr<MapSchedule> G_MapSchedule;