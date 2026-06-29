#include "PBulletBase.h"
#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Application.h"
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../Collider/ColliderSphere.h"

PBulletBase::PBulletBase(void)
	: ActorBase::ActorBase()
	, bulletState_(BULLET_STATE::INACTIVE)
	, radius_(0.0f)
	, shotSpeedXZ_(0.0f), shotSpeedY_(0.0f)
	, shotPow_(UtilityMath::VECTOR_ZERO)
	, curGravityPow_(0.0f)
	, aliveTime_(0.0f)
	, shotCnt_(0)
	, isVisible_(false)
	, isFinish_(false)
	, power_(0), activePower_(0)
	, isActiveDestroy_(false)
{
}

void PBulletBase::InitCollider(void)
{
	// 再初期化時、処理を終了
	if (!ownColliders_.empty()) { return; }

	// 衝突判定マネージャに登録
	ColliderSphere* sphere = new ColliderSphere(ColliderBase::TAG::PLAYER_BULLET, &transform_, UtilityMath::VECTOR_ZERO, radius_);
	ownColliders_.emplace(0, sphere);
}

void PBulletBase::InitPost(void)
{
	isVisible_ = true;
	bulletState_ = BULLET_STATE::INACTIVE;
	SetParam();
}


void PBulletBase::Update(void)
{
	if (bulletState_ == BULLET_STATE::SHOT)
	{
		VECTOR pos = shotPow_;
		curGravityPow_ += (Application::GRAVITY * timeManager_.GetDeltaTime());
		pos.y -= curGravityPow_;

		transform_.Translate(pos);

		if (aliveTime_ <= 0)
		{
			BlastAction();
		}
	}
	else if (bulletState_ == BULLET_STATE::BLAST && isActiveDestroy_)
	{
		activePower_ = 0;
		bulletState_ = BULLET_STATE::INACTIVE;
		return;
	}

	UpdatePost();


	if (bulletState_ == BULLET_STATE::INACTIVE
		|| bulletState_ == BULLET_STATE::BLAST) {
		return;
	}


	const std::vector<ColliderBase::TAG> BOSS_TAG
		= { ColliderBase::TAG::BOSS
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
		&& shotPow_.y < 0.0f)
	{
		BlastAction();
	}
}

void PBulletBase::Draw(void)
{
	if (!isVisible_) { return; }

	if (transform_.modelId == -1)
	{
		constexpr int SPHERE_DIV = 16;
		DrawSphere3D(transform_.pos, radius_, SPHERE_DIV, 0xffffff, 0xffffff, true);
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

	isVisible_ = true;

	curGravityPow_ = 0.0f;
	transform_.pos = VAdd(_pos, VScale(_throwDir, radius_));

	isFinish_ = _isFinish;

	transform_.Update();
}

void PBulletBase::Shot(const VECTOR& _shotDir)
{
	VECTOR shotPowXZ = VScale(UtilityMath::VNormalize(_shotDir), shotSpeedXZ_);
	float shotPowY = VScale(UtilityMath::VNormalize(_shotDir), shotSpeedY_).y;
	shotPow_.x = shotPowXZ.x;
	shotPow_.y = shotPowY;
	shotPow_.z = shotPowXZ.z;

	bulletState_ = BULLET_STATE::SHOT;

	curGravityPow_ = 0.0f;

	transform_.Update();

	// 当たり判定登録
	ownColliders_.at(0)->SetRadius(radius_);
	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, true);
}

bool PBulletBase::IsAlive(void) const
{
	return(bulletState_ != BULLET_STATE::INACTIVE
			|| aliveTime_ <= 0.0f
			|| isVisible_);
}
void PBulletBase::SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir)
{
	// 追従位置割り当て
	transform_.pos = VAdd(_pos, VScale(_offsetDir, radius_));
}
