#include "UdpManager.h"
#include "UdpBase.h"
void UdpManager::Release()
{
	for (UdpBase* It : UdpManagers)
	{

		It->Release();

	}
	std::this_thread::sleep_for(std::chrono::seconds(10));
}

void UdpManager::init(bool is_mainserver, int recvThreadCount, int recvportnumber, int sendThreadCount, int sendportnumber)
{
	//recvportnumber 9009
	//sendportnumber 9012
	Recv_To_Routine.Init("127.0.0.1", recvportnumber, recvThreadCount);//9009 루틴에게 리시브한거 9010 루틴에게 보낼거
	UdpManagers.push_back(&Recv_To_Routine);
	std::thread Recv_TICK(&Recv_To_RoutineServer::Udp_Recv_Tick, UdpManagers[0]);
	Recv_TICK.detach();
	Send_To_Routine.Init("127.0.0.1", sendportnumber, sendThreadCount);
	UdpManagers.push_back(&Send_To_Routine);
	std::thread Send_TICK(&Send_To_RoutineServer::Udp_Send_Tick, UdpManagers[1]);
	Send_TICK.detach();
	return;



}

Send_To_RoutineServer& UdpManager::Get_Send_To_Routine()
{

	return Send_To_Routine;
}

