#pragma once
#include "../NetworkPlatform.h"
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <vector>
#include <string>
// ����ȭ�� ��Ŷ �ý��� ����
enum class PacketType : uint8_t;
enum class SpellType : unsigned int;
class Vec3;
class Object;
class Send_To_RoutineServer;
struct Struct_Transform;
struct Struct_CharacterMesh;
struct Task {
    std::vector<uint8_t> Recv_data;   // ���ſ� ���̳ʸ� ������
    std::vector<uint8_t> Send_data;   // �۽ſ� ���̳ʸ� ������
};

class UdpBase
{
public:
    UdpBase();
    ~UdpBase();

    // �ʱ�ȭ �Լ� (��Ŷ �ý��� �ʱ�ȭ ����)
    virtual void Initialize();

    void Udp_Recv_Tick();
    bool PrepareSendTask(Object* obj);
    void Udp_Send_Tick();

    // ���� �Լ���
    bool PushSendTaskToQueue(const std::vector<uint8_t>& data);

    virtual Task& Get_Task() = 0;

    virtual SOCKET Get_Sock_SendServer() = 0;
    virtual SOCKET Get_Sock_RecvServer() = 0;

    virtual sockaddr_in& Get_Sockaddr_SendServer() = 0;
    virtual sockaddr_in& Get_Sockaddr_RecvServer() = 0;
    virtual std::queue<Task>& Get_taskQueue() = 0;
    virtual std::mutex& Get_queueMutex() = 0;
    virtual std::condition_variable& Get_cv_() = 0;
    virtual std::vector<std::thread>& Get_workers_() = 0;
    virtual bool C_Is_Running() = 0;
    virtual int C_Get_WorkCount() = 0;
    virtual int C_Get_Send_Port()const = 0;
    virtual int C_Get_Recv_Port()const = 0;
    virtual std::string C_Get_Send_ip()const = 0;
    virtual void Release() = 0;
    virtual void Set_Running(bool Change_Running) = 0;

private:
    int Recv(Task& task, int Len = MAX_BUFFER_SIZE);
    bool Send(const Task& out_buffer, int Len = MAX_BUFFER_SIZE);
    void ReceiveLoop();
    void Recv_WorkerThread(int id);
    void Send_WorkerThread(int id);
  
};