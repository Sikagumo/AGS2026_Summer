#include "PBulletBomb.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"

constexpr float RADIUS_BULLET = 9.0f;
constexpr float RADIUS_BLAST = 100.0f;
constexpr float SCALE_BOMB = 1.5f;
constexpr float SHOT_SPEED_BOMB_XZ = 10.0f;
constexpr float SHOT_SPEED_BOMB_Y  = 8.5f;
constexpr float TIME_ALIVE_BOMB = 15.0f;
constexpr int POWER = 10;


PBulletBomb::PBulletBomb(void)
	: PBulletBase::PBulletBase()
{
}

void PBulletBomb::Load(void)
{
}

void PBulletBomb::InitTransform(void)
{
	
}

void PBulletBomb::InitPost(void)
{
	PBulletBase::InitPost();

}

void PBulletBomb::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		if (isActiveDestroy_) 
		{
			// ìñÇΩÇËîªíËñ≥å¯âª
			CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);
			bulletState_ = BULLET_STATE::INACTIVE;
			activePower_ = 0;
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
	power_ = POWER;
	transform_.InitTransform(SCALE_BOMB, transform_.quaRot, Quaternion::Identity());
}

void PBulletBomb::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activePower_ = power_;

	// ìñÇΩÇËîªíËñ≥å¯âª
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);

	// îöî≠óLå¯âª
	radiusBlast_ = RADIUS_BLAST;

	ownColliders_.at(static_cast<int>(COLLISION_TYPE::BLAST)).at(0)->SetRadius(radiusBlast_);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, true);
}