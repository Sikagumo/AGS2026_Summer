#include "PBulletBase.h"
#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"
#include "../../../../../../Application.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collision/CollisionController.h"

namespace
{
	// 弾の球の分割数
	constexpr int SPHERE_DIV = 16;

	// 弾が着弾(爆発)する対象タグ。毎フレーム生成しないよう静的に保持
	constexpr std::array<ColliderBase::TAG, 10> HIT_TARGET_TAGS =
	{
		ColliderBase::TAG::BOSS, ColliderBase::TAG::ENEMY, ColliderBase::TAG::ENEMYROBO,
		ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::WEAPON_CANNON_R,
		ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::WEAPON_MG_R,
		ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::WEAPON_MP_R,
		ColliderBase::TAG::WEAPON_RG
	};
}

PBulletBase::PBulletBase(int _shotType, bool _isGravity)
	: ActorBase::ActorBase()
	, shotType_(_shotType)
	, bulletState_(BULLET_STATE::INACTIVE)
	, radiusBullet_(0.0f)
	, radiusBlast_(0.0f)
	, shotSpeedXZ_(0.0f)
	, shotSpeedY_(0.0f)
	, throwPow_(UtilityMath::VECTOR_ZERO)
	, throwDir_(UtilityMath::VECTOR_ZERO)
	, curGravityPow_(0.0f)
	, aliveTime_(0.0f)
	, shotCnt_(0)
	, isVisible_(false)
	, power_(0)
	, activePowerBullet_(0)
	, activePowerBlast_(0)
	, isActiveDestroy_(false)
	, IS_GRAVITY(_isGravity)
{
}

void PBulletBase::InitCollider(void)
{
	// 再初期化時、処理を終了
	if (!ownColliders_.empty()) { return; }

	// 衝突判定マネージャに登録
	ColliderSphere* bullet = new ColliderSphere(ColliderBase::TAG::PLAYER_BULLET, &transform_
									, UtilityMath::VECTOR_ZERO, radiusBullet_);
	ownColliders_[static_cast<int>(COLLISION_TYPE::BULLET)]
		.emplace_back(bullet);

	ColliderSphere* blast = new ColliderSphere(ColliderBase::TAG::PLAYER_BLAST, &transform_
									, UtilityMath::VECTOR_ZERO, radiusBullet_);
	ownColliders_[static_cast<int>(COLLISION_TYPE::BLAST)]
		.emplace_back(blast);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, true);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
}

void PBulletBase::InitPost(void)
{
	isVisible_ = true;

	bulletState_ = BULLET_STATE::INACTIVE;

	activePowerBullet_ = 0;
	activePowerBlast_  = 0;

	isActiveDestroy_ = false;

	radiusBlast_ = 0.0f;

	SetParam();

	ownColliders_.at(static_cast<int>(COLLISION_TYPE::BLAST))
		.at(0)->SetRadius(radiusBlast_);
}

void PBulletBase::Update(void)
{
	if (bulletState_ == BULLET_STATE::SHOT)
	{
		VECTOR pos = throwPow_;

		if (IS_GRAVITY)
		{
			curGravityPow_ += (Application::GetInstance().GetGravityPow() * timeManager_.GetDeltaTime());
			pos.y -= curGravityPow_;
		}

		transform_.Translate(pos);

		if (aliveTime_ > 0.0f)
		{
			aliveTime_ -= timeManager_.GetDeltaTime();
		}
		else
		{
			BlastAction();
		}
	}

	// 弾別の更新処理
	UpdatePost();


	if (bulletState_ == BULLET_STATE::INACTIVE
		|| bulletState_ == BULLET_STATE::BLAST) {
		return;
	}


	for (auto tag : HIT_TARGET_TAGS)
	{
		if (CollisionController::GetInstance().IsActorCollidingWithTag(this, tag))
		{
			BlastAction();
			return;
		}
	}

	// ステージに衝突時、爆発処理
	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE)
		|| throwPow_.y < 0.0f)
	{
		BlastAction();
	}
}

void PBulletBase::Draw(void)
{
	if (transform_.modelId == -1
		&& isVisible_)
	{
		DrawSphere3D(transform_.pos, radiusBullet_, SPHERE_DIV, 0xffffff, 0xffffff, true);
	}

#ifdef _DEBUG
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		DrawSphere3D(transform_.pos, radiusBlast_, SPHERE_DIV, 0xff0000, 0xffffff, false);
	}
#endif
}

void PBulletBase::ReleasePost(void)
{
	
}

void PBulletBase::ChangeBulletState(BULLET_STATE _state)
{
	bulletState_ = _state;

	ChangeBulletStateProc();
}


void PBulletBase::Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt)
{
	shotCnt_ = _shotCnt;

	SetParam();

	bulletState_ = BULLET_STATE::INACTIVE;

	curGravityPow_ = 0.0f;
	throwDir_ = _throwDir;
	transform_.pos = VAdd(_pos, VScale(_throwDir, radiusBullet_));

	transform_.Update();

	isVisible_ = true;
}

void PBulletBase::Shot(const VECTOR& _shotDir)
{
	VECTOR shotDir = ((UtilityMath::EqualsVZero(_shotDir))
							? throwDir_ : _shotDir);


	if (!IS_GRAVITY)
	{
		throwPow_ = VScale(UtilityMath::VNormalize(shotDir), shotSpeedXZ_);
	}
	else
	{
		VECTOR shotPowXZ = VScale(UtilityMath::VNormalize(shotDir), shotSpeedXZ_);
		float shotPowY = VScale(UtilityMath::VNormalize(shotDir), shotSpeedY_).y;

		throwPow_.x = shotPowXZ.x;
		throwPow_.y = shotPowY;
		throwPow_.z = shotPowXZ.z;
	}

	bulletState_ = BULLET_STATE::SHOT;

	curGravityPow_ = 0.0f;

	transform_.Update();

	// 当たり判定登録
	ownColliders_.at(static_cast<int>(COLLISION_TYPE::BULLET)).at(0)->SetRadius(radiusBullet_);
	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, true);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
}

bool PBulletBase::IsAlive(void) const
{
	return(bulletState_ != BULLET_STATE::INACTIVE && !isActiveDestroy_);
}
void PBulletBase::SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir)
{
	// 追従位置割り当て
	transform_.pos = VAdd(_pos, VScale(_offsetDir, radiusBullet_));
}
