#pragma once
#include "NetBase.h"

class NetHost : public NetBase
{
public:
    using NetBase::NetBase;

    void UpdateConnecting(void) override;

    void UpdateGotoGame(void) override;

    void UpdateGamePlaying(void) override;
};