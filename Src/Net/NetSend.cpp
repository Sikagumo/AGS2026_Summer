#include "NetSend.h"
#include "../Manager/System/NetManager.h"

std::uint32_t CalculateCRC(const void* data, size_t size)
{
	return 0;
}

NetSend::NetSend(NetManager& netManager, int& sendSocketId)
	: netManager_(netManager)
	, sendSocketId_(sendSocketId)
{

}

void NetSend::Send(NET_DATA_TYPE type)
{
	switch (type)
	{
	case NET_DATA_TYPE::USER:

		SendUser();

		break;

	case NET_DATA_TYPE::USERS:

		SendUsers();

		break;

	case NET_DATA_TYPE::ACTION_HIST_ALL:

		SendActionHisAll();

		break;

	case NET_DATA_TYPE::BOSS_ACTOION:

		SendBossAction();

		break;
	}
}

NET_BASIC_DATA NetSend::MakeBasicData(NET_DATA_TYPE type, std::uint32_t crc)
{
	NET_BASIC_DATA data;

	data.type = type;

	data.key = 0;

	data.gameTime = 0.0f;

	data.crc = crc;

	return data;
}

void NetSend::SendUser(void)
{
}

void NetSend::SendUsers(void)
{
	NET_JOINT_USER myInfo = {};

	NET_BASIC_DATA basicData = MakeBasicData(NET_DATA_TYPE::USER, 0);
	char buffer[sizeof(NET_BASIC_DATA) + sizeof(NET_JOINT_USER)];

	memcpy(buffer, &basicData, sizeof(basicData));
	memcpy(buffer + sizeof(basicData), &myInfo, sizeof(myInfo));

	SendUDP_Client(buffer, sizeof(buffer));
}

void NetSend::SendActionHisAll(void)
{

}

void NetSend::SendBossAction(void)
{
	NET_BOSS_ACTION BossAction = {};

	std::uint32_t crc = CalculateCRC(&BossAction, sizeof(BossAction));

	NET_BASIC_DATA basicData = MakeBasicData(NET_DATA_TYPE::BOSS_ACTOION, 0);

	char buffer[sizeof(NET_BASIC_DATA) + sizeof(NET_BOSS_ACTION)];

	memcpy(buffer, &basicData, sizeof(basicData));

	memcpy(buffer + sizeof(basicData), &BossAction, sizeof(BossAction));

	SendUDP_Host(buffer, sizeof(buffer));
}

void NetSend::SendAction(void)
{
}

void NetSend::SendUDP_Host(const void* bufptr, int size)
{

}

void NetSend::SendUDP_Client(const void* bufptr, int size)
{
	IPDATA targetIp = netManager_.GetHostIp();

	NetWorkSendUDP(sendSocketId_, targetIp, 65000, bufptr, size);
}
