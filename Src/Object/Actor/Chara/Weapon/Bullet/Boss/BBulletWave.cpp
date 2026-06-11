#include <DxLib.h>
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collision/CollisionController.h"
#include "BBulletWave.h"

BBulletWave::BBulletWave(Transform& _transform):
	bossTransform_(_transform),
	radius_(INIT_RADIUS),
	

	BBulletBase()
{
}

BBulletWave::~BBulletWave(void)
{
}

void BBulletWave::Load(void)
{
}

void BBulletWave::ReleasePost(void)
{
}

void BBulletWave::SetPos(VECTOR _pos)
{
	transform_.pos = _pos;
}

void BBulletWave::InitTransform(void)
{
	transform_.scl = bossTransform_.scl;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.pos = bossTransform_.pos;
	transform_.Update();
}

void BBulletWave::InitCollider(void)
{
	transform_.pos = bossTransform_.pos;
	ColliderSphere* colHitSphere = new ColliderSphere(ColliderBase::TAG::HIT_WAVE, &transform_, { 0.0f,0.0f,0.0f }, radius_);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colHitSphere);

	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::HIT_WAVE, false);
}

void BBulletWave::InitAnimation(void)
{
}

void BBulletWave::InitPost(void)
{
}

void BBulletWave::UpdateProcess(void)
{
	
	if (isAttack_)
	{
		radius_ += INCREASE_RADIUS;
		CollisionController::GetInstance().SetActorColliderRadius(this, ColliderBase::TAG::HIT_WAVE, radius_);
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::HIT_WAVE, true);
	}
	if (radius_ >= MAX_RADIUS)
	{
		radius_ = INIT_RADIUS;

		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::HIT_WAVE, false);
		CollisionController::GetInstance().SetActorColliderRadius(this, ColliderBase::TAG::HIT_WAVE, radius_);
		isAttack_ = false;		
	}
}

void BBulletWave::UpdateProcessPost(void)
{
}

void BBulletWave::DrawPre(void)
{
	for (auto& col : ownColliders_)
	{
		col.second->Draw();
	}
#ifdef _DEBUG
	DrawFormatString(10, 200, 0xffffff, "radius:%f", radius_);
	DrawFormatString(10, 230, 0xffffff, "pos:%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
#endif
}
