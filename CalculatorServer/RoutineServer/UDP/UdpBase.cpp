#include "UdpBase.h"
#include "../../Core/Navmesh/NavMesh.h"
#include "../../Core/Object/Object/ObjectComponent/Object.h"
#include "Recv_To_RoutineServer.h"
#include "Send_To_RoutineServer.h"
#include "../../Core/Maps/Timer/Timer.h"
#include "../Message/OptimizedPacketHandler/OptimizedPacketHandler.h"
#include <iostream>

UdpBase::UdpBase()
{
}

UdpBase::~UdpBase()
{
}

void UdpBase::Initialize()
{
 
}

void UdpBase::Udp_Send_Tick()
{
    int ThreadCount = C_Get_WorkCount();
    std::vector<std::thread>& Workers = Get_workers_();
    for (int i = 0; i < ThreadCount; ++i)
    {
        Workers.emplace_back(&UdpBase::Send_WorkerThread, this, i);
    }
    for (auto& workers : Workers)
    {
        workers.detach();
    }
    while (bool running = C_Is_Running())
    {
        // 메인 루프에서 대기
       // std::this_thread::sleep_for(std::chrono::milliseconds(1);
    }
    std::cout << "\nudp send 서버도 종료\n";
}

void UdpBase::Udp_Recv_Tick()
{
    int ThreadCount = C_Get_WorkCount();
    std::vector<std::thread>& Workers = Get_workers_();
    for (int i = 0; i < ThreadCount; ++i)
    {
        Workers.emplace_back(&UdpBase::Recv_WorkerThread, this, i);
    }
    for (auto& workers : Workers)
    {
        workers.detach();
    }
    ReceiveLoop();

    std::cout << "\nudp recv 서버도 종료\n";
}

bool UdpBase::PrepareSendTask(Object* obj)
{
    if (obj->Get_Name().empty()) return false;

    SOCKET sock = Get_Sock_RecvServer();
    sockaddr_in& Recvsockaddr = Get_Sockaddr_RecvServer();
    int Recvsockaddrsize = sizeof(Recvsockaddr);
    Task& task = Get_Task();
    std::mutex& queueMutex_ = Get_queueMutex();
    std::queue<Task>& taskQueue_ = Get_taskQueue();
    std::condition_variable& cv_ = Get_cv_();

    while (bool running = C_Is_Running())
    {
    }
    return true;
}

int UdpBase::Recv(Task& task, int Len)
{
    SOCKET sock = Get_Sock_RecvServer();
    sockaddr_in& Recvsockaddr = Get_Sockaddr_RecvServer();
    int Recvsockaddrsize = sizeof(Recvsockaddr);
    std::unique_ptr<char[]> TempBuffer(new char[MAX_BUFFER_SIZE]);
    std::memset(TempBuffer.get(), 0x00, MAX_BUFFER_SIZE);

    int len = recvfrom(sock, TempBuffer.get(), MAX_BUFFER_SIZE, 0, (sockaddr*)&Recvsockaddr, &Recvsockaddrsize);
    if (len == SOCKET_ERROR)
        return len;

    try
    {
        // 바이너리 데이터를 Task에 저장 (JSON 제거)
        task.Recv_data.clear();
        task.Recv_data.insert(task.Recv_data.end(), TempBuffer.get(), TempBuffer.get() + len);

     
    }
    catch (const std::exception& e)
    {
        std::cout << "recv 바이너리 에러: " << e.what() << std::endl;
        return SOCKET_ERROR;
    }
    return len;
}

