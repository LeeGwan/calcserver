#include "CalculatorServer.h"
#include "../Core/Maps/MapSchedule.h"
#include "UDP/UdpManager.h"
#include"../Core/Navmesh/NavMesh.h"
#include "../Core/Object/Object/DB/ObjectDB/ObjectLib.h"
#include "../Core/Object/Object/DB/SpellDB/SpellLib.h"
#include <crtdbg.h>
bool CalculatorServer::IsCalculatorServer_Init()
{


    int input = 0;

    ObjectDB::initialize();
    SpellDB::initialize();
    //9009, 9010

    while (1)
    {
        std::cout << "서버종료 하실거에요? 1번:";std::cin >> input;
        if (input == 1)
        {

            G_MapSchedule->Shutdownserver();
            ObjectDB::Release();
            SpellDB::Release();
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    return true;
}