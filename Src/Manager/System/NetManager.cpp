#include "NetManager.h"
#include "../../Net/NetHost.h"
#include "../../Net/NetClient.h"
#include "../../Net/NetSend.h"

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

void NetManager::Destroy(void)
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
	, frameNo_(0)
	, recvSocketId_(-1)
	, sendSocketId_(-1)
	, hostIp_(127, 0, 0, 1)
{

}

NetManager::~NetManager(void)
{
	Stop();
}

void NetManager::Run(NET_MODE mode)
{
	if (isRunning_) return;

	mode_ = mode;

	recvSocketId_ = MakeUDPSocket(65000);

	sendSocketId_ = MakeUDPSocket(0);

	if (mode == NET_MODE::HOST)
	{
		netBase_ = new NetHost(*this);
	}
	else
	{
		netBase_ = new NetClient(*this);
	}

	netSend_ = new NetSend(*this, sendSocketId_);

	isRunning_ = true;

	recvThread_ = std::thread(&NetManager::RecvLoop, this);
}

void NetManager::Stop(void)
{
	isRunning_ = false;

	if (recvThread_.joinable())
	{
		recvThread_.join();
	}

	delete netBase_;

	netBase_ = nullptr;

	delete netSend_;

	netSend_ = nullptr;
}

void NetManager::Update(void)
{
	if (!netBase_)return;

	frameNo_++;

	GAME_STATE state = pool_.selfUser_.state;

	switch (state)
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

	}
}

void NetManager::Send(NET_DATA_TYPE type)
{
	if (netSend_)
	{
		netSend_->Send(type);
	}
}

void NetManager::SetSelfInfo(const NET_JOINT_USER& info)
{
	std::lock_guard<std::mutex> Lock(poolMutex_);

	pool_.selfUser_ = info;
}

void NetManager::RecvLoop(void)
{
	char buffer[4096];

	IPDATA senderIp;

	int senderPort;

	while (isRunning_)
	{
		if (CheckNetWorkRecvUDP(recvSocketId_) > 0)
		{
			int size = NetWorkRecvUDP(recvSocketId_, &senderIp, &senderPort, buffer, sizeof(buffer), FALSE);

			if (size >= sizeof(NET_BASIC_DATA))
			{
				// 送信元のIPを表示してみる
				printfDx("受信! from %d.%d.%d.%d サイズ:%d\n",
					senderIp.d1, senderIp.d2, senderIp.d3, senderIp.d4, size);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}