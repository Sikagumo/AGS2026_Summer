#include "NetManager.h"
#include <iostream>
#include <thread>
#include <DxLib.h>
#include "../Common/CRC.h"
#include "../Manager/SceneManager.h"
#include "../Net/NetHost.h"
#include "../Net/NetSend.h"
#include "../Net/NetClient.h"
#include "NetManager.h"

NetManager* NetManager::instance_ = nullptr;

void NetManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new NetManager();
	}
}

NetManager& NetManager::GetInstance(void)
{
	return *instance_;
}

void NetManager::Destroy(void)
{
	instance_->Stop();
}

NET_MODE NetManager::GetMode(void) const
{
	return mode_;
}

bool NetManager::IsHost(void) const
{
	return mode_ == NET_MODE::HOST;
}

void NetManager::Run(NET_MODE mode)
{

	isRunning_ = true;

	// ホスト or クライアント
	mode_ = mode;

	int port = 0;
	switch (mode_)
	{
	case NET_MODE::HOST:
		instance_->net_ = new NetHost(*instance_);
		port = RECEIVE_PORT_HOST;
		break;
	case NET_MODE::CLIENT:
		instance_->net_ = new NetClient(*instance_);
		port = RECEIVE_PORT_CLIENT_RANGE + GetRand(899);
		break;
	}

	// 自分のネットユーザ情報
	auto& self = poolShare_.selfJoinUser_;
	self.mode = mode_;
	self.key = GetRand(999999);
	GetMyIPAddress(&self.ip);
	self.port = port;
	self.gameState = GAME_STATE::NONE;

	// 自分のアクション情報
	poolShare_.selfAction_.key = self.key;
	poolShare_.selfActionHis_.key = self.key;

	// UDPを使用した通信を行うソケットハンドルを作成する
	sendSocketId_ = MakeUDPSocket(-1);
	recvSocketId_ = MakeUDPSocket(port);

	// 自分自身をプレイヤーに追加する
	poolShare_.joinUsers_.emplace(self.key, self);

	// データのコピー
	pool_ = poolShare_;

	// 受信処理を行うスレッドを起動する
	recvThread_ = std::thread(&NetManager::UdpReceiveThread, this);

	// 送信用クラスの作成
	netSend_ = new NetSend(*instance_, sendSocketId_);

	frameNo_ = 0;
	isSync_ = false;

}

void NetManager::Update(void)
{

	if (net_ == nullptr)
	{
		// Runが実行されるまで処理しない
		return;
	}

	if (isSync_)
	{

		// TODO 完全同期になっているので、ある程度非同期でも実行できるようにしたい
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
		{
			// 通信データ反映
			DataReflection();

			// 全ユーザの最新フレームが揃うまで待つ
			if (IsSameFrameNo())
			{
				break;
			}

			// 状態別更新(主に送信処理)
			UpdateState();

			// 一定時間待機
			WaitTimer(static_cast<int>(SYNC_TERM_MSEC));
		}

	}
	else
	{
		// 通信データ反映
		DataReflection();
	}

	// 状態別更新(主に送信処理)
	UpdateState();

}

void NetManager::UpdateState(void)
{

	switch (pool_.selfJoinUser_.gameState)
	{
	case GAME_STATE::CONNECTING:
		net_->UpdateConnecting();
		break;
	case GAME_STATE::GOTO_GAME:
		net_->UpdateGotoGame();
		break;
	case GAME_STATE::GAME_PLAYING:
		net_->UpdateGamePlaying();
		break;
	}

}

void NetManager::DataReflection(void)
{

	// 排他制御
	std::lock_guard<std::mutex> lock(poolMutex_);

	// 自分自身をリスト更新

	// 固有データから共有データへコピー
	poolShare_.selfJoinUser_ = pool_.selfJoinUser_;
	poolShare_.selfAction_ = pool_.selfAction_;
	poolShare_.selfAction_.frameNo = frameNo_;
	poolShare_.selfActionHis_ = pool_.selfActionHis_;

	// 自分優先
	ReplaceActionHis(poolShare_.selfActionHis_);

	if (IsHost())
	{
		// ルーム機能はホストデータを正とする
		ReplaceUser(poolShare_.selfJoinUser_);
	}

	// データのコピー
	pool_ = poolShare_;

}

