#include "PBulletBomb.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"

constexpr float RADIUS_BULLET = 9.0f;
constexpr float RADIUS_BLAST = 65.0f;
constexpr float SCALE_BOMB = 1.0f;
constexpr float TIME_ALIVE_BOMB = 10.0f;
constexpr int POWER_BOMB = 50;


PBulletBomb::PBulletBomb(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
{
}

void PBulletBomb::Load(void)
{
}

void PBulletBomb::InitTransform(void)
{
	transform_.InitTransform(SCALE_BOMB
		, transform_.quaRot, Quaternion::Identity());
}


void PBulletBomb::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		if (isActiveDestroy_) 
		{
			// ìñÇΩÇËîªíËñ≥å¯âª
			CollisionController::GetInstance()
				.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
			bulletState_ = BULLET_STATE::INACTIVE;
			activePowerBullet_ = 0;
			activePowerBlast_ = 0;
			return;
		}

		// íeÇè¡ñ≈Ç≥ÇπÇÈ
		isActiveDestroy_ = true;
	}
}

void PBulletBomb::PreActiveProcess(void)
{
}

void PBulletBomb::ChangeBulletStateProc(void)
{
}

void PBulletBomb::SetParam(void)
{
	shotSpeedXZ_ = SHOT_SPEED_BOMB_XZ;
	shotSpeedY_ = SHOT_SPEED_BOMB_Y;

	aliveTime_ = TIME_ALIVE_BOMB;

	radiusBullet_ = RADIUS_BULLET;
	radiusBlast_ = 0.0f;

	power_ = POWER_BOMB;
}

void PBulletBomb::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activePowerBlast_ = POWER_BOMB;

	// ìñÇΩÇËîªíËñ≥å¯âª
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);

	// îöî≠óLå¯âª
	radiusBlast_ = RADIUS_BLAST;

	ownColliders_[static_cast<int>(COLLISION_TYPE::BLAST)]
		.at(0)->SetRadius(radiusBlast_);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, true);


	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_PLAYER_BLAST,
		transform_.pos, Quaternion::Identity().ToEuler(),
		{ 35.0f , 35.0f, 35.0f }, 1.0f, this);
}