#include "PlayerBase.h"

PlayerBase::PlayerBase(int _playerNo, PLAYER_TYPE _playerType)
	: CharaBase::CharaBase()
	, playerNo_(_playerNo)
	, playerType_(_playerType)
{
}