#pragma once

#include "Netbase.h"

class NetClient : public NetBase
{
	using NetBase::NetBase;

	void UpdateConnecting(void) override;

	void UpdateGotoGame(void) override;

	void UpdateGamePlaying(void) override;

	void OnReceiveBossAction(const NET_BOSS_ACTION& bossAciton) override;
};

