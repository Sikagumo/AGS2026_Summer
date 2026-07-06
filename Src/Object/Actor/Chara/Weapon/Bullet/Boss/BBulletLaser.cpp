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
		ColliderBase::TAG::LASER, &transform_, { 0.0f,0.0f,0.0f }, { 0.0f,8000.0f,0.0f }, 200.0f);
	ownColliders_[static_cast<int>(ColliderBase::TAG::LASER)].push_back(colCapsule);


	CollisionController::GetInstance().RegisterActor(this);
}

void BBulletLaser::InitAnimation(void)
{
}

void BBulletLaser::InitPost(void)
{
}

void BBulletLaser::UpdateProcess(void)
{
	transform_.quaRot = weaponTrans_.quaRot;
}

void BBulletLaser::UpdateProcessPost(void)
{
}

void BBulletLaser::DrawPre(void)
{
	for (auto& [id, colliderVector] : ownColliders_)
	{
		for (auto* collider : colliderVector)
		{
			if (collider == nullptr)
			{
				continue;
			}

			collider->Draw();
		}
	}
}
