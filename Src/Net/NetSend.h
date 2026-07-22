#pragma once
#include "NetStructures.h"

class NetSend
{
public:

	NetSend(int _sendSocketId);

	~NetSend(void) = default;

	void Send(NET_DATA_TYPE _type);

private:
	int sendSocketId_;

	void SendUser(void);

	void SendUsers(void);

	void SendActionHisAll(void);

	void SendBossAction(void);

	void SendAction(void);

	void SendGoGameScene(void);

	NET_BASIC_DATA MakeBasicData(NET_DATA_TYPE _type, std::uint32_t _crc);

	void SendUDP_Host(const void* _bufferPointer, int _dataSize);

	void SendUDP_Client(const void* _bufferPointer, int _dataSize);
};