bool UdpBase::Send(const Task& out_buffer, int Len)
{
    try {
        // 1. 입력 데이터 유효성 검사
        if (out_buffer.Send_data.empty()) {
            std::cout << "Send: Empty data buffer" << std::endl;
            return false;
        }

        if (out_buffer.Send_data.size() > 65536) { // 64KB 제한
            std::cout << "Send: Data too large: " << out_buffer.Send_data.size() << std::endl;
            return false;
        }

        // 2. 안전한 데이터 복사
        std::vector<uint8_t> binary;
        binary.reserve(out_buffer.Send_data.size());

        try {
            binary = out_buffer.Send_data;
        }
        catch (const std::exception& e) {
            std::cout << "Send: Failed to copy data: " << e.what() << std::endl;
            return false;
        }

        // 3. 복사된 데이터 검증
        if (binary.empty() || binary.size() != out_buffer.Send_data.size()) {
            std::cout << "Send: Data copy verification failed" << std::endl;
            return false;
        }

        // 4. 소켓 정보 가져오기
        SOCKET sock = Get_Sock_SendServer();
        if (sock == INVALID_SOCKET) {
            std::cout << "Send: Invalid socket" << std::endl;
            return false;
        }

        sockaddr_in& Sendsockaddr = Get_Sockaddr_SendServer();
        int Sendsockaddrsize = sizeof(Sendsockaddr);

        // 5. 실제 송신
        int send_result = sendto(sock,
            reinterpret_cast<char*>(binary.data()),
            static_cast<int>(binary.size()),
            0,
            (sockaddr*)&Sendsockaddr,
            Sendsockaddrsize);

        bool Result = (send_result != SOCKET_ERROR);

        if (!Result) {
            int ErrorCode = WSAGetLastError();
            std::cout << " Send error: " << ErrorCode << std::endl;
        }
       

        // 6. 메모리 해제
        try {
            binary.clear();
            binary.shrink_to_fit();
        }
        catch (...) {
            std::cout << "Send: Warning - failed to clear binary buffer" << std::endl;
        }

        return Result;

    }
    catch (const std::exception& e) {
        std::cout << "Send: Exception: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cout << "Send: Unknown exception" << std::endl;
        return false;
    }
}

void UdpBase::ReceiveLoop()
{
    SOCKET sock = Get_Sock_RecvServer();
    sockaddr_in& Recvsockaddr = Get_Sockaddr_RecvServer();
    int Recvsockaddrsize = sizeof(Recvsockaddr);
    Task& task = Get_Task();
    std::mutex& queueMutex_ = Get_queueMutex();
    std::queue<Task>& taskQueue_ = Get_taskQueue();
    std::condition_variable& cv_ = Get_cv_();

    while (bool running = C_Is_Running())
    {
        int len = Recv(task);
        if (len == SOCKET_ERROR)
        {
            if (!running) break;
            else
            {
                continue;
            }
        }
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            {
                taskQueue_.push(task); // task를 큐에 추가
            }
        }
        cv_.notify_one();
    }
}

void UdpBase::Recv_WorkerThread(int id)
{
    Timer RecvTimer;
    float Time = 0.f;
    while (bool running = C_Is_Running())
    {
        RecvTimer.Update();
        Time += RecvTimer.GetDeltaTime();

        Task task;
        std::queue<Task>& taskQueue = Get_taskQueue();
        {
            std::unique_lock<std::mutex> lock(Get_queueMutex());

            Get_cv_().wait(lock, [&] {return !taskQueue.empty() || !C_Is_Running(); });
            if (!C_Is_Running()) break;

            task = taskQueue.front();
            taskQueue.pop();
        }

        // 최적화된 바이너리 패킷 처리 (JSON 대신)
        if (!task.Recv_data.empty()) {
            bool success = OptimizedPacketHandler::ProcessSecurePacket(task.Recv_data);
            if (!success) {
                std::cout << " 워커 스레드 " << id << ": 패킷 처리 실패 (변조/손상)" << std::endl;
            }
           
        }
    }
}

void UdpBase::Send_WorkerThread(int id)
{
    Timer SendTimer;
    float Time = 0.f;
    SOCKET sock = Get_Sock_SendServer();
    sockaddr_in& Sendsockaddr = Get_Sockaddr_SendServer();
    int Sendsockaddrsize = sizeof(Sendsockaddr);

    while (bool running = C_Is_Running())
    {
        SendTimer.Update();
        Task task;
        Time += SendTimer.GetDeltaTime();

        std::queue<Task>& taskQueue = Get_taskQueue();
        {
            std::unique_lock<std::mutex> lock(Get_queueMutex());
            Get_cv_().wait(lock, [&] {return !taskQueue.empty() || !C_Is_Running(); });
            if (!C_Is_Running()) break;

            task = taskQueue.front();
            taskQueue.pop();
        }

        Send(task);
    }
}

bool UdpBase::PushSendTaskToQueue(const std::vector<uint8_t>& data)
{
    Task task;
    std::mutex& queueMutex_ = this->Get_queueMutex();
    std::queue<Task>& taskQueue_ = this->Get_taskQueue();
    std::condition_variable& cv_ = this->Get_cv_();

    task.Send_data = data;

    if (task.Send_data.empty()) return false;

    std::lock_guard<std::mutex> lock(queueMutex_);
    {
        taskQueue_.push(task); // task를 큐에 추가
    }

    cv_.notify_one();
    return true;
}

