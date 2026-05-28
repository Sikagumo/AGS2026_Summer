#include "../Collider/ColliderBase.h"
#include "../Manager/CollisionManager.h"


#include "DamageController.h"

DamageController::DamageController():
	bossAttack_(0),
	bossDamege_(0),
	playerAttack_(0),
	playerDamege_(0),
	weaponMGRDamege_(0),
	weaponMGLDamege_(0),
	weaponMPRDamege_(0),
	weaponMPLDamege_(0),
	weaponRGDamege_(0),
	weaponCannonLDamege_(0),
	weaponCannonRDamege_(0)
{
}

DamageController::~DamageController()
{
}

void DamageController::Update()
{
	
	bossDamege_ = 0;
	playerDamege_ = 0;
	weaponMGRDamege_ = 0;
	weaponMGLDamege_ = 0;
	weaponMPRDamege_ = 0;
	weaponMPLDamege_ = 0;
	weaponRGDamege_ = 0;
	weaponCannonLDamege_ = 0;
	weaponCannonRDamege_ = 0;

	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::BOSS, ColliderBase::TAG::PLAYER_BULLET))
	{
		bossDamege_ = playerAttack_;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponCannonLDamege_ = playerAttack_;
		bossDamege_ = playerAttack_ * BOSS_CAT_DAMEGE;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponCannonRDamege_ = playerAttack_;
		bossDamege_ = playerAttack_ * BOSS_CAT_DAMEGE;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMGLDamege_ = playerAttack_;
		bossDamege_ = playerAttack_ * BOSS_CAT_DAMEGE;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMGRDamege_ = playerAttack_;
		bossDamege_ = playerAttack_ * BOSS_CAT_DAMEGE;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMPLDamege_ = playerAttack_;
		bossDamege_ = playerAttack_ * BOSS_CAT_DAMEGE;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMPRDamege_ = playerAttack_;
		bossDamege_ = playerAttack_ * BOSS_CAT_DAMEGE;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_RG, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponRGDamege_ = playerAttack_;
		bossDamege_ = playerAttack_* BOSS_CAT_DAMEGE;
	}

	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::HIT_WAVE, ColliderBase::TAG::PLAYER))
	{
		playerDamege_ = playerHp_*(static_cast<float>(BOSS_WEPO_DAMEGE::PRESSWAVE)/10);
		
	}
	else
	{
		playerDamege_ = 0;
	}
}
