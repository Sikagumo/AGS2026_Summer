#include "PBulletRecovery.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../../Utility/UtilityMath.h"

constexpr float RADIUS_BULLET = 10.0f;
constexpr float RADIUS_RECOVERY = 250.0f;
constexpr float SCALE_RECOVERY = 1.5f;
constexpr float TIME_ALIVE_RECOVERY = 15.0f;


PBulletRecovery::PBulletRecovery(void)
	: PBulletBase::PBulletBase()
{
}

void PBulletRecovery::Load(void)
{
}

void PBulletRecovery::InitTransform(void)
{
	
}

void PBulletRecovery::InitPost(void)
{
	PBulletBase::InitPost();

	ColliderSphere* recovery = new ColliderSphere(ColliderBase::TAG::PLAYER_RECOVERY, &transform_
								, UtilityMath::VECTOR_ZERO, radiusBlast_);
	ownColliders_.emplace(static_cast<int>(COLLISION_TYPE::RECOVERY)
		, recovery);

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, true);
}

void PBulletRecovery::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		// íeÇè¡ñ≈Ç≥ÇπÇÈ
		isActiveDestroy_ = true;
		if (isActiveDestroy_)
		{
			// ìñÇΩÇËîªíËñ≥å¯âª
			CollisionController::GetInstance()
				.SetCollisionActive(this, ColliderBase::TAG::PLAYER_RECOVERY, false);
			bulletState_ = BULLET_STATE::INACTIVE;
			return;
		}

		
	}
}

void PBulletRecovery::PreActiveProcess(void)
{
}

void PBulletRecovery::ChangeBulletStateProc(void)
{
}

void PBulletRecovery::SetParam(void)
{
	shotSpeedXZ_ = SHOT_SPEED_RECOVERY_XZ;
	shotSpeedY_ = SHOT_SPEED_RECOVERY_Y;

	aliveTime_ = TIME_ALIVE_RECOVERY;

	radiusBullet_ = RADIUS_BULLET;
	radiusBlast_ = 0.0f;

	transform_.InitTransform(SCALE_RECOVERY
		, transform_.quaRot, Quaternion::Identity());
}
void PBulletRecovery::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;

	// ìñÇΩÇËîªíËñ≥å¯âª
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);

	// îöî≠óLå¯âª
	radiusBlast_ = RADIUS_RECOVERY;

	ownColliders_.at(static_cast<int>(COLLISION_TYPE::RECOVERY))->SetRadius(radiusBlast_);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_RECOVERY, true);
}