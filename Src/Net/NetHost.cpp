#include "NetHost.h"

#include <DxLib.h>

#include "../Manager/System/NetManager.h"

void NetHost::UpdateConnecting(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;
	if (timerUser_ >= SEND_INTERVAL_LIMIT)
	{
		timerUser_ = 0.0f;
		netManager_.Send(NET_DATA_TYPE::USERS);
	}
}

void NetHost::UpdateGotoGame(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;
	if (timerUser_ >= SEND_INTERVAL_LIMIT)
	{
		timerUser_ = 0.0f;
		netManager_.Send(NET_DATA_TYPE::USERS);
	}
}

void NetHost::UpdateGamePlaying(void)
{
	netManager_.Send(NET_DATA_TYPE::ACTION_HIST_ALL);
	netManager_.Send(NET_DATA_TYPE::BOSS_ACTION);
	netManager_.Send(NET_DATA_TYPE::ACTION_HIST_RELAY);
}