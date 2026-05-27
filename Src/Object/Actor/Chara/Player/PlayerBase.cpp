#include "PlayerBase.h"

constexpr int HP = 50;
constexpr float INVINSIVE_TIME = 1.0f;

PlayerBase::PlayerBase(int _playerNo, BULLET_TYPE _bulletType, PLAYER_TYPE _playerType)
	: CharaBase::CharaBase()
	, playerNo_(_playerNo)
	, bulletType_(_bulletType)
	, playerType_(_playerType)
	, hp_(HP), MAX_HP(HP)
	, curInvTime_(0.0f)
{
}

void PlayerBase::SetDamage(int _damage)
{
	if (curInvTime_ > 0.0f || hp_ <= 0 || _damage <= 0) { return; }

	curInvTime_ = INVINSIVE_TIME;

	hp_ -= _damage;
}
