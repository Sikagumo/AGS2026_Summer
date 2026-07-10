#include "../Collider/ColliderBase.h"
#include "../Collision/CollisionController.h"


#include "DamageController.h"

DamageController::DamageController()
	: bossDamage_(0)
	, weaponMGLDamage_(0), weaponMGRDamage_(0)
	, weaponMPLDamage_(0), weaponMPRDamage_(0)
	, weaponRGDamage_(0)
	, weaponCannonLDamage_(0) , weaponCannonRDamage_(0)
	, playerHpMax_{}
	, playerAttackBullet_(0), playerAttackBlast_(0)
	, damageData_(0)
	, isInvincible_(false)
{

	cannon_.type = BOSS_WEPO_TYPE::CANNON;
	cannon_.attack = 0.2f;
	mg_.type = BOSS_WEPO_TYPE::MG;
	mg_.attack = 0.01f;
	mp_.type = BOSS_WEPO_TYPE::MP;
	mp_.attack = 0.7f;
	rg_.type = BOSS_WEPO_TYPE::RG;
	rg_.attack = 0.2;
	pressWave_.type = BOSS_WEPO_TYPE::PRESSWAVE;
	pressWave_.attack = 0.1f;
	rode_.type = BOSS_WEPO_TYPE::RODE;
	rode_.attack = 0.3f;
}

DamageController::~DamageController()
{
}