void NetManager::UpdateEndOfFrame(void)
{
	if (isSync_)
	{
		// 同期中のフレーム進行
		NextFrame();
	}
}

void NetManager::Stop(void)
{

	DeleteUDPSocket(sendSocketId_);
	DeleteUDPSocket(recvSocketId_);

	if (isRunning_) {

		isRunning_ = false;

		if (recvThread_.joinable()) {
			recvThread_.join();
		}

	}

}

void NetManager::Send(NET_DATA_TYPE type)
{
	netSend_->Send(type);
}

void NetManager::ChangeGameState(GAME_STATE state)
{

	if (pool_.selfJoinUser_.gameState == state)
	{
		return;
	}

	pool_.selfJoinUser_.gameState = state;

	switch (state)
	{
	case GAME_STATE::NONE:
		break;
	case GAME_STATE::CONNECTING:
		break;
	case GAME_STATE::GOTO_GAME:
		break;
	case GAME_STATE::GAME_PLAYING:
		frameNo_ = 1;
		isSync_ = true;
		pool_.selfAction_.frameNo = frameNo_;
		break;
	}

}

NetManager::NetManager(void)
{
	isRunning_ = false;
	frameNo_ = 0;
	mode_ = NET_MODE::NONE;
	net_ = nullptr;
	netSend_ = nullptr;
	isRunning_ = false;
	recvSocketId_ = -1;
	sendSocketId_ = -1;
	isSync_ = false;
}

NetManager::NetManager(const NetManager& ins)
{
	isRunning_ = false;
	frameNo_ = 0;
	mode_ = NET_MODE::NONE;
	net_ = nullptr;
	netSend_ = nullptr;
	isRunning_ = false;
	recvSocketId_ = -1;
	sendSocketId_ = -1;
	isSync_ = false;
}

NetManager::~NetManager(void)
{
	Stop();
}

void NetManager::UdpReceiveThread(void)
{

	while (isRunning_)
	{

		{
			// 排他制御
			std::lock_guard<std::mutex> lock(poolMutex_);

			// データの受信
			UdpReceiveData();
		}

		GAME_STATE state = GetGameStateSelf();

		switch (state)
		{
		case GAME_STATE::NONE:
			break;
		case GAME_STATE::CONNECTING:
			net_->UdpReceiveThreadConnecting();
			break;
		case GAME_STATE::GOTO_GAME:
			net_->UdpReceiveThreadGotoGame();
			break;
		case GAME_STATE::GAME_PLAYING:
			net_->UdpReceiveThreadGamePlaying();
			break;
		}

		// 受信ループの待機
		WaitTimer(static_cast<int>(NetManager::RECEIVE_TERM_MSEC));

	}

}

