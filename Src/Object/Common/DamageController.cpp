#include "../Collider/ColliderBase.h"
#include "../Manager/CollisionManager.h"


#include "DamageController.h"

DamageController::DamageController():
	bossAttack_(0),
	bossDamage_(0),
	playerAttack_(0),
	playerDamage_(0),
	weaponMGRDamage_(0),
	weaponMGLDamage_(0),
	weaponMPRDamage_(0),
	weaponMPLDamage_(0),
	weaponRGDamage_(0),
	weaponCannonLDamage_(0),
	weaponCannonRDamage_(0),
	playerHp_(0)

{
}

DamageController::~DamageController()
{
}

void DamageController::Update()
{
	
	bossDamage_ = 0;
	playerDamage_ = 0;
	weaponMGRDamage_ = 0;
	weaponMGLDamage_ = 0;
	weaponMPRDamage_ = 0;
	weaponMPLDamage_ = 0;
	weaponRGDamage_ = 0;
	weaponCannonLDamage_ = 0;
	weaponCannonRDamage_ = 0;

	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::BOSS, ColliderBase::TAG::PLAYER_BULLET))
	{
		bossDamage_ = playerAttack_;
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponCannonLDamage_ = playerAttack_;
		bossDamage_ = static_cast<int>(playerAttack_ * BOSS_CUT_DAMAGE);
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponCannonRDamage_ = playerAttack_;
		bossDamage_ = static_cast<int>(playerAttack_ * BOSS_CUT_DAMAGE);
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMGLDamage_ = playerAttack_;
		bossDamage_ = static_cast<int>(playerAttack_ * BOSS_CUT_DAMAGE);
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMGRDamage_ = playerAttack_;
		bossDamage_ = static_cast<int>(playerAttack_ * BOSS_CUT_DAMAGE);
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMPLDamage_ = playerAttack_;
		bossDamage_ = static_cast<int>(playerAttack_ * BOSS_CUT_DAMAGE);
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMPRDamage_ = playerAttack_;
		bossDamage_ = static_cast<int>(playerAttack_ * BOSS_CUT_DAMAGE);
	}
	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_RG, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponRGDamage_ = playerAttack_;
		bossDamage_ = static_cast<int>(playerAttack_* BOSS_CUT_DAMAGE);
	}

	if (CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::HIT_WAVE, ColliderBase::TAG::PLAYER))
	{
		// 割合ダメージ値
		const float RATE_DAMAGE = (playerHp_ * (static_cast<float>(BOSS_WEPO_DAMAGE::PRESSWAVE) / 10));
		playerDamage_ = static_cast<int>(RATE_DAMAGE);
		
	}
	else
	{
		playerDamage_ = 0;
	}
}
