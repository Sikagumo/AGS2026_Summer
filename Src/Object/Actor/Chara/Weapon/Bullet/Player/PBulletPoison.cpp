#include "PBulletPoison.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../../Manager/System/TimeManager.h"

constexpr float RADIUS_BULLET = 5.0f;
constexpr float RADIUS_POISON = 87.5f;
constexpr float SCALE_POISON = 1.0f;
constexpr float TIME_ALIVE_POISON = 7.5f;
constexpr int POWER_POISON = 1;
constexpr float ACTIVE_TIME = 1.25f;


PBulletPoison::PBulletPoison(void)
	: PBulletBase::PBulletBase()
	, activeTime_(0.0f)
{
}

void PBulletPoison::Load(void)
{
}

void PBulletPoison::InitTransform(void)
{
	
}

void PBulletPoison::InitPost(void)
{
	PBulletBase::InitPost();

	activeTime_ = 0.0f;
}

void PBulletPoison::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		if (isActiveDestroy_) 
		{
			// ìñÇΩÇËîªíËñ≥å¯âª
			CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
			bulletState_ = BULLET_STATE::INACTIVE;
			activePowerBullet_ = 0;
			return;
		}

		if (activeTime_ > 0.0f)
		{
			activeTime_ -= TimeManager::GetInstance().GetDeltaTime();
		}
		else
		{
			// íeÇè¡ñ≈Ç≥ÇπÇÈ
			isActiveDestroy_ = true;
		}
	}
}

void PBulletPoison::PreActiveProcess(void)
{
}

void PBulletPoison::ChangeBulletStateProc(void)
{
}

void PBulletPoison::SetParam(void)
{
	shotSpeedXZ_ = SHOT_SPEED_POISON_XZ;
	shotSpeedY_ = SHOT_SPEED_POISON_Y;

	aliveTime_ = TIME_ALIVE_POISON;

	radiusBullet_ = RADIUS_BULLET;
	radiusBlast_ = 0.0f;
	power_ = POWER_POISON;
	transform_.InitTransform(SCALE_POISON, transform_.quaRot, Quaternion::Identity());
}

void PBulletPoison::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activePowerBlast_ = power_;

	// ìñÇΩÇËîªíËñ≥å¯âª
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);

	// îöî≠óLå¯âª
	radiusBlast_ = RADIUS_POISON;

	ownColliders_.at(static_cast<int>(COLLISION_TYPE::BLAST))->SetRadius(radiusBlast_);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, true);
}