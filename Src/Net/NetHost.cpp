#include "NetHost.h"
#include "../Manager/System/NetManager.h"
#include "../Manager/System/TimeManager.h"

void NetHost::UpdateConnecting(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;

	if (timerUser_ >= SEND_TIMERVAL_USER)
	{
		timerUser_ = 0.0f;

		netManager_.Send(NET_DATA_TYPE::USERS);
	}
}

void NetHost::UpdateGotoGame(void)
{
	UpdateConnecting();
}

void NetHost::UpdateGamePlaying(void)
{
	netManager_.Send(NET_DATA_TYPE::BOSS_ACTOION);

	netManager_.Send(NET_DATA_TYPE::ACTION_HIST_ALL);
}

void NetHost::OnReceiveAction(const NET_ACTION_HIS& actionHis)
{

}
