#include "PBulletBig.h"
#include <algorithm>
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../Collision/CollisionController.h"

constexpr float RADIUS_BIG = 9.0f;
constexpr float RADIUS_INCREMENT = 27.5f;
constexpr float RADIUS_BIG_BLAST = RADIUS_BIG + (RADIUS_INCREMENT * 4);
constexpr float SCALE_BIG = 1.5f;
constexpr float SCALE_BIG_INCREMENT = 2.5f;
constexpr float TIME_ALIVE_BIG = 5.0f;

// çUåÇóÕ
constexpr int POWER_START = 50;
constexpr int POWER_INCREMENT = 75;

// ägëÂäÆóπÇ‹Ç≈ÇÃéûä‘
constexpr float RADIUS_DURATION = 1.0f;



PBulletBig::PBulletBig(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
	, radiusMax_(0.0f), scaleMax_(0.0f)
	, scaleUpTime_(0.0f), isScaleUp_(false)
{
}

void PBulletBig::Load(void)
{
}

void PBulletBig::InitTransform(void)
{

}

void PBulletBig::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		// íÖíeéûÅAë¶è¡ñ≈Ç≥ÇπÇÈ
		activePowerBullet_ = 0;
		bulletState_ = BULLET_STATE::INACTIVE;

		// ìñÇΩÇËîªíËñ≥å¯âª
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);
		return;
	}


	if (isScaleUp_ && radiusBullet_ < radiusMax_)
	{
		/* íeä€ägëÂèàóù */
		scaleUpTime_ += timeManager_.GetDeltaTime();

		float term = (scaleUpTime_ / RADIUS_DURATION);
		term = std::clamp(term, 0.0f, 1.0f);

		radiusBullet_ += ((radiusMax_ - radiusBullet_) * (term * term));
		transform_.SetScale((scaleMax_ * (term * term)));
	}
}

void PBulletBig::ChangeBulletStateProc(void)
{
	if (bulletState_ == BULLET_STATE::SHOT)
	{
		transform_.SetScale(RADIUS_BIG);
	}
}

void PBulletBig::SetParam(void)
{
	radiusBullet_ = RADIUS_BIG;
	radiusMax_ = RADIUS_BIG + (RADIUS_INCREMENT * shotCnt_);

	shotSpeedXZ_ = SHOT_SPEED_BIG_XZ;
	shotSpeedY_ = SHOT_SPEED_BIG_Y;

	aliveTime_ = TIME_ALIVE_BIG;

	scaleUpTime_ = 0.0f;

	isScaleUp_ = false;

	power_ = POWER_START + (POWER_INCREMENT * shotCnt_);
	scaleMax_ = SCALE_BIG + (SCALE_BIG_INCREMENT * shotCnt_);
	transform_.InitTransform(SCALE_BIG, transform_.quaRot, Quaternion::Identity());
}

void PBulletBig::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activePowerBullet_ = power_;

	// íeÇè¡ñ≈Ç≥ÇπÇÈ
	isActiveDestroy_ = true;
}