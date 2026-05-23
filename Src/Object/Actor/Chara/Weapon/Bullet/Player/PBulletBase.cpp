#include "PBulletBase.h"
#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/Generic/SceneManager.h"
#include "../../../../../../Application.h"
#include "../../../../../Manager/CollisionManager.h"
#include "../../../../../Collider/ColliderSphere.h"

PBulletBase::PBulletBase(void)
	: ActorBase::ActorBase()
	, bulletState_(BULLET_STATE::INACTIVE)
	, radius_(0.0f)
	, shotSpeed_(0.0f)
	, shotPow_(UtilityMath::VECTOR_ZERO)
	, curGravityPow_(0.0f)
	, aliveTime_(0.0f)
	, shotCnt_(0)
	, isVisible_(false)
{
}

void PBulletBase::InitCollider(void)
{
	// è’ìÀîªíËÉ}ÉlÅ[ÉWÉÉÇ…ìoò^
	ownColliders_.emplace(0
		, new ColliderSphere(ColliderBase::TAG::PLAYER_BULLET, &transform_, UtilityMath::VECTOR_ZERO, radius_));
}

void PBulletBase::InitPost(void)
{
	isVisible_ = true;
}


void PBulletBase::Update(void)
{
	if (bulletState_ == BULLET_STATE::SHOT)
	{
		VECTOR pos = shotPow_;
		curGravityPow_ += (Application::GRAVITY * sceneManager_.GetDeltaTime());
		pos.y -= curGravityPow_;

		transform_.Translate(pos);

		if (aliveTime_ <= 0)
		{
			bulletState_ = BULLET_STATE::INACTIVE;
		}
	}
	else if (bulletState_ == BULLET_STATE::BLAST)
	{

	}
	// è’ìÀéû
	//bulletState_ = BULLET_STATE::BLAST;

	UpdatePost();

	CollisionManager& colMng = CollisionManager::GetInstance();

	//if (colMng.IsTagCollidingWithTag(, ColliderBase::TAG::BOSS))
	if (colMng.IsActorCollidingWithTag(this, ColliderBase::TAG::BOSS))
	{
		// damageController_.É_ÉÅÅ[ÉWìoò^(power_);
		// 
		// è’ìÀîªíËÉ}ÉlÅ[ÉWÉÉÇ…ìoò^
		ownColliders_.emplace(0
			, new ColliderSphere(ColliderBase::TAG::PLAYER_BULLET, &transform_, UtilityMath::VECTOR_ZERO, radius_));

	}
	else 
	{
		//if (colMng.IsActorCollidingWithTag(this, ColliderBase::TAG::WEAPON_CANNON)
		// damageController_.É_ÉÅÅ[ÉWìoò^(power_ / 0.8f);
	}
}

void PBulletBase::Draw(void)
{
	if (!isVisible_) { return; }

	if (transform_.modelId == -1)
	{
		constexpr int SPHERE_DIV = 16;
		DrawSphere3D(transform_.pos, radius_, SPHERE_DIV, 0xffffff, 0xffffff, false);
	}
}

void PBulletBase::Release(void)
{
	
}

void PBulletBase::Create(const VECTOR& _pos, int _shotCnt)
{
	shotCnt_ = _shotCnt;

	SetParam();

	bulletState_ = BULLET_STATE::INACTIVE;

	isVisible_ = true;

	curGravityPow_ = 0.0f;
	transform_.pos = _pos;

	transform_.Update();
}

void PBulletBase::Shot(const VECTOR& _shotDir, const Quaternion& _rot)
{
	shotPow_ = VScale(UtilityMath::VNormalize(_shotDir), shotSpeed_);

	bulletState_ = BULLET_STATE::SHOT;

	curGravityPow_ = 0.0f;

	transform_.Update();

	CollisionManager::GetInstance().RegisterActor(this);
}

bool PBulletBase::IsAlive(void) const
{
	return(bulletState_ != BULLET_STATE::INACTIVE
			|| aliveTime_ <= 0.0f
			|| isVisible_);
}
void PBulletBase::SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir)
{
	// í«è]à íuäÑÇËìñÇƒ
	transform_.pos = VAdd(_pos, VScale(_offsetDir, radius_));
}
