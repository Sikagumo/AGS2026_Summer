#pragma once

#include <mutex>
#include <map>

#include "../../Net/NetStructures.h"

class NetBase;
class NetSend;

class NetManager
{
public:

	static void CreateInstance(void);

	static NetManager& GetInstance(void);

	static void DestroyInstance(void);

	void Run(NET_MODE mode);

	void Stop(void);

	void Update(void);

	void Send(NET_DATA_TYPE type);

	NET_MODE GetMode(void) const { return mode_; }

	int GetSendSocket(void) const { return sendSocketId_; }

	bool IsHost(void) const { return mode_ == NET_MODE::HOST; }

	void SetSelfInfo(const NET_JOINT_USER& nfo);

	unsigned int GetFrameNo(void) const { return frameNo_; }

	std::mutex& GetMutex(void) { return poolMutex_; }

	void SetHostIp(IPDATA ip) { hostIp_ = ip; }

	IPDATA GetHostIp(void) const { return hostIp_; }

private:

	struct NET_POOL
	{
		NET_JOINT_USER selfUser_;

		std::map<int, NET_JOINT_USER> remoteUsers_;

		NET_ACTION selfAction_;

		NET_BOSS_ACTION bossAction_;
	} pool_;

	NetManager(void);

	~NetManager(void);

	static NetManager* instance_;

	NetBase* netBase_;

	NetSend* netSend_;

	IPDATA hostIp_;

	bool isRunning_;

	NET_MODE mode_;

	unsigned int frameNo_;

	std::thread recvThread_;

	std::mutex poolMutex_;

	int recvSocketId_;

	int sendSocketId_;

	void RecvLoop(void);
};

