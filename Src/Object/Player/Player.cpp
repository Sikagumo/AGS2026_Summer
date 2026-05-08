#include "Player.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Common/AnimationController.h"

Player::Player(int _playerNo)
	: CharaBase::CharaBase()
	, playerNo_(_playerNo)
{
}


void Player::InitLoadPost(void)
{
	//transform_.modelId = resMng_.GetHandleId(ResourceManager::SRC::
}
void Player::InitAnimationPost(void)
{
}
void Player::InitTransform(void)
{
	transform_.InitTransform();
}
void Player::InitCollider(void)
{

}
void Player::InitPost(void)
{
}


void Player::UpdateProcess(void)
{
}

void Player::Release(void)
{
}