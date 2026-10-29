#include "NetBase.h"

namespace
{
	// ‰Šú‰»—p‚Ì’è”
	constexpr float VALUE_ZERO_F = 0.0f;
}

NetBase::NetBase(NetManager& _netManager)
	: netManager_(_netManager)
	, timerUser_(VALUE_ZERO_F)
	, timerAction_(VALUE_ZERO_F)
{
}