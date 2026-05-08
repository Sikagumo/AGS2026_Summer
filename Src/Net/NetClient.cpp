#include "NetClient.h"
#include "../Manager/System/NetManager.h"

void NetClient::UpdateConnecting(void)
{
	timerUser_ += SEND_INTERVAL_ACTION;

	if (timerUser_ >= SEND_TIMERVAL_USER)
	{
		timerUser_ = 0.0f;

		netManager_.Send(NET_DATA_TYPE::USER);
	}
}

void NetClient::UpdateGotoGame(void)
{
	UpdateConnecting();
}

void NetClient::UpdateGamePlaying(void)
{
	netManager_.Send(NET_DATA_TYPE::USER);
}

void NetClient::OnReceiveBossAction(const NET_BOSS_ACTION& boss)
{

}
