#pragma once

#include <mutex>
#include <map>
#include <vector>
#include "../../Net/NetStructures.h"

class NetBase;
class NetSend;

class NetManager
{
public:
	static void CreateInstance(void);
	static NetManager& GetInstance(void);
	static void DestroyInstance(void);

	void Run(NET_MODE _mode);
	void Stop(void);
	void Update(void);
	void Send(NET_DATA_TYPE _type);

	NET_MODE GetMode(void) const { return mode_; }
	bool IsHost(void) const { return mode_ == NET_MODE::HOST; }
	int GetMyKey(void) const { return pool_.selfUser_.key; }
	void SetHostIp(IPDATA _ip);
	IPDATA GetHostIp(void) const { return hostIp_; }
	NET_JOIN_USER GetSelfUser(void) const;
	std::map<int, NET_JOIN_USER> GetNetUsers(void) const;
	void SetSelfInfo(const NET_JOIN_USER& info);

private:
	NetManager(void);
	~NetManager(void);

	static NetManager* instance_;

	NetBase* netBase_;
	NetSend* netSend_;

	bool isRunning_;
	NET_MODE mode_;
	int recvSocketId_;
	int sendSocketId_;
	IPDATA hostIp_;

	struct NET_POOL
	{
		NET_JOIN_USER selfUser_;
		std::map<int, NET_JOIN_USER> remoteUsers_;
	} pool_;

	mutable std::mutex poolMutex_;
};