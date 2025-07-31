#include "Recv_To_RoutineServer.h"
#include <iostream>


void Recv_To_RoutineServer::Init(const std::string In_Ip, int Recv_In_port_from_Routine, int In_workerCount)
{
    ip = In_Ip;
    Recv_Port = Recv_In_port_from_Routine;
    workerCount = In_workerCount;
    running_ = true;

    //  수정된 네트워크 초기화
#ifdef PLATFORM_WINDOWS
    if (NETWORK_INIT(wsaData) != 0) {  // wsaData 멤버 변수 사용
        std::cout << "네트워크 초기화 실패" << std::endl;
        return;
    }
#else
    NETWORK_INIT(wsaData); // Linux는 wsaData 무시됨
#endif

    // 소켓 생성
    serverSocket_from_Routine_to_Calc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket_from_Routine_to_Calc == INVALID_SOCKET_VALUE) {
        std::cout << "소켓 생성 실패: " << GET_LAST_ERROR() << std::endl;
        return;
    }

    // 플랫폼별 타임아웃 설정
    SET_SOCKET_TIMEOUT(serverSocket_from_Routine_to_Calc, 1000);

    // 주소 설정
    serverAddr_from_Routine_to_Calc.sin_family = AF_INET;
#ifdef PLATFORM_WINDOWS
    serverAddr_from_Routine_to_Calc.sin_port = htons(Recv_Port);
#else
    serverAddr_from_Routine_to_Calc.sin_port = htons(Recv_Port);
#endif
    serverAddr_from_Routine_to_Calc.sin_addr.s_addr = INADDR_ANY;

    // 바인드
    int result = bind(serverSocket_from_Routine_to_Calc,
        (SockAddr*)&serverAddr_from_Routine_to_Calc,
        sizeof(serverAddr_from_Routine_to_Calc));

    if (result == SOCKET_ERROR_VALUE) {
        std::cout << "바인드 실패: " << GET_LAST_ERROR() << std::endl;
    }
    
}

void Recv_To_RoutineServer::Release()
{
    running_ = false;

    // 큐 정리
    while (!taskQueue_.empty()) {
        taskQueue_.pop();
    }

    cv_.notify_all();

    // 스레드 종료 대기
    for (auto& thread : workers_) {
        if (thread.joinable()) thread.join();
    }

    // 소켓 닫기
    if (serverSocket_from_Routine_to_Calc != INVALID_SOCKET_VALUE) {
        CLOSE_SOCKET(serverSocket_from_Routine_to_Calc);
    }
    if (serverSocket_from_Calc_to_Routine != INVALID_SOCKET_VALUE) {
        CLOSE_SOCKET(serverSocket_from_Calc_to_Routine);
    }

    // 플랫폼별 네트워크 정리
    NETWORK_CLEANUP();
}

Task& Recv_To_RoutineServer::Get_Task()
{
    return task;
}

SocketType Recv_To_RoutineServer::Get_Sock_SendServer()
{
    return serverSocket_from_Calc_to_Routine;
}

SocketType Recv_To_RoutineServer::Get_Sock_RecvServer()
{
    return serverSocket_from_Routine_to_Calc;
}

SockAddrIn& Recv_To_RoutineServer::Get_Sockaddr_SendServer()
{
    static SockAddrIn dummy{};
    return dummy;
}

SockAddrIn& Recv_To_RoutineServer::Get_Sockaddr_RecvServer()
{
    return serverAddr_from_Routine_to_Calc;
}

std::queue<Task>& Recv_To_RoutineServer::Get_taskQueue()
{
    return taskQueue_;
}

std::mutex& Recv_To_RoutineServer::Get_queueMutex()
{
    return queueMutex_;
}

std::condition_variable& Recv_To_RoutineServer::Get_cv_()
{
    return cv_;
}

std::vector<std::thread>& Recv_To_RoutineServer::Get_workers_()
{
    return workers_;
}

bool Recv_To_RoutineServer::C_Is_Running()
{
    return running_;
}

int Recv_To_RoutineServer::C_Get_WorkCount()
{
    return workerCount;
}

int Recv_To_RoutineServer::C_Get_Send_Port() const
{
    return 0;
}



int Recv_To_RoutineServer::C_Get_Recv_Port()const
{
    return Recv_Port;
}

std::string Recv_To_RoutineServer::C_Get_Send_ip() const
{
    return ip;
}

void Recv_To_RoutineServer::Set_Running(bool Change_Running)
{
    running_ = Change_Running;
}

