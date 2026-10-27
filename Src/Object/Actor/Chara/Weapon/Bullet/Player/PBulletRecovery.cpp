#include "PBulletRecovery.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"

PBulletRecovery::PBulletRecovery(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
	, activeTimeRecovery_(0.0f)
{
}

void PBulletRecovery::InitTransform(void)
{
	transform_.InitTransform(SCALE_RECOVERY
		, transform_.quaRot, Quaternion::Identity());
}

void PBulletRecovery::InitCollider(void)
{
	PBulletBase::InitCollider();

	// 回復範囲の判定は一度だけ生成する
	auto& support = ownColliders_[static_cast<int>(COLLISION_TYPE::SUPPORT)];
	if (!support.empty()) { return; }

	support.emplace_back(new ColliderSphere(ColliderBase::TAG::PLAYER_RECOVERY, &transform_
		, UtilityMath::VECTOR_ZERO, 0.0f));

	SetColliderActive(ColliderBase::TAG::PLAYER_RECOVERY, false);
}

void PBulletRecovery::SetParam(void)
{
	shotSpeedXZ_ = SHOT_SPEED_RECOVERY_XZ;
	shotSpeedY_ = SHOT_SPEED_RECOVERY_Y;

	aliveTime_ = TIME_ALIVE_RECOVERY;

	radiusBullet_ = RADIUS_BULLET;
	radiusBlast_ = 0.0f;

	// 回復範囲の無効化
	SetColliderRadius(COLLISION_TYPE::SUPPORT, radiusBlast_);
	SetColliderActive(ColliderBase::TAG::PLAYER_RECOVERY, false);
}

void PBulletRecovery::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		if (isActiveDestroy_)
		{
			// 当たり判定無効化
			CollisionController::GetInstance()
				.SetCollisionActive(this, ColliderBase::TAG::PLAYER_RECOVERY, false);

			bulletState_ = BULLET_STATE::INACTIVE;
			return;
		}

		if (activeTimeRecovery_ > 0.0f)
		{
			activeTimeRecovery_ -= TimeManager::GetInstance().GetDeltaTime();
		}
		else
		{
			// 回復の継続時間が終了したら消滅させる
			isActiveDestroy_ = true;
		}
	}
}

void PBulletRecovery::BlastAction(void)
{
	// 着弾状態へ移行(弾の当たり判定は無効化)
	BeginBlast();
	activeTimeRecovery_ = ACTIVE_TIME;

	// 回復当たり判定有効化
	radiusBlast_ = RADIUS_RECOVERY;
	SetColliderRadius(COLLISION_TYPE::SUPPORT, radiusBlast_);
	SetColliderActive(ColliderBase::TAG::PLAYER_RECOVERY, true);

	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_PLAYER_RECOVERY
		, transform_.pos, Quaternion::Identity().ToEuler()
		, { RECOVERY_EFFECT_SCALE, RECOVERY_EFFECT_SCALE, RECOVERY_EFFECT_SCALE }
		, RECOVERY_EFFECT_SPEED, this);
}