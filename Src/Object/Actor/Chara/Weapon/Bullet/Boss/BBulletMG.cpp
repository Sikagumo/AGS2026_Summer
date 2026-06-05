
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Manager/CollisionManager.h"
#include "BBulletMG.h"

BBulletMG::BBulletMG(Transform& _transform)
{
}

BBulletMG::~BBulletMG(void)
{
}

void BBulletMG::Load(void)
{
}

void BBulletMG::ReleasePost(void)
{
}


void BBulletMG::InitTransform(void)
{
	transform_.scl = { 1.0f,1.0f,1.0f };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.Update();
}

void BBulletMG::InitCollider(void)
{
	ColliderSphere* colSphere = new ColliderSphere(
		ColliderBase::TAG::MG_BULLET, &transform_, {0.0f,0.0f,0.0f}, radiuse_);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colSphere);
	

	CollisionManager::GetInstance().RegisterActor(this);

}

void BBulletMG::InitAnimation(void)
{
}

void BBulletMG::InitPost(void)
{
	speed_ = INIT_SPEED;
	isAlive_ = true;
}

void BBulletMG::UpdateProcess(void)
{

	if (CollisionManager::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER) || CollisionManager::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE))
	{
		isAlive_ = false;
	}

	if (isAlive_)
	{
		// 弾を移動させる
		// 移動量の計算(方向×スピード)
		VECTOR movePow = VScale(dir_, speed_);
		// 移動処理
		transform_.pos = VAdd(transform_.pos, movePow);

	}
	else
	{
		CollisionManager::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MG_BULLET, false);
	}
}

void BBulletMG::UpdateProcessPost(void)
{
}

void BBulletMG::DrawPre(void)
{
	if (isAlive_)
	{
		DrawSphere3D(transform_.pos, radiuse_, 16,0xffff00, 0xffff00, true);
	}
}
