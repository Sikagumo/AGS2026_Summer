#pragma once

#include "NetStructures.h"

class NetManager;

class NetSend
{
public:
	NetSend(NetManager& netManager, int& sendSocketId);

	~NetSend(void) = default;

	void Send(NET_DATA_TYPE type);

private:

	void SendUser(void);

	void SendUsers(void);

	void SendActionHisAll(void);

	void SendBossAction(void);

	void SendAction(void);

	NET_BASIC_DATA MakeBasicData(NET_DATA_TYPE type, std::uint32_t crc);

	void SendUDP_Host(const void* bufptr, int size);

	void SendUDP_Client(const void* bufptr, int size);

	NetManager& netManager_;

	int& sendSocketId_;
};