void NetManager::UdpReceiveData(void)
{

	const int SIZE_BASIC = sizeof(NET_BASIC_DATA);

	for (int i = 0; i < NetManager::MAX_RECEIVE_NUM; i++)
	{

		char buf[MAX_SEND_BYTES] = { 0 };
		char* bufptr = buf;

		// UDPデータを受信
		int bufSize = NetWorkRecvUDP(
			recvSocketId_, nullptr, nullptr, bufptr, MAX_SEND_BYTES, false);

		if (bufSize == -3)
		{
			// 受信データがない
			break;
		}
		if (bufSize == -1 || bufSize == -2)
		{
			// -1 エラー発生 －２：エラー発生（受信データよりバッファのサイズの方が小さい）
			continue;
		}

		// 基本データサイズチェック
		int headSize = SIZE_BASIC;
		if (bufSize >= headSize)
		{

			// 基本データを先に取得して通信種別で処理を振り分ける
			NET_BASIC_DATA data = GetBasicData(bufptr);
			bufptr += SIZE_BASIC;

			switch (data.type)
			{
			case NET_DATA_TYPE::USER:
			{
				if (bufSize == SIZE_BASIC + sizeof(NET_JOIN_USER))
				{
					NET_JOIN_USER* cast = reinterpret_cast<NET_JOIN_USER*>(bufptr);
					// CRCチェック
					std::uint32_t crc = CRC::Calculate(cast, sizeof(NET_JOIN_USER), CRC::CRC_32());
					if (crc == data.crc)
					{
						ReplaceUser(*cast);
					}
				}
				break;
			}
			case NET_DATA_TYPE::USERS:
			{
				if (bufSize == SIZE_BASIC + sizeof(NET_JOIN_USERS))
				{
					if (!IsHost() && data.key == pool_.hostJoinUser_.key)
					{
						SceneManager::GetInstance().SetTotalGameTime(data.gameTime);
					}
					NET_JOIN_USERS* cast = reinterpret_cast<NET_JOIN_USERS*>(bufptr);
					// CRCチェック
					std::uint32_t crc = CRC::Calculate(cast, sizeof(NET_JOIN_USERS), CRC::CRC_32());
					if (crc == data.crc)
					{
						ReplaceUsers(*cast);
					}
				}
				break;
			}
			case NET_DATA_TYPE::ACTION_HIS_ALL:
			{
				if (bufSize == SIZE_BASIC + sizeof(NET_ACTION_HIS))
				{
					if (!IsHost() && data.key == pool_.hostJoinUser_.key)
					{
						SceneManager::GetInstance().SetTotalGameTime(data.gameTime);
					}
					NET_ACTION_HIS* cast = reinterpret_cast<NET_ACTION_HIS*>(bufptr);
					// CRCチェック
					std::uint32_t crc = CRC::Calculate(cast, sizeof(NET_ACTION_HIS), CRC::CRC_32());
					if (crc == data.crc)
					{
						ReplaceActionHis(*cast);
					}
				}
				break;
			}
			}

		}

	}

}

NET_BASIC_DATA NetManager::GetBasicData(const char* bufptr)
{
	NET_BASIC_DATA ret;
	int size;

	// 通信種別
	size = sizeof(ret.type);
	unsigned char* pType = reinterpret_cast<unsigned char*>(&ret.type);
	memcpy(pType, bufptr, size);
	bufptr += size;

	// ユーザキー
	size = sizeof(ret.key);
	unsigned char* pKey = reinterpret_cast<unsigned char*>(&ret.key);
	memcpy(pKey, bufptr, size);
	bufptr += size;

	// ゲーム時間
	size = sizeof(ret.gameTime);
	unsigned char* pTime = reinterpret_cast<unsigned char*>(&ret.gameTime);
	memcpy(pTime, bufptr, size);
	bufptr += size;

	// CRC
	size = sizeof(ret.crc);
	unsigned char* pCRC = reinterpret_cast<unsigned char*>(&ret.crc);
	memcpy(pCRC, bufptr, size);
	bufptr += size;

	return ret;
}

void NetManager::NextFrame(void)
{
	frameNo_++;
}

const NET_JOIN_USER NetManager::GetSelf(void) const
{
	return pool_.selfJoinUser_;
}

const std::map<int, NET_JOIN_USER> NetManager::GetNetUsers(void) const
{
	return pool_.joinUsers_;
}

void NetManager::ReplaceUser(NET_JOIN_USER entity)
{
	if (poolShare_.joinUsers_.find(entity.key) == poolShare_.joinUsers_.end())
	{
		// 配列にないので追加
		size_t size = poolShare_.joinUsers_.size();
		entity.playerType = static_cast<PLAYER_TYPE>(size);
		poolShare_.joinUsers_.emplace(entity.key, entity);
	}
	else
	{
		// GameStateは戻らないように
		if (poolShare_.joinUsers_[entity.key].gameState <= entity.gameState)
		{
			// プレイヤー種別はホスト優先
			PLAYER_TYPE type = poolShare_.joinUsers_[entity.key].playerType;
			entity.playerType = type;
			poolShare_.joinUsers_[entity.key] = entity;
		}
	}
}

void NetManager::ReplaceUsers(NET_JOIN_USERS entities)
{

	poolShare_.joinUsers_.clear();

	for (const auto& user : entities.users)
	{

		if (user.mode == NET_MODE::HOST
			|| user.mode == NET_MODE::CLIENT)
		{
			poolShare_.joinUsers_.emplace(user.key, user);
		}

		if (user.key == poolShare_.selfJoinUser_.key)
		{
			poolShare_.selfJoinUser_ = user;
		}

		if (user.mode == NET_MODE::HOST)
		{
			poolShare_.hostJoinUser_ = user;
		}

	}

}

