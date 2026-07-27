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
	void SetRoomWordId(int _id);

	// 自分の最新アクションを履歴に追加する
	void AddSelfAction(const NET_ACTION& _action);

	// 自分の過去数フレーム分のアクション履歴を取得する
	NET_ACTION_HIS GetSelfActionHis(void) const;

	// 他人のアクション履歴を取得する関数
	std::map<int, NET_ACTION_HIS> GetRemoteActionHis(void) const;

	bool GetHasReceivedGoGame(void) const { return hasReceivedGoGame_; }

	void ResetGoGame(void);

	void SetBossAction(const NET_BOSS_ACTION& action);

	const NET_BOSS_ACTION GetBossAction(void) const { return pool_.bossAction; }

	void SetGameTime(float _time);

	float GetGameTime(void) const { return gameTime_; }

	/// @brief 通信相手とのタイムアウトを検知する
	bool GetIsConnectionLost(void) const;

private:

	// 接続ロストと判定するまでの秒数
	static constexpr float CONNECTION_TIMEOUT = 5.0f;

	static NetManager* instance_;

	NetBase* netBase_;
	NetSend* netSend_;

	bool isRunning_;
	NET_MODE mode_;
	NET_ACTION_HIS selfActionHis_;
	int recvSocketId_;
	int sendSocketId_;
	int roomWordId_;
	bool hasReceivedGoGame_;
	float gameTime_;
	IPDATA hostIp_;
	float hostTimeoutTimer_;
	std::map<int, float> clientTimeoutTimers_;

	std::map<int, NET_ACTION_HIS> remoteActionHis_;

	struct NET_POOL
	{
		NET_JOIN_USER selfUser_;
		NET_BOSS_ACTION bossAction;
		std::map<int, NET_JOIN_USER> remoteUsers_;
	};

	NET_POOL pool_;

	mutable std::mutex poolMutex_;


	NetManager(void);
	~NetManager(void);

	/// @brief 受信したUDPパケットの解析とデータ種別ごとの振り分け処理
	void UdpReceiveData(void);
};