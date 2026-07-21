#include "PBulletBase.h"
#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"
#include "../../../../../../Application.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collision/CollisionController.h"

PBulletBase::PBulletBase(int _shotType, bool _isGravity)
	: ActorBase::ActorBase()
	, shotType_(_shotType)
	, bulletState_(BULLET_STATE::INACTIVE)
	, radiusBullet_(0.0f) , radiusBlast_(0.0f)
	, shotSpeedXZ_(0.0f), shotSpeedY_(0.0f)
	, throwPow_(UtilityMath::VECTOR_ZERO)
	, curGravityPow_(0.0f)
	, aliveTime_(0.0f)
	, shotCnt_(0)
	, isVisible_(false)
	, isFinish_(false)
	, power_(0), activePowerBullet_(0), activePowerBlast_(0)
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
}

void PBulletBase::InitPost(void)
{
	isVisible_ = true;
	bulletState_ = BULLET_STATE::INACTIVE;
	activePowerBullet_ = 0;
	activePowerBlast_ = 0;
	isActiveDestroy_ = false;

	SetParam();
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

		if (aliveTime_ <= 0.0f)
		{
			BlastAction();
		}
	}

	UpdatePost();


	if (bulletState_ == BULLET_STATE::INACTIVE
		|| bulletState_ == BULLET_STATE::BLAST) {
		return;
	}


	const std::vector<ColliderBase::TAG> BOSS_TAG
		= { ColliderBase::TAG::BOSS, ColliderBase::TAG::ENEMY, ColliderBase::TAG::ENEMYROBO
			, ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::WEAPON_CANNON_R
			, ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::WEAPON_MG_R
			, ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::WEAPON_MP_R
			, ColliderBase::TAG::WEAPON_RG};

	CollisionController& colMng = CollisionController::GetInstance();

	for (auto tag : BOSS_TAG)
	{
		if (colMng.IsActorCollidingWithTag(this, tag))
		{
			BlastAction();
			return;
		}
	}

	// ステージに衝突時、爆発処理
	if (colMng.IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE)
		|| throwPow_.y < 0.0f)
	{
		BlastAction();
	}
}

void PBulletBase::Draw(void)
{
	constexpr int SPHERE_DIV = 16;

	if (transform_.modelId == -1
		&& isVisible_)
	{
		DrawSphere3D(transform_.pos, radiusBullet_, SPHERE_DIV, 0xffffff, 0xffffff, true);
	}


	if (bulletState_ == BULLET_STATE::BLAST)
	{
#ifdef _DEBUG
		DrawSphere3D(transform_.pos, radiusBlast_, SPHERE_DIV, 0xff0000, 0xffffff, false);
#endif
	}
}

void PBulletBase::ReleasePost(void)
{
	
}

void PBulletBase::ChangeBulletState(BULLET_STATE _state)
{
	bulletState_ = _state;

	ChangeBulletStateProc();
}


void PBulletBase::Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt, bool _isFinish)
{
	shotCnt_ = _shotCnt;

	SetParam();

	bulletState_ = BULLET_STATE::INACTIVE;

	curGravityPow_ = 0.0f;
	throwDir_ = _throwDir;
	transform_.pos = VAdd(_pos, VScale(_throwDir, radiusBullet_));

	isFinish_ = _isFinish;

	transform_.Update();

	isVisible_ = true;
}

void PBulletBase::Shot(const VECTOR& _shotDir)
{
	VECTOR shotDir = ((UtilityMath::EqualsVZero(_shotDir))
							? throwDir_ : _shotDir);

	VECTOR shotPowXZ = VScale(UtilityMath::VNormalize(_shotDir), shotSpeedXZ_);
	float shotPowY = VScale(UtilityMath::VNormalize(_shotDir), shotSpeedY_).y;

	if (!IS_GRAVITY)
	{
		throwPow_ = VScale(UtilityMath::VNormalize(shotDir), shotSpeedXZ_);
	}
	else
	{
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
	return(bulletState_ != BULLET_STATE::INACTIVE
			&& !isActiveDestroy_);
}
void PBulletBase::SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir)
{
	// 追従位置割り当て
	transform_.pos = VAdd(_pos, VScale(_offsetDir, radiusBullet_));
}
