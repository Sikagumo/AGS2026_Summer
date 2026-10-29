#include "NetManager.h"
#include "../../Net/NetHost.h"
#include "../../Net/NetClient.h"
#include "../../Net/NetSend.h"
#include "../../Common/CRC.h"
#include "TimeManager.h" 
#include <cstdlib>

namespace
{
	// マジックナンバー排除用の定数
	constexpr int INITIAL_SOCKET_ID = -1;        // ソケットIDの初期値
	constexpr int INITIAL_KEY_VALUE = -1;        // ユーザーキーの初期値
	constexpr unsigned int INITIAL_FRAME_NO = 0; // フレーム番号の初期値
	constexpr int INITIAL_ANIM_ID = 0;           // アニメーションIDの初期値
	constexpr float VALUE_ZERO_F = 0.0f;         // 浮動小数点数のゼロ（初期化やリセット用）
	constexpr int RANDOM_MAX_KEY = 99999;        // ユーザーキー生成時のランダム値の最大値
}

NetManager* NetManager::instance_ = nullptr;

void NetManager::CreateInstance(void)
{
	if (!instance_)
	{
		instance_ = new NetManager();
	}
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
	, receiveSocketId_(INITIAL_SOCKET_ID)
	, sendSocketId_(INITIAL_SOCKET_ID)
	, roomWordId_(-1)
	, hostIp_(LOCALHOST_IP)
	, hasReceivedGoGame_(false)
	, gameTime_(DEFAULT_GAME_TIME)
	, hostTimeoutTimer_(VALUE_ZERO_F)
	, connectionTimeout_(CONNECTION_TIMEOUT_DEFAULT)
{
	selfActionHistory_.key = INITIAL_KEY_VALUE;
	for (int i = 0; i < NUM_FRAME; ++i)
	{
		selfActionHistory_.actions[i].key = INITIAL_KEY_VALUE;
		selfActionHistory_.actions[i].frameNo = INITIAL_FRAME_NO;
		selfActionHistory_.actions[i].animId = INITIAL_ANIM_ID;
	}
}

NetManager::~NetManager(void)
{
	Stop();
}

void NetManager::Run(NET_MODE _mode)
{
	if (isRunning_)
	{
		return;
	}

	mode_ = _mode;
	isRunning_ = true;

	// ランダムキーの生成
	netPool_.selfUser.key = (rand() + GetNowCount()) % RANDOM_MAX_KEY + 1;

	netPool_.selfUser.mode = mode_;
	netPool_.selfUser.gameState = GAME_STATE::CONNECTING;
	netPool_.selfUser.roomWordId = roomWordId_;

	GetMyIPAddress(&netPool_.selfUser.ipAddress);

	if (mode_ == NET_MODE::HOST)
	{
		netPool_.selfUser.port = HOST_PORT;
		receiveSocketId_ = MakeUDPSocket(HOST_PORT);
		sendSocketId_ = receiveSocketId_;

		netBase_ = new NetHost(*this);
	}
	else if (mode_ == NET_MODE::CLIENT)
	{
		receiveSocketId_ = MakeUDPSocket(INITIAL_SOCKET_ID);
		sendSocketId_ = receiveSocketId_;

		netBase_ = new NetClient(*this);
	}

	netSend_ = new NetSend(sendSocketId_);
}

void NetManager::Stop(void)
{
	if (!isRunning_)
	{
		return;
	}

	if (receiveSocketId_ != INITIAL_SOCKET_ID)
	{
		DeleteUDPSocket(receiveSocketId_);
		receiveSocketId_ = INITIAL_SOCKET_ID;
		sendSocketId_ = INITIAL_SOCKET_ID;
	}

	delete netBase_;
	netBase_ = nullptr;

	delete netSend_;
	netSend_ = nullptr;

	isRunning_ = false;
	mode_ = NET_MODE::NONE;

	netPool_.remoteUsers.clear();
	netPool_.selfUser = NET_JOIN_USER();
	netPool_.bossAction = NET_BOSS_ACTION();

	remoteActionHistory_.clear();
	hasReceivedGoGame_ = false;

	gameTime_ = DEFAULT_GAME_TIME;

	hostTimeoutTimer_ = VALUE_ZERO_F;
	clientTimeoutTimers_.clear();
}

void NetManager::Update(void)
{
	if (!isRunning_)
	{
		return;
	}

	UdpReceiveData();

	const float deltaTime = TimeManager::GetInstance().GetDeltaTime();

	if (mode_ == NET_MODE::CLIENT)
	{
		hostTimeoutTimer_ += deltaTime;
	}
	else if (mode_ == NET_MODE::HOST)
	{
		std::lock_guard<std::mutex> lock(poolMutex_);
		for (auto& pair : clientTimeoutTimers_)
		{
			pair.second += deltaTime;
		}

		for (auto it = clientTimeoutTimers_.begin(); it != clientTimeoutTimers_.end(); )
		{
			if (it->second > connectionTimeout_)
			{
				netPool_.remoteUsers.erase(it->first);
				remoteActionHistory_.erase(it->first);
				it = clientTimeoutTimers_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	if (netBase_)
	{
		NET_JOIN_USER selfUser = GetSelfUser();
		switch (selfUser.gameState)
		{
		case GAME_STATE::CONNECTING:
			netBase_->UpdateConnecting();
			break;

		case GAME_STATE::GOTO_GAME:
			netBase_->UpdateGotoGame();
			break;

		case GAME_STATE::GAME_PLAYING:
			netBase_->UpdateGamePlaying();
			break;

		default:
			break;
		}
	}
}

void NetManager::Send(NET_DATA_TYPE _type)
{
	if (netSend_)
	{
		netSend_->Send(_type);
	}
}

void NetManager::AddSelfAction(const NET_ACTION& _action)
{
	std::lock_guard<std::mutex> lock(poolMutex_);

	// 履歴を1つずつ後ろにずらす
	for (int i = NUM_FRAME - 1; i > 0; --i)
	{
		selfActionHistory_.actions[i] = selfActionHistory_.actions[i - 1];
	}

	// 先頭0に最新のアクションを入れる
	selfActionHistory_.actions[0] = _action;
	selfActionHistory_.key = GetMyKey();
}

void NetManager::UdpReceiveData(void)
{
	while (CheckNetWorkRecvUDP(receiveSocketId_))
	{
		IPDATA senderIp;                  // 送信元のIPアドレス
		int senderPort = 0;               // 送信元のポート番号
		char buffer[MAX_SEND_BYTES];      // 受信バッファ

		int receiveSize = NetWorkRecvUDP(receiveSocketId_, &senderIp, &senderPort, buffer, sizeof(buffer), FALSE);

		if (receiveSize >= sizeof(NET_BASIC_DATA))
		{
			NET_BASIC_DATA* header = reinterpret_cast<NET_BASIC_DATA*>(buffer);

			if (mode_ == NET_MODE::CLIENT)
			{
				hostTimeoutTimer_ = VALUE_ZERO_F;
			}

			if (mode_ == NET_MODE::HOST && header->type == NET_DATA_TYPE::USER)
			{
				// 受信したユーザー情報
				NET_JOIN_USER* user = reinterpret_cast<NET_JOIN_USER*>(buffer + sizeof(NET_BASIC_DATA));

				if (user->roomWordId != roomWordId_)
				{
					continue;
				}

				{
					std::lock_guard<std::mutex> lock(poolMutex_);

					user->ipAddress = senderIp;
					user->port = senderPort;
					netPool_.remoteUsers[user->key] = *user;
				}

				Send(NET_DATA_TYPE::USERS);
			}
			else if (mode_ == NET_MODE::CLIENT && header->type == NET_DATA_TYPE::USERS)
			{
				SetHostIp(senderIp);

				// 受信した複数のユーザー情報
				NET_JOIN_USERS* users = reinterpret_cast<NET_JOIN_USERS*>(buffer + sizeof(NET_BASIC_DATA));
				std::lock_guard<std::mutex> lock(poolMutex_);

				for (int i = 0; i < MAX_PLAYERS; ++i)
				{
					// modeがNONE以外なら、ホスト自身も含めてすべてリストに入れる
					if (users->users[i].mode != NET_MODE::NONE)
					{
						// 自分の情報はスキップ
						if (users->users[i].key == GetMyKey())
						{
							continue;
						}

						netPool_.remoteUsers[users->users[i].key] = users->users[i];
					}
				}
			}
			else if (header->type == NET_DATA_TYPE::ACTION_HIST_ALL)
			{
				// 受信したアクション履歴
				NET_ACTION_HIS* history = reinterpret_cast<NET_ACTION_HIS*>(buffer + sizeof(NET_BASIC_DATA));
				std::lock_guard<std::mutex> lock(poolMutex_);

				// 自分の送ったデータが跳ね返って来たものは無視し、他人のデータを保存する
				if (history->key != GetMyKey())
				{
					remoteActionHistory_[history->key] = *history;

					if (mode_ == NET_MODE::HOST)
					{
						clientTimeoutTimers_[history->key] = VALUE_ZERO_F;
					}
				}

				if (mode_ == NET_MODE::CLIENT && netPool_.selfUser.gameState != GAME_STATE::GAME_PLAYING)
				{
					hasReceivedGoGame_ = true;
				}
			}
			else if (header->type == NET_DATA_TYPE::GO_GAME_SCENE)
			{
				hasReceivedGoGame_ = true;
			}
			else if (header->type == NET_DATA_TYPE::BOSS_ACTION)
			{
				// 受信したボスのアクション
				NET_BOSS_ACTION* bossAction = reinterpret_cast<NET_BOSS_ACTION*>(buffer + sizeof(NET_BASIC_DATA));
				std::lock_guard<std::mutex> lock(poolMutex_);

				// クライアントは受信したボスの最新状態をローカルのプールに保存する
				if (!IsHost())
				{
					netPool_.bossAction = *bossAction;
					gameTime_ = header->gameTime;

					if (netPool_.selfUser.gameState != GAME_STATE::GAME_PLAYING)
					{
						hasReceivedGoGame_ = true;
					}
				}
			}
			else if (header->type == NET_DATA_TYPE::ACTION_HIST_RELAY)
			{
				// 受信した全ユーザーのアクション履歴
				NET_ACTION_HIS_ALL* allHistory = reinterpret_cast<NET_ACTION_HIS_ALL*>(buffer + sizeof(NET_BASIC_DATA));
				std::lock_guard<std::mutex> lock(poolMutex_);

				// クライアントのみが受信・処理する
				if (!IsHost())
				{
					for (int i = 0; i < allHistory->count; ++i)
					{
						int key = allHistory->histories[i].key;

						// 自分自身のデータは無視する
						if (key == GetMyKey())
						{
							continue;
						}
						remoteActionHistory_[key] = allHistory->histories[i];
					}
				}
			}
			else if (header->type == NET_DATA_TYPE::LEAVE_ROOM)
			{
				std::lock_guard<std::mutex> lock(poolMutex_);

				// 送信者のkeyを使って即座にリストから削除する
				int senderKey = header->key;

				if (netPool_.remoteUsers.find(senderKey) != netPool_.remoteUsers.end())
				{
					netPool_.remoteUsers.erase(senderKey);
					remoteActionHistory_.erase(senderKey);
					clientTimeoutTimers_.erase(senderKey);
				}
			}
		}
	}
}

bool NetManager::GetIsConnectionLost(void) const
{
	std::lock_guard<std::mutex> lock(poolMutex_);

	if (mode_ == NET_MODE::CLIENT)
	{
		return (hostTimeoutTimer_ > CONNECTION_TIMEOUT_DEFAULT);
	}
	else if (mode_ == NET_MODE::HOST)
	{
		// 現在ルームに登録されているユーザーのキーだけをチェックする
		for (const auto& userPair : netPool_.remoteUsers)
		{
			auto it = clientTimeoutTimers_.find(userPair.first);

			// タイマーがまだ登録されていない場合はスキップ
			if (it == clientTimeoutTimers_.end())
			{
				continue;
			}

			if (it->second > CONNECTION_TIMEOUT_DEFAULT)
			{
				return true;
			}
		}
	}

	return false;
}