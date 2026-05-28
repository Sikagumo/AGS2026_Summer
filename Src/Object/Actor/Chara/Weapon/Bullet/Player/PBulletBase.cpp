#include "PBulletBase.h"
#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Application.h"
#include "../../../../../Manager/CollisionManager.h"
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
{
}

void PBulletBase::InitCollider(void)
{
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
		curGravityPow_ += (Application::GRAVITY * timeManager_.GetDeltaTime());
		pos.y -= curGravityPow_;

		transform_.Translate(pos);

		if (aliveTime_ <= 0)
		{
			BlastAction();
		}
	}
	else if (bulletState_ == BULLET_STATE::BLAST)
	{
		activePower_ = 0;
		bulletState_ = BULLET_STATE::INACTIVE;
	}
	// Õ“ËŽž
	//bulletState_ = BULLET_STATE::BLAST;

	UpdatePost();

	CollisionManager& colMng = CollisionManager::GetInstance();

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
	for (auto tag : BOSS_TAG)
	{
		if (colMng.IsActorCollidingWithTag(this, tag))
		{
			BlastAction();
			return;
		}
	}
	if (colMng.IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE) && shotPow_.y < 0.0f)
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

void PBulletBase::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activePower_ = power_;

	// “–‚½‚è”»’è–³Œø‰»
	CollisionManager::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);


	// Õ“Ë”»’èƒ}ƒl[ƒWƒƒ‚É“o˜^
	//ColliderSphere* blast = new ColliderSphere(ColliderBase::TAG::PLAYER_BULLET, &transform_, UtilityMath::VECTOR_ZERO, radius_);
	//ownColliders_.emplace(0, blast);
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


	if (isFinish_)
	{
		constexpr float SHOT_POW = 1.5f;
		shotPow_.y *= SHOT_POW;
	}
	bulletState_ = BULLET_STATE::SHOT;

	curGravityPow_ = 0.0f;

	transform_.Update();

	// Õ“Ë”»’èƒ}ƒl[ƒWƒƒ‚É“o˜^
	ColliderSphere* sphere = new ColliderSphere(ColliderBase::TAG::PLAYER_BULLET, &transform_, UtilityMath::VECTOR_ZERO, radius_);
	ownColliders_.emplace(0, sphere);

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
	// ’Ç]ˆÊ’uŠ„‚è“–‚Ä
	transform_.pos = VAdd(_pos, VScale(_offsetDir, radius_));
}
