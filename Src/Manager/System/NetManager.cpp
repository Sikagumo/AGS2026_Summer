#include "NetManager.h"
#include "../../Net/NetHost.h"
#include "../../Net/NetClient.h"
#include "../../Net/NetSend.h"
#include <cstdlib>

NetManager* NetManager::instance_ = nullptr;

void NetManager::CreateInstance(void)
{
	if (!instance_) instance_ = new NetManager();
}

NetManager& NetManager::GetInstance(void)
{
	return *instance_;
}

void NetManager::DestroyInstance(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

NetManager::NetManager(void)
	: netBase_(nullptr)
	, netSend_(nullptr)
	, isRunning_(false)
	, mode_(NET_MODE::NONE)
	, recvSocketId_(-1)
	, sendSocketId_(-1)
	, roomWordId_(-1)
	, hostIp_(LOCALHOST_IP)
	, hasReceivedGoGame_(false)
{
	selfActionHis_.key = -1;
	for (int i = 0; i < NUM_FRAME; ++i) 
	{
		selfActionHis_.actions[i].key = -1;
		selfActionHis_.actions[i].frameNo = 0;
		selfActionHis_.actions[i].animId = 0;
	}
}

NetManager::~NetManager(void)
{
	Stop();
}

void NetManager::Run(NET_MODE _mode)
{
	if (isRunning_) { return; }

	mode_ = _mode;
	isRunning_ = true;

	pool_.selfUser_.key = rand() % 99999 + 1;
	pool_.selfUser_.mode = mode_;
	pool_.selfUser_.gameState = GAME_STATE::CONNECTING;
	pool_.selfUser_.roomWordId = roomWordId_;

	GetMyIPAddress(&pool_.selfUser_.ip);

	if (mode_ == NET_MODE::HOST)
	{
		pool_.selfUser_.port = HOST_PORT;
		recvSocketId_ = MakeUDPSocket(HOST_PORT);
		sendSocketId_ = recvSocketId_;

		netBase_ = new NetHost(*this);
	}
	else if (mode_ == NET_MODE::CLIENT)
	{
		recvSocketId_ = MakeUDPSocket(-1);
		sendSocketId_ = recvSocketId_;

		netBase_ = new NetClient(*this);
	}

	netSend_ = new NetSend(sendSocketId_);
}

void NetManager::Stop(void)
{
	if (!isRunning_) return;

	if (recvSocketId_ != -1)
	{
		DeleteUDPSocket(recvSocketId_);
		recvSocketId_ = -1;
		sendSocketId_ = -1;
	}

	delete netBase_; netBase_ = nullptr;
	delete netSend_; netSend_ = nullptr;

	isRunning_ = false;
	mode_ = NET_MODE::NONE;
	pool_.remoteUsers_.clear();
}
void NetManager::Update(void)
{
	if (!isRunning_) return;

	while (CheckNetWorkRecvUDP(recvSocketId_) == TRUE)
	{
		IPDATA senderIp;
		int senderPort;
		char buffer[MAX_SEND_BYTES];
		int recvSize = NetWorkRecvUDP(recvSocketId_, &senderIp, &senderPort, buffer, 
			sizeof(buffer), FALSE);
		
		if (recvSize >= sizeof(NET_BASIC_DATA))
		{
			NET_BASIC_DATA* header = reinterpret_cast<NET_BASIC_DATA*>(buffer);

			if (mode_ == NET_MODE::HOST && header->type == NET_DATA_TYPE::USER)
			{
				NET_JOIN_USER* user = reinterpret_cast<NET_JOIN_USER*>(buffer + 
					sizeof(NET_BASIC_DATA));

				if (user->roomWordId != roomWordId_) return;

				std::lock_guard<std::mutex> lock(poolMutex_);

				// リストにいない新しいキーなら「通信成功」を出す
				if (pool_.remoteUsers_.find(user->key) == pool_.remoteUsers_.end())
				{
					printfDx("【HOST】クライアント(Key:%d)との通信成功！\n", user->key);
				}

				user->ip = senderIp;
				user->port = senderPort;
				pool_.remoteUsers_[user->key] = *user;
			}
			else if (mode_ == NET_MODE::CLIENT && header->type == NET_DATA_TYPE::USERS)
			{
				SetHostIp(senderIp);

				NET_JOIN_USERS* users = reinterpret_cast<NET_JOIN_USERS*>(buffer + 
					sizeof(NET_BASIC_DATA));
				std::lock_guard<std::mutex> lock(poolMutex_);

				for (int i = 0; i < MAX_PLAYERS; ++i)
				{
					// modeがNONE以外なら、ホスト自身も含めてすべてリストに入れる
					if (users->users[i].mode != NET_MODE::NONE)
					{
						// 自分の情報はスキップ
						if (users->users[i].key == GetMyKey()) continue;

						// リストに登録
						if (pool_.remoteUsers_.find(users->users[i].key) 
							== pool_.remoteUsers_.end())
						{
							printfDx("【CLIENT】ユーザー(Key:%d)をリストに追加しました！\n", 
								users->users[i].key);
						}
						pool_.remoteUsers_[users->users[i].key] = users->users[i];
					}
				}
			}
			else if (header->type == NET_DATA_TYPE::ACTION_HIST_ALL)
			{
				NET_ACTION_HIS* his = reinterpret_cast<NET_ACTION_HIS*>(buffer 
					+ sizeof(NET_BASIC_DATA));
				std::lock_guard<std::mutex> look(poolMutex_);

				// 自分の送ったデータが跳ね返って来たものは無視し、他人のデータを保存する
				if (his->key != GetMyKey())
				{
					remoteActionHis_[his->key] = *his;
				}
			}
			else if (header->type == NET_DATA_TYPE::GO_GAME_SCENE)
			{
				hasReceivedGoGame_ = true;
			}
		}
	}

	if (netBase_)
	{
		NET_JOIN_USER self = GetSelfUser();
		switch (self.gameState)
		{
		case GAME_STATE::CONNECTING:  netBase_->UpdateConnecting(); break;
		case GAME_STATE::GOTO_GAME:    netBase_->UpdateGotoGame();   break;
		case GAME_STATE::GAME_PLAYING: netBase_->UpdateGamePlaying(); break;
		}
	}
}

void NetManager::Send(NET_DATA_TYPE _type)
{
	if (netSend_) netSend_->Send(_type);
}

void NetManager::SetHostIp(IPDATA _ip)
{
	hostIp_ = _ip;
}

NET_JOIN_USER NetManager::GetSelfUser(void) const
{
	std::lock_guard<std::mutex> lock(poolMutex_);
	return pool_.selfUser_;
}

std::map<int, NET_JOIN_USER> NetManager::GetNetUsers(void) const
{
	std::lock_guard<std::mutex> lock(poolMutex_);
	return pool_.remoteUsers_;
}

void NetManager::SetSelfInfo(const NET_JOIN_USER& info)
{
	std::lock_guard<std::mutex> lock(poolMutex_);
	pool_.selfUser_ = info;
}

void NetManager::SetRoomWordId(int _id)
{
	roomWordId_ = _id;
}

void NetManager::AddSelfAction(const NET_ACTION& _action)
{
	std::lock_guard<std::mutex> lock(poolMutex_);

	// 履歴を1つずつ後ろにずらす
	for (int i = NUM_FRAME - 1; i > 0; --i)
	{
		selfActionHis_.actions[i] = selfActionHis_.actions[i - 1];
	}

	// 先頭0に最新のアクションを入れる
	selfActionHis_.actions[0] = _action;
	selfActionHis_.key = GetMyKey();
}

NET_ACTION_HIS NetManager::GetSelfActionHis(void) const
{
	std::lock_guard<std::mutex> lock(poolMutex_);
	return selfActionHis_;
}

std::map<int, NET_ACTION_HIS> NetManager::GetRemoteActionHis(void) const
{
	std::lock_guard<std::mutex> lock(poolMutex_);
	return remoteActionHis_;
}

void NetManager::ResetGoGame(void)
{
	hasReceivedGoGame_ = false;
}