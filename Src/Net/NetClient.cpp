#include "NetClient.h"

#include <DxLib.h>

#include "../Manager/System/NetManager.h"


void NetClient::UpdateConnecting(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;
	if (timerUser_ >= SEND_INTERVAL_LIMIT)
	{
		timerUser_ = 0.0f;
		netManager_.Send(NET_DATA_TYPE::USER);
	}
}

void NetClient::UpdateGotoGame(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;
	if (timerUser_ >= SEND_INTERVAL_LIMIT)
	{
		timerUser_ = 0.0f;
		netManager_.Send(NET_DATA_TYPE::USER);
	}
}

void NetClient::UpdateGamePlaying(void)
{
	netManager_.Send(NET_DATA_TYPE::ACTION_HIST_ALL);
}