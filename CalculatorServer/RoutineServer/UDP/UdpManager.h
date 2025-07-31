#pragma once
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include "Recv_To_RoutineServer.h"
#include "Send_To_RoutineServer.h"
class UdpBase;
class UdpManager final
{
public:

	void Release();
	void init(bool is_mainserver, int recvThreadCount, int recvportnumber, int sendThreadCount, int sendportnumber);
	Send_To_RoutineServer& Get_Send_To_Routine();
private:
	std::vector<UdpBase*> UdpManagers;
	Recv_To_RoutineServer Recv_To_Routine;
	Send_To_RoutineServer Send_To_Routine;


};
