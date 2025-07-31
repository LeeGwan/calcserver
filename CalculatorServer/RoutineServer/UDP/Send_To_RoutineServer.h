#pragma once
#include "UdpBase.h"

class Send_To_RoutineServer : public UdpBase
{
public:
    void Init(const std::string In_Ip, int Send_In_port_to_Routine, int In_workerCount);
    Task& Get_Task() override;
    SocketType Get_Sock_SendServer() override;
    SocketType Get_Sock_RecvServer() override;
    SockAddrIn& Get_Sockaddr_SendServer() override;
    SockAddrIn& Get_Sockaddr_RecvServer() override;
    std::queue<Task>& Get_taskQueue() override;
    std::mutex& Get_queueMutex() override;
    std::condition_variable& Get_cv_() override;
    std::vector<std::thread>& Get_workers_() override;
    bool C_Is_Running() override;
    int C_Get_WorkCount() override;
    int C_Get_Send_Port() const override;
    int C_Get_Recv_Port() const override;
    std::string C_Get_Send_ip() const override;
    void Set_Running(bool Change_Running) override;
    void Release() override;

private:
    int Send_Port;

    SocketType serverSocket_from_Routine_to_Calc = INVALID_SOCKET_VALUE;
    SocketType serverSocket_from_Calc_to_Routine = INVALID_SOCKET_VALUE;
    SockAddrIn serverAddr_from_Calc_to_Routine = {};

    //  Windows 전용 wsaData 추가
#ifdef PLATFORM_WINDOWS
    WSADATA wsaData;
#endif

    std::queue<Task> taskQueue_;
    Task task;
    std::mutex queueMutex_;
    std::condition_variable cv_;
    std::vector<std::thread> workers_;
    bool running_;
    int workerCount;
    std::string ip;
};