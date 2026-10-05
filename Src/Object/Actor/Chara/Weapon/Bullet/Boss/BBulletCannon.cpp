#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collision/CollisionController.h"
#include "BBulletCannon.h"

BBulletCannon::BBulletCannon(void)
	: aliveTime_(0)
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
	transform_.scl = BULLET_SCALE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y);
	transform_.Update();
}

void BBulletCannon::InitCollider(void)
{
	ColliderSphere* colSphere = new ColliderSphere(
		ColliderBase::TAG::CANNON_BULLET, &transform_, COLLIDER_OFFSET, radius_);
	ownColliders_[static_cast<int>(ColliderBase::TAG::MG_BULLET)].push_back(colSphere);

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

void BBulletCannon::CreateBullets(VECTOR _pos, VECTOR _dir, float _radius)
{
	transform_.pos = _pos;
	dir_ = _dir;
	radius_ = _radius;
}

void BBulletCannon::UpdateProcess(void)
{
	// プレイヤーまたはステージに衝突したら非生存状態へ
	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER) ||
		CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE))
	{
		isAlive_ = false;
	}

	if (isAlive_)
	{
		aliveTime_++;

		// 武器の回転を反映
		transform_.quaRot = weaponTrans_.quaRot;

		// 移動量の計算（方向 × スピード）
		VECTOR movePow = VScale(dir_, speed_);
		transform_.pos = VAdd(transform_.pos, movePow);

		// 寿命チェック
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
	if (isAlive_)
	{
		MV1DrawModel(transform_.modelId);
	}
}