void DamageController::Update()
{
	bossDamage_ = 0;
	playerDamage_ = 0;
	weaponMGLDamage_ = 0;
	weaponMGRDamage_ = 0;
	weaponMPLDamage_ = 0;
	weaponMPRDamage_ = 0;
	weaponRGDamage_ = 0;
	weaponCannonLDamage_ = 0;
	weaponCannonRDamage_ = 0;
	isInvincible_ = true;

	// ボス関連とプレイヤー弾＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	// 左キャノンとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponCannonLDamage_ = playerAttackBullet_;
		bossDamage_ = static_cast<int>(playerAttackBullet_ * BOSS_CUT_DAMAGE);
	}
	// 右キャノンとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponCannonRDamage_ = playerAttackBullet_;
		bossDamage_ = static_cast<int>(playerAttackBullet_ * BOSS_CUT_DAMAGE);
	}
	// 左ガトリングとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMGLDamage_ = playerAttackBullet_;
		bossDamage_ = static_cast<int>(playerAttackBullet_ * BOSS_CUT_DAMAGE);
	}
	// 右ガトリングとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMGRDamage_ = playerAttackBullet_;
		bossDamage_ = static_cast<int>(playerAttackBullet_ * BOSS_CUT_DAMAGE);
	}
	// 左ミサイルポッドとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMPLDamage_ = playerAttackBullet_;
		bossDamage_ = static_cast<int>(playerAttackBullet_ * BOSS_CUT_DAMAGE);
	}
	// 右ミサイルポッドとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_R, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponMPRDamage_ = playerAttackBullet_;
		bossDamage_ = static_cast<int>(playerAttackBullet_ * BOSS_CUT_DAMAGE);
	}
	// レールガンとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_RG, ColliderBase::TAG::PLAYER_BULLET))
	{
		weaponRGDamage_ = playerAttackBullet_;
		bossDamage_ = static_cast<int>(playerAttackBullet_ * BOSS_CUT_DAMAGE);
	}
	// ボスとプレイヤー弾
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::BOSS, ColliderBase::TAG::PLAYER_BULLET))
	{
		bossDamage_ = playerAttackBullet_;
	}
	
	if (playerAttackBlast_ > 0)
	{
		// 左キャノンとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::PLAYER_BLAST))
		{
			weaponCannonLDamage_ = playerAttackBlast_;
			bossDamage_ = static_cast<int>(playerAttackBlast_ * BOSS_CUT_DAMAGE);
		}
		// 右キャノンとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_CANNON_R, ColliderBase::TAG::PLAYER_BLAST))
		{
			weaponCannonRDamage_ = playerAttackBlast_;
			bossDamage_ = static_cast<int>(playerAttackBlast_ * BOSS_CUT_DAMAGE);
		}
		// 左ガトリングとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::PLAYER_BLAST))
		{
			weaponMGLDamage_ = playerAttackBlast_;
			bossDamage_ = static_cast<int>(playerAttackBlast_ * BOSS_CUT_DAMAGE);
		}
		// 右ガトリングとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MG_R, ColliderBase::TAG::PLAYER_BLAST))
		{
			weaponMGRDamage_ = playerAttackBlast_;
			bossDamage_ = static_cast<int>(playerAttackBlast_ * BOSS_CUT_DAMAGE);
		}
		// 左ミサイルポッドとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::PLAYER_BLAST))
		{
			weaponMPLDamage_ = playerAttackBlast_;
			bossDamage_ = static_cast<int>(playerAttackBlast_ * BOSS_CUT_DAMAGE);
		}
		// 右ミサイルポッドとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_MP_R, ColliderBase::TAG::PLAYER_BLAST))
		{
			weaponMPRDamage_ = playerAttackBlast_;
			bossDamage_ = static_cast<int>(playerAttackBlast_ * BOSS_CUT_DAMAGE);
		}
		// レールガンとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::WEAPON_RG, ColliderBase::TAG::PLAYER_BLAST))
		{
			weaponRGDamage_ = playerAttackBlast_;
			bossDamage_ = static_cast<int>(playerAttackBlast_ * BOSS_CUT_DAMAGE);
		}
		// ボスとプレイヤー爆発
		if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::BOSS, ColliderBase::TAG::PLAYER_BLAST))
		{
			bossDamage_ = playerAttackBlast_;
		}
	}

	// プレイヤーとボスの攻撃＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::HIT_WAVE, ColliderBase::TAG::PLAYER))
	{
		// HP割合ダメージ
		const float RATE_DAMAGE = (playerHpMax_ * pressWave_.attack);
		playerDamage_ = static_cast<int>(RATE_DAMAGE);
		damageData_ = playerDamage_;
	}
	else if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::MG_BULLET, ColliderBase::TAG::PLAYER))
	{
		// HP割合ダメージ
		const float RATE_DAMAGE = (playerHpMax_ * mg_.attack);
		playerDamage_ = static_cast<int>(RATE_DAMAGE);
		isInvincible_ = false;
		damageData_ = playerDamage_;
	}
	else if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::ROAD_ATTACK, ColliderBase::TAG::PLAYER))
	{
		// HP割合ダメージ
		const float RATE_DAMAGE = (playerHpMax_ * rode_.attack);
		playerDamage_ = static_cast<int>(RATE_DAMAGE);
		damageData_ = playerDamage_;
	}
	else if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::MISSILE_ATTACK, ColliderBase::TAG::PLAYER))
	{
		// HP割合ダメージ
		const float RATE_DAMAGE = (playerHpMax_ * mp_.attack);
		playerDamage_ = static_cast<int>(RATE_DAMAGE);
		damageData_ = playerDamage_;
	}
	else if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::LASER, ColliderBase::TAG::PLAYER))
	{
		// HP割合ダメージ
		const float RATE_DAMAGE = (playerHpMax_ * rg_.attack);
		playerDamage_ = static_cast<int>(RATE_DAMAGE);
		isInvincible_ = false;
		damageData_ = playerDamage_;
	}
	else if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::CANNON_BULLET, ColliderBase::TAG::PLAYER))
	{
		// HP割合ダメージ
		const float RATE_DAMAGE = (playerHpMax_ * cannon_.attack);
		playerDamage_ = static_cast<int>(RATE_DAMAGE);
		damageData_ = playerDamage_;
	}
}

void DamageController::DebugDraw(void)
{
	DrawFormatString(10, 150, 0xffffff, "PlayerDamage_:%d", damageData_);
}

void DamageController::SetPlayerAttack(int _bulletAttack, int _blastAttack)
{
	playerAttackBullet_ = _bulletAttack;
	playerAttackBlast_ = _blastAttack;
}