IPDATA NetManager::GetHostIp(void) const
{
	return hostIp_;
}

void NetManager::SetHostIp(IPDATA ip)
{
	hostIp_ = ip;
}

int NetManager::GetHostPort(void) const
{
	return RECEIVE_PORT_HOST;
}

GAME_STATE NetManager::GetGameStateHost(void) const
{
	return pool_.hostJoinUser_.gameState;
}

GAME_STATE NetManager::GetGameStateSelf(void) const
{
	return pool_.selfJoinUser_.gameState;
}

bool NetManager::IsSameGameState(GAME_STATE state)
{

	if (pool_.joinUsers_.size() == 0)
	{
		return false;
	}

	for (auto& user : pool_.joinUsers_)
	{
		if (user.second.gameState != state)
		{
			return false;
		}
	}

	return true;

}

void NetManager::ResetAction(void)
{
	pool_.selfAction_.actBits = 0;
}

unsigned int NetManager::GetFrameNo(void) const
{
	return frameNo_;
}

void NetManager::MakeActionHis(NET_ACTION action)
{

	bool isExists = false;
	for (int i = 0; i < NUM_FRAME; i++)
	{
		if (action.frameNo == pool_.selfActionHis_.actions[i].frameNo)
		{
			return;
		}
	}

	unsigned int min = 4294967295;
	int idx = -1;
	for (int i = 0; i < NUM_FRAME; i++)
	{
		if (min > pool_.selfActionHis_.actions[i].frameNo)
		{
			min = pool_.selfActionHis_.actions[i].frameNo;
			idx = i;
		}
	}

	pool_.selfActionHis_.actions[idx] = action;

}

bool NetManager::IsSync(void)
{
	return isSync_;
}

void NetManager::SetAction(PLAYER_ACTION act)
{
	pool_.selfAction_.actBits |= static_cast<int>(act);
}

bool NetManager::IsSameFrameNo(void)
{

	bool ret = false;

	if (pool_.selfJoinUser_.gameState != GAME_STATE::GAME_PLAYING)
	{
		return true;
	}

	if (pool_.joinUserActionHis_.size() != pool_.joinUsers_.size())
	{
		return false;
	}

	unsigned int frameNo = pool_.selfAction_.frameNo;
	if (frameNo == 0)
	{
		return false;
	}

	int cnt = 0;
	for (const auto& his : pool_.joinUserActionHis_)
	{
		for (const auto& act : his.second.actions)
		{
			if (frameNo == act.frameNo)
			{
				cnt += 1;
			}
		}
	}

	if (cnt == pool_.joinUsers_.size())
	{
		ret = true;
	}

	return ret;


}

const NET_ACTION_HIS NetManager::GetSelfActionHis(void) const
{
	return pool_.selfActionHis_;
}

const NET_ACTION NetManager::GetSelfAction(void) const
{
	return pool_.selfAction_;
}

const std::map<int, NET_ACTION_HIS> NetManager::GetActionHis(void) const
{
	return pool_.joinUserActionHis_;
}

void NetManager::ReplaceActionHis(NET_ACTION_HIS entity)
{
	auto& map = poolShare_.joinUserActionHis_;
	if (map.find(entity.key) == map.end())
	{
		map.emplace(entity.key, entity);
	}
	else
	{
		map[entity.key] = entity;
	}
}

bool NetManager::IsAction(int key, PLAYER_ACTION action)
{
	if (pool_.joinUserActionHis_.find(key) != pool_.joinUserActionHis_.end())
	{

		for (const auto& actionHis : pool_.joinUserActionHis_[key].actions)
		{
			if (frameNo_ == actionHis.frameNo)
			{
				return actionHis.actBits & static_cast<int>(action);
			}
		}

	}
	return false;
}

bool NetManager::IsAction(int key, PLAYER_ACTION action, bool isAction)
{
	if (pool_.selfJoinUser_.key == key)
	{
		return isAction;
	}
	return IsAction(key, action);
}
