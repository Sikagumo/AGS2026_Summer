#include "PBulletBase.h"
#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/Generic/SceneManager.h"
#include "../../../../../../Application.h"

PBulletBase::PBulletBase(void)
	: ActorBase::ActorBase()
	, bulletState_(BULLET_STATE::INACTIVE)
	, radius_(0.0f)
	, shotSpeed_(0.0f)
	, shotPow_(UtilityMath::VECTOR_ZERO)
	, curGravityPow_(0.0f)
	, aliveTime_(0.0f)
	, shotCnt_(0)
{
}

void PBulletBase::InitCollider(void)
{
	// ‹…‚Ì“–‚½‚è”»’è‚ð“±“ü
}

void PBulletBase::InitPost(void)
{
}


void PBulletBase::Update(void)
{
	if (!IsAlive()) { return; }

	if (bulletState_ == BULLET_STATE::SHOT)
	{
		VECTOR pos = shotPow_;
		curGravityPow_ += (Application::GRAVITY * sceneManager_.GetDeltaTime());
		pos.y -= curGravityPow_;

		transform_.Translate(pos);
		auto temp = transform_.pos;

		if (aliveTime_ <= 0)
		{
			bulletState_ = BULLET_STATE::INACTIVE;
		}
	}
	else if (bulletState_ == BULLET_STATE::BLAST)
	{

	}
	// Õ“ËŽž
	//bulletState_ = BULLET_STATE::BLAST;
}

void PBulletBase::Draw(void)
{
	if (!IsAlive()) { return; }
	if (transform_.modelId == -1)
	{
		constexpr int SPHERE_DIV = 16;
		DrawSphere3D(transform_.pos, radius_, SPHERE_DIV, 0xffffff, 0xffffff, true);
	}
}

void PBulletBase::Release(void)
{
	
}

void PBulletBase::CreateShot(const VECTOR& _pos, const VECTOR& _shotDir, const Quaternion& _rot, int _shotCnt)
{
	shotCnt_ = _shotCnt;

	SetParam();

	shotPow_ = VScale(UtilityMath::VNormalize(_shotDir), shotSpeed_);

	bulletState_ = BULLET_STATE::SHOT;

	curGravityPow_ = 0.0f;
	transform_.pos = _pos;

	transform_.Update();
}

bool PBulletBase::IsAlive(void) const
{
	return(bulletState_ != BULLET_STATE::INACTIVE
			|| aliveTime_ <= 0.0f);
}
