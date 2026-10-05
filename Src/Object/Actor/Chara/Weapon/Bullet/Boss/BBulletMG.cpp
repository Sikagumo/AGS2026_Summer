#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collision/CollisionController.h"
#include "BBulletMG.h"

BBulletMG::BBulletMG(void)
	: aliveTime_(0)
{
}

BBulletMG::~BBulletMG(void)
{
}

void BBulletMG::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_BULLET));
}

void BBulletMG::ReleasePost(void)
{
}

void BBulletMG::InitTransform(void)
{
	transform_.scl = BULLET_SCALE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y);
	transform_.Update();
}

void BBulletMG::InitCollider(void)
{
	ColliderSphere* colSphere = new ColliderSphere(
		ColliderBase::TAG::MG_BULLET, &transform_, COLLIDER_OFFSET, radius_);
	ownColliders_[static_cast<int>(ColliderBase::TAG::MG_BULLET)].push_back(colSphere);

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
	// プレイヤーまたはステージとの接触判定
	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER) ||
		CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE))
	{
		isAlive_ = false;
	}

	// 画面外（地面以下）に落下した場合の消滅判定
	if (transform_.pos.y <= MIN_ALTITUDE)
	{
		isAlive_ = false;
	}

	if (isAlive_)
	{
		aliveTime_++;

		// 発射元武器の姿勢を追従
		transform_.quaRot = weaponTrans_.quaRot;

		// 移動処理（方向 × スピード）
		VECTOR movePow = VScale(dir_, speed_);
		transform_.pos = VAdd(transform_.pos, movePow);

		// 寿命到達チェック
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

void BBulletMG::UpdateProcessPost(void)
{
}

void BBulletMG::DrawPre(void)
{
	if (isAlive_)
	{
		MV1DrawModel(transform_.modelId);
	}
}