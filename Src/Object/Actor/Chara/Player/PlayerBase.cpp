#include "PlayerBase.h"

PlayerBase::PlayerBase(int _playerNo, BULLET_TYPE _bulletType, PLAYER_TYPE _playerType)
	: CharaBase::CharaBase()
	, playerNo_(_playerNo)
	, bulletType_(_bulletType)
	, playerType_(_playerType)
{
}