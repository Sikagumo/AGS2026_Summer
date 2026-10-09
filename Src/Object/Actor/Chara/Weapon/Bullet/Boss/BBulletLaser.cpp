#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderCapsule.h"
#include "../../../../../Collision/CollisionController.h"

#include "BBulletLaser.h"

BBulletLaser::BBulletLaser(void)
{
}

BBulletLaser::~BBulletLaser(void)
{
}

void BBulletLaser::Load(void)
{
}

void BBulletLaser::ReleasePost(void)
{
}

void BBulletLaser::Shot(void)
{
}

void BBulletLaser::InitTransform(void)
{
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y);
	transform_.Update();
}

void BBulletLaser::InitCollider(void)
{
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::LASER, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::LASER)].push_back(colCapsule);

	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::LASER, false);
}

void BBulletLaser::InitAnimation(void)
{
}

void BBulletLaser::InitPost(void)
{
}

void BBulletLaser::UpdateProcess(void)
{
	// ïêäÌÇÃâÒì]Çí«è]
	transform_.quaRot = weaponTrans_.quaRot;

	// çUåÇÉtÉâÉOÇ…âûÇ∂ÇƒìñÇΩÇËîªíËÇÃóLå¯/ñ≥å¯Çêÿë÷
	if (isAttack_)
	{
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::LASER, true);
	}
	else
	{
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::LASER, false);
	}
}

void BBulletLaser::UpdateProcessPost(void)
{
}

void BBulletLaser::DrawPre(void)
{
}