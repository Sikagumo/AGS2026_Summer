#include "PBulletBig.h"
#include "PBulletBase.h"

constexpr float RADIUS_BIG = 15.0f;
constexpr float RADIUS_INCREMENT = 10.0f;
constexpr float RADIUS_BIG_BLAST = RADIUS_BIG + (RADIUS_INCREMENT * 4);
constexpr float SCALE_BIG = 1.5f;
constexpr float SCALE_BIG_INCREMENT = 1.5f;
constexpr float SHOT_SPEED_BIG = 7.5f;
constexpr float TIME_ALIVE_BIG = 5.0f;


PBulletBig::PBulletBig(void)
	:PBulletBase::PBulletBase()
{
}

void PBulletBig::InitLoad(void)
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

	shotSpeed_ = SHOT_SPEED_BIG;

	aliveTime_ = TIME_ALIVE_BIG;

	transform_.InitTransform(SCALE_BIG, transform_.quaRot, Quaternion::Identity());
}
