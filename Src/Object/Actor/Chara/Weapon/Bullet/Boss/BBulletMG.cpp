#include <DxLib.h>
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Manager/CollisionController.h"
#include "BBulletMG.h"

BBulletMG::BBulletMG(Transform& _transform)
{
}

BBulletMG::~BBulletMG(void)
{
}

void BBulletMG::Load(void)
{
	transform_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_BULLET));
}

void BBulletMG::ReleasePost(void)
{
}


void BBulletMG::InitTransform(void)
{
	transform_.scl = { 0.05f,0.05f,0.05f };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.Update();
}

void BBulletMG::InitCollider(void)
{
	ColliderSphere* colSphere = new ColliderSphere(
		ColliderBase::TAG::MG_BULLET, &transform_, {0.0f,0.0f,0.0f}, radiuse_);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colSphere);
	

	CollisionController::GetInstance().RegisterActor(this);

}

void BBulletMG::InitAnimation(void)
{
}

void BBulletMG::InitPost(void)
{
	aliveTime_ = 0;
	speed_ = INIT_SPEED;
	isAlive_ = true;
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MG_BULLET, true);
}

void BBulletMG::UpdateProcess(void)
{

	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER))
	{
		isAlive_ = false;
	}
	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE))
	{
		isAlive_ = false;
	}

	if (isAlive_)
	{
		aliveTime_++;
		// ’e‚ðˆÚ“®‚³‚¹‚é
		// ˆÚ“®—Ê‚ÌŒvŽZ(•ûŒü~ƒXƒs[ƒh)
		VECTOR movePow = VScale(dir_, speed_);
		// ˆÚ“®ˆ—
		transform_.pos = VAdd(transform_.pos, movePow);

		if (aliveTime_ > MAX_ALIVE_TIME)
		{
			isAlive_ = false;
		}
	}
	else
	{
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MG_BULLET, false);
	}
}

void BBulletMG::UpdateProcessPost(void)
{
}

void BBulletMG::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
	

}
