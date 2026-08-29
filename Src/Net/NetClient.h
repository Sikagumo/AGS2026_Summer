#pragma once
#include "NetBase.h"

class NetClient : public NetBase
{
public:
    using NetBase::NetBase;

    void UpdateConnecting(void) override;

    void UpdateGotoGame(void) override;

    void UpdateGamePlaying(void) override;

private:

    // ユーザー情報送信のしきい値時間
    static constexpr float SEND_INTERVAL_LIMIT = 0.1f; 
};