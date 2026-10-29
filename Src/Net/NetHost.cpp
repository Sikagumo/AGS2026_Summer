#include "NetHost.h"
#include <DxLib.h>
#include "../Manager/System/NetManager.h"

namespace
{
	// ‰Šú‰»—p‚Ì’è”
	constexpr float VALUE_ZERO_F = 0.0f;
}

void NetHost::UpdateConnecting(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;
	if (timerUser_ >= SEND_INTERVAL_LIMIT)
	{
		timerUser_ = VALUE_ZERO_F;
		netManager_.Send(NET_DATA_TYPE::USERS);
	}
}

void NetHost::UpdateGotoGame(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;
	if (timerUser_ >= SEND_INTERVAL_LIMIT)
	{
		timerUser_ = VALUE_ZERO_F;
		netManager_.Send(NET_DATA_TYPE::USERS);
	}
}

void NetHost::UpdateGamePlaying(void)
{
	netManager_.Send(NET_DATA_TYPE::ACTION_HIST_ALL);
	netManager_.Send(NET_DATA_TYPE::BOSS_ACTION);
	netManager_.Send(NET_DATA_TYPE::ACTION_HIST_RELAY);
}