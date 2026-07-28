#include "NetSend.h"

#include <cstring>
#include <Dxlib.h>

#include "../Manager/System/NetManager.h"
#include "../Common/CRC.h" 

NetSend::NetSend(int _sendSocketId)
	: sendSocketId_(_sendSocketId)
{
}

void NetSend::Send(NET_DATA_TYPE _type)
{
	switch (_type)
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

	case NET_DATA_TYPE::GO_GAME_SCENE:
		SendGoGameScene();
		break;
	}
}

NET_BASIC_DATA NetSend::MakeBasicData(NET_DATA_TYPE _type, std::uint32_t _crc)
{
	NET_BASIC_DATA data;

	data.type = _type;
	data.key = NetManager::GetInstance().GetMyKey();
	data.gameTime = NetManager::GetInstance().GetGameTime(); ;
	data.crc = _crc;

	return data;
}

void NetSend::SendUser(void)
{
	// 【クライアント用】自分の情報をホストへ送る
	NET_JOIN_USER self = NetManager::GetInstance().GetSelfUser();
	uint32_t crc = CRC::Calculate(&self, sizeof(NET_JOIN_USER), CRC::CRC_32());
	NET_BASIC_DATA basicData = MakeBasicData(NET_DATA_TYPE::USER, 0);

	char buffer[MAX_SEND_BYTES];
	memcpy(buffer, &basicData, sizeof(NET_BASIC_DATA));
	memcpy(buffer + sizeof(NET_BASIC_DATA), &self, sizeof(NET_JOIN_USER));

	int sendSize = sizeof(NET_BASIC_DATA) + sizeof(NET_JOIN_USER);

	// ホストへ向けて発射
	SendUDP_Host(buffer, sendSize);
}

void NetSend::SendUsers(void)
{
	// 【ホスト用】全員のリストを各クライアントへ送る
	NET_JOIN_USERS usersData;

	// 配列を一旦初期化
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		usersData.users[i].mode = NET_MODE::NONE;
	}

	// 自分の情報（ホスト）を配列にセット
	NET_JOIN_USER self = NetManager::GetInstance().GetSelfUser();
	usersData.users[0] = self;

	// 接続してきているクライアント達を配列にセット
	auto users = NetManager::GetInstance().GetNetUsers();
	int idx = 1;
	for (const auto& pair : users)
	{
		if (idx >= MAX_PLAYERS) break;
		usersData.users[idx] = pair.second;
		idx++;
	}

	uint32_t crc = CRC::Calculate(&usersData, sizeof(NET_JOIN_USERS), CRC::CRC_32());
	NET_BASIC_DATA basicData = MakeBasicData(NET_DATA_TYPE::USERS, 0);

	char buffer[MAX_SEND_BYTES];
	memcpy(buffer, &basicData, sizeof(NET_BASIC_DATA));
	memcpy(buffer + sizeof(NET_BASIC_DATA), &usersData, sizeof(NET_JOIN_USERS));

	int sendSize = sizeof(NET_BASIC_DATA) + sizeof(NET_JOIN_USERS);

	// 接続済みクライアント全員へ向けて発射
	SendUDP_Client(buffer, sendSize);
}

void NetSend::SendActionHisAll(void)
{
	NET_ACTION_HIS myActionHis = NetManager::GetInstance().GetSelfActionHis();

	myActionHis.key = NetManager::GetInstance().GetMyKey();

	uint32_t crc = CRC::Calculate(&myActionHis, sizeof(NET_ACTION_HIS), CRC::CRC_32());
	NET_BASIC_DATA basicData = MakeBasicData(NET_DATA_TYPE::ACTION_HIST_ALL, 0);
	char buffer[MAX_SEND_BYTES];
	
	memcpy(buffer, &basicData, sizeof(NET_BASIC_DATA));
	memcpy(buffer + sizeof(NET_BASIC_DATA), &myActionHis, sizeof(NET_ACTION_HIS));
	
	int sendSize = sizeof(NET_BASIC_DATA) + sizeof(NET_ACTION_HIS);
	
	if (NetManager::GetInstance().IsHost())
	{
		SendUDP_Client(buffer, sendSize);
	}
	else
	{
		SendUDP_Host(buffer, sendSize);
	}
}

void NetSend::SendBossAction(void)
{
	// ボスの情報はホストしか送信しない
	if (!NetManager::GetInstance().IsHost())
	{
		return;
	}

	// ボスの最新状態を取得する
	NET_BOSS_ACTION bossAction = NetManager::GetInstance().GetBossAction();
	uint32_t crc = CRC::Calculate(&bossAction, sizeof(NET_BOSS_ACTION), CRC::CRC_32());
	NET_BASIC_DATA basicData = MakeBasicData(NET_DATA_TYPE::BOSS_ACTOION, 0);

	char buffer[MAX_SEND_BYTES];
	memcpy(buffer, &basicData, sizeof(NET_BASIC_DATA));
	memcpy(buffer + sizeof(NET_BASIC_DATA), &bossAction, sizeof(NET_BOSS_ACTION));

	int sendSize = sizeof(NET_BASIC_DATA) + sizeof(NET_BOSS_ACTION);

	SendUDP_Client(buffer, sendSize);
}

void NetSend::SendAction(void)
{
}

void NetSend::SendUDP_Host(const void* _bufferPointer, int _dataSize)
{
	IPDATA hostIp = NetManager::GetInstance().GetHostIp();
	NetWorkSendUDP(sendSocketId_, hostIp, HOST_PORT, _bufferPointer, _dataSize);
}

void NetSend::SendUDP_Client(const void* _bufferPointer, int _dataSize)
{
	// ホストは接続済みの全クライアントへ送信する
	int myKey = NetManager::GetInstance().GetMyKey();
	auto users = NetManager::GetInstance().GetNetUsers();

	for (const auto& userPair : users)
	{
		const NET_JOIN_USER& remoteUser = userPair.second;
		if (remoteUser.mode == NET_MODE::NONE || remoteUser.key == myKey) continue;

		NetWorkSendUDP(sendSocketId_, remoteUser.ip, remoteUser.port, _bufferPointer, _dataSize);
	}
}

void NetSend::SendGoGameScene(void)
{
	// ホストしか送らない
	if (!NetManager::GetInstance().IsHost()) { return; }

	// ペイロードなし、ヘッダーのみ送信
	NET_BASIC_DATA basicData = MakeBasicData(NET_DATA_TYPE::GO_GAME_SCENE, 0);

	char buffer[sizeof(NET_BASIC_DATA)];
	memcpy(buffer, &basicData, sizeof(NET_BASIC_DATA));

	SendUDP_Client(buffer, sizeof(NET_BASIC_DATA));
}