#include "Send_To_RoutineServer.h"
#include <iostream>

void Send_To_RoutineServer::Init(const std::string In_Ip, int Send_In_port_to_Routine, int In_workerCount)
{
    ip = In_Ip;
    Send_Port = Send_In_port_to_Routine;
    workerCount = In_workerCount;
    running_ = true;

    //수정된 네트워크 초기화
#ifdef PLATFORM_WINDOWS
    if (NETWORK_INIT(wsaData) != 0) {  // wsaData 멤버 변수 사용
        std::cout << "네트워크 초기화 실패" << std::endl;
        return;
    }
#else
    NETWORK_INIT(wsaData); // Linux는 wsaData 무시됨
#endif

    // 소켓 생성
    serverSocket_from_Calc_to_Routine = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket_from_Calc_to_Routine == INVALID_SOCKET_VALUE) {
        std::cout << "소켓 생성 실패: " << GET_LAST_ERROR() << std::endl;
        return;
    }

    // 플랫폼별 타임아웃 설정
    SET_SOCKET_TIMEOUT(serverSocket_from_Calc_to_Routine, 1000);

    // 주소 설정
    serverAddr_from_Calc_to_Routine.sin_family = AF_INET;
#ifdef PLATFORM_WINDOWS
    serverAddr_from_Calc_to_Routine.sin_port = htons(Send_Port);
    serverAddr_from_Calc_to_Routine.sin_addr.s_addr = inet_addr(ip.c_str());
#else
    serverAddr_from_Calc_to_Routine.sin_port = htons(Send_Port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr_from_Calc_to_Routine.sin_addr);
#endif


}
void Send_To_RoutineServer::Release()
{
    running_ = false;
    // 모든 스레드 깨우기

    while (true)
    {

        if (!taskQueue_.empty())
        {
            taskQueue_.pop();
        }
        else
        {
            break;
        }
    }
    cv_.notify_all();
    for (auto& thread : workers_)
    {

        if (thread.joinable()) thread.join();
    }
    if (serverSocket_from_Routine_to_Calc)
    {
        closesocket(serverSocket_from_Routine_to_Calc);
    }
    if (serverSocket_from_Calc_to_Routine)
    {
        closesocket(serverSocket_from_Calc_to_Routine);
    }
    WSACleanup();
 
   // std::cout << "All threads are finished. Shutting down server." << std::endl;
}

Task& Send_To_RoutineServer::Get_Task()
{
    return task;
}







SOCKET Send_To_RoutineServer::Get_Sock_SendServer()
{
    return serverSocket_from_Calc_to_Routine;
}

SOCKET Send_To_RoutineServer::Get_Sock_RecvServer()
{
    return serverSocket_from_Routine_to_Calc;
}

sockaddr_in& Send_To_RoutineServer::Get_Sockaddr_SendServer()
{
    return serverAddr_from_Calc_to_Routine;
}

sockaddr_in& Send_To_RoutineServer::Get_Sockaddr_RecvServer()
{
    static sockaddr_in dummy{};
    return dummy;
}



std::queue<Task>& Send_To_RoutineServer::Get_taskQueue()
{
    return taskQueue_;
}

std::mutex& Send_To_RoutineServer::Get_queueMutex()
{
    return queueMutex_;
}

std::condition_variable& Send_To_RoutineServer::Get_cv_()
{
    return cv_;
}

std::vector<std::thread>& Send_To_RoutineServer::Get_workers_()
{
    return workers_;
}

bool Send_To_RoutineServer::C_Is_Running()
{
    return running_;
}

int Send_To_RoutineServer::C_Get_WorkCount()
{
    return workerCount;
}

int Send_To_RoutineServer::C_Get_Send_Port()const
{
    return Send_Port;
}

int Send_To_RoutineServer::C_Get_Recv_Port() const
{
    return 0;
}



std::string Send_To_RoutineServer::C_Get_Send_ip() const
{
    return ip;
}

void Send_To_RoutineServer::Set_Running(bool Change_Running)
{
    running_ = Change_Running;
}

