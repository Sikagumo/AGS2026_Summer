#include "NetBase.h"

NetBase::NetBase(NetManager& _netManager)
	: netManager_(_netManager)
	, timerUser_(0.0f)
	, timerAction_(0.0f)
{
}