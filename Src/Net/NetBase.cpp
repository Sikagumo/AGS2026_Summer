#include "NetBase.h"

NetBase::NetBase(NetManager& netManager)
	: netManager_(netManager)
	, timerUser_(0.0f)
	, timerAction_(0.0f)
{
}
