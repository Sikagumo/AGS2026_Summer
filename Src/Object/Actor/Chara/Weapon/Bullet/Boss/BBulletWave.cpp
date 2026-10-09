#include <DxLib.h>
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collision/CollisionController.h"
#include "BBulletWave.h"

BBulletWave::BBulletWave(const Transform& _transform)
	: BBulletBase()
	, bossTransform_(_transform)
{
	radius_ = INIT_RADIUS;
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
	ColliderSphere* colHitSphere = new ColliderSphere(
		ColliderBase::TAG::HIT_WAVE, &transform_, COLLIDER_OFFSET, radius_);
	ownColliders_[static_cast<int>(ColliderBase::TAG::HIT_WAVE)].push_back(colHitSphere);

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
		// 衝撃波の判定を徐々に広げる
		radius_ += INCREASE_RADIUS;
		CollisionController::GetInstance().SetActorColliderRadius(this, ColliderBase::TAG::HIT_WAVE, radius_);
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::HIT_WAVE, true);
	}

	// 最大サイズに達したら初期化して判定をオフにする
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
}