#include "PBulletBig.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../../Manager/System/TimeManager.h"

constexpr float RADIUS_BIG = 9.0f;
constexpr float RADIUS_INCREMENT = 22.5f;
constexpr float RADIUS_BIG_BLAST = RADIUS_BIG + (RADIUS_INCREMENT * 4);
constexpr float SCALE_BIG = 1.5f;
constexpr float SCALE_BIG_INCREMENT = 2.5f;
constexpr float SHOT_SPEED_BIG_XZ = 10.0f;
constexpr float SHOT_SPEED_BIG_Y  = 7.5f;
constexpr float TIME_ALIVE_BIG = 5.0f;
constexpr int POWER_START = 50;
constexpr int POWER_INCREMENT = 50;


PBulletBig::PBulletBig(void)
	: PBulletBase::PBulletBase()
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

void PBulletBig::InitPost(void)
{
	SetParam();
}

void PBulletBig::UpdatePost(void)
{
	if (isScaleUp_ && radius_ < radiusMax_)
	{
		constexpr float RADIUS_DURATION = 1.0f;
		scaleUpTime_ += timeManager_.GetDeltaTime();

		float term = (scaleUpTime_ / RADIUS_DURATION);
		term = std::clamp(term, 0.0f, 1.0f);

		radius_ += ((radiusMax_ - radius_) * (term * term));
		transform_.SetScale((scaleMax_ * (term * term)));
	}
}

void PBulletBig::ChangeBulletState(BULLET_STATE _state)
{
	bulletState_ = _state;

	if (bulletState_ == BULLET_STATE::SHOT)
	{
		transform_.SetScale(RADIUS_BIG);
	}
}

void PBulletBig::SetParam(void)
{
	radius_ = RADIUS_BIG;
	radiusMax_ = RADIUS_BIG + (RADIUS_INCREMENT * shotCnt_);

	shotSpeedXZ_ = SHOT_SPEED_BIG_XZ;
	shotSpeedY_ = SHOT_SPEED_BIG_Y;

	aliveTime_ = TIME_ALIVE_BIG;

	scaleUpTime_ = 0.0f;

	power_ = POWER_START + (POWER_INCREMENT * shotCnt_);
	scaleMax_ = SCALE_BIG + (SCALE_BIG_INCREMENT * shotCnt_);
	transform_.InitTransform(SCALE_BIG, transform_.quaRot, Quaternion::Identity());
}
