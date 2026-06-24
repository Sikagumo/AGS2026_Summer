#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collision/CollisionController.h"
#include "BBulletCannon.h"

BBulletCannon::BBulletCannon()
{
}

BBulletCannon::~BBulletCannon(void)
{
}

void BBulletCannon::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_BULLET));
}

void BBulletCannon::ReleasePost(void)
{
}

void BBulletCannon::InitTransform(void)
{
	transform_.scl = { 0.5f,0.5f,0.5f };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y);
	transform_.Update();
}

void BBulletCannon::InitCollider(void)
{
	ColliderSphere* colSphere = new ColliderSphere(
		ColliderBase::TAG::MG_BULLET, &transform_, { 0.0f,0.0f,0.0f }, radius_);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colSphere);


	CollisionController::GetInstance().RegisterActor(this);
}

void BBulletCannon::InitAnimation(void)
{
	aliveTime_ = 0;
	speed_ = INIT_SPEED;
	isAlive_ = true;
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MG_BULLET, true);
}

void BBulletCannon::InitPost(void)
{
}

void BBulletCannon::UpdateProcess(void)
{
	if (isAlive_)
	{
		aliveTime_++;
		// ’e‚ðˆÚ“®‚³‚¹‚é
		// ˆÚ“®—Ê‚ÌŒvŽZ(•ûŒü~ƒXƒs[ƒh)
		float targetAngle = atan2(dir_.x, dir_.z);
		transform_.quaRot = weaponTrans_.quaRot;



		VECTOR movePow = VScale(dir_, speed_);
		// ˆÚ“®ˆ—
		transform_.pos = VAdd(transform_.pos, movePow);

		if (aliveTime_ > MAX_ALIVE_TIME)
		{
			aliveTime_ = 0;
			isAlive_ = false;

		}
	}
	else
	{
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MG_BULLET, false);
		CollisionController::GetInstance().UnregisterActor(this);
	}
}

void BBulletCannon::UpdateProcessPost(void)
{
}

void BBulletCannon::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
}
