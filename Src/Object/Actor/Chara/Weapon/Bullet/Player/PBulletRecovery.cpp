#include "PBulletRecovery.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"


constexpr float RADIUS_BULLET = 10.0f;
constexpr float RADIUS_RECOVERY = 250.0f;
constexpr float SCALE_RECOVERY = 1.5f;
constexpr float TIME_ALIVE_RECOVERY = 15.0f;
constexpr float ACTIVE_TIME = 1.5f;

PBulletRecovery::PBulletRecovery(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
	, activeTime_(0.0f)
{
}

void PBulletRecovery::Load(void)
{
}

void PBulletRecovery::InitTransform(void)
{
	transform_.InitTransform(SCALE_RECOVERY
		, transform_.quaRot, Quaternion::Identity());
}

void PBulletRecovery::InitPost(void)
{
	PBulletBase::InitPost();


	ColliderSphere* recovery = new ColliderSphere(ColliderBase::TAG::PLAYER_RECOVERY, &transform_
		, UtilityMath::VECTOR_ZERO, radiusBlast_);
	ownColliders_[static_cast<int>(COLLISION_TYPE::SUPPORT)]
		.emplace_back(recovery);

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_RECOVERY, false);

	ownColliders_.at(static_cast<int>(COLLISION_TYPE::SUPPORT))
		.at(0)->SetRadius(radiusBlast_);
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

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_RECOVERY, false);
}

void PBulletRecovery::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		if (isActiveDestroy_)
		{
			// ìñÇΩÇËîªíËñ≥å¯âª
			CollisionController::GetInstance()
				.SetCollisionActive(this, ColliderBase::TAG::PLAYER_RECOVERY, false);
			bulletState_ = BULLET_STATE::INACTIVE;
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

void PBulletRecovery::PreActiveProcess(void)
{
}

void PBulletRecovery::ChangeBulletStateProc(void)
{
}

void PBulletRecovery::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activeTime_ = ACTIVE_TIME;

	// ìñÇΩÇËîªíËñ≥å¯âª
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);


	// âÒïúìñÇΩÇËîªíËóLå¯âª
	radiusBlast_ = RADIUS_RECOVERY;

	ownColliders_.at(static_cast<int>(COLLISION_TYPE::SUPPORT))
		.at(0)->SetRadius(radiusBlast_);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_RECOVERY, true);


	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_PLAYER_RECOVERY,
		transform_.pos, Quaternion::Identity().ToEuler(),
		{ 35.0f , 35.0f, 35.0f }, 1.0f, this);
}