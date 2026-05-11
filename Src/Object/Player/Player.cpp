#include "Player.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Common/AnimationController.h"

Player::Player(int _playerNo)
	: PlayerBase::PlayerBase(_playerNo)
	
{
}


void Player::InitLoadPost(void)
{
	transform_.modelId = resMng_.LoadHandleId(ResourceManager::SRC::MODEL_PLAYER);
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