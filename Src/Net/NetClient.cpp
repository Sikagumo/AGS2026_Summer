#include "NetClient.h"

#include <DxLib.h>

#include "../Manager/System/NetManager.h"


void NetClient::UpdateConnecting(void)
{
    timerUser_ += SEND_INTERVAL_ACTION;
    if (timerUser_ >= 1.0f)
    {
        timerUser_ = 0.0f;
        netManager_.Send(NET_DATA_TYPE::USER);
    }
}

void NetClient::UpdateGotoGame(void)
{
    timerUser_ += SEND_INTERVAL_ACTION;
    if (timerUser_ >= 1.0f)
    {
        timerUser_ = 0.0f;
        netManager_.Send(NET_DATA_TYPE::USER);
    }
}