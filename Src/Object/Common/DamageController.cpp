#include "../Collider/ColliderBase.h"
#include "../Manager/CollisionManager.h"

#include "DamageController.h"

DamageController::DamageController():
	bossAttack_(0),
	bossDamege_(0),
	weaponDamege_(0),
	playerAttack_(0),
	playerDamege_(0)
{
}

DamageController::~DamageController()
{
}

void DamageController::Update()
{
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::BOSS, ColliderBase::TAG::PLAYER_BULLET))
	{
		bossDamege_ = playerAttack_;
	}
	else 
	{
		bossDamege_ = 0;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponDamege_ = playerAttack_;
	}
	else
	{
		weaponDamege_ = 0;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponDamege_ = playerAttack_;
	}
	else
	{
		weaponDamege_ = 0;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponDamege_ = playerAttack_;
	}
	else
	{
		weaponDamege_ = 0;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponDamege_ = playerAttack_;
	}
	else
	{
		weaponDamege_ = 0;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponDamege_ = playerAttack_;
	}
	else
	{
		weaponDamege_ = 0;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponDamege_ = playerAttack_;
	}
	else
	{
		weaponDamege_ = 0;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_RG, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponDamege_ = playerAttack_;
	}
	else
	{
		weaponDamege_ = 0;
	}

}
