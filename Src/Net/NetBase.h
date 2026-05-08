#pragma once

#include <vector>

#include "NetStructures.h"

class NetManager;

class NetBase
{
public:
	NetBase(NetManager& Manager);

	virtual ~NetBase(void) = default;

	virtual void UpdateConnecting(void) = 0;

	virtual void UpdateGotoGame(void) = 0;

	virtual void UpdateGamePlaying(void) = 0;

	virtual void OnReceiveUser(void) {};

	virtual void OnReceiveUsers(const std::vector<NET_JOINT_USER>& users) {};

	virtual void OnReceiveAction(const NET_ACTION_HIS& actionHis) {};

	virtual void OnReceiveBossAction(const NET_BOSS_ACTION& bossAction) {};

protected:

	NetManager& netManager_;

	static constexpr float SEND_TIMERVAL_USER = 1.0f;

	static constexpr float SEND_INTERVAL_ACTION = 0.1f / 60.0f;

	float timerUser_;

	float timerAction_;
};

