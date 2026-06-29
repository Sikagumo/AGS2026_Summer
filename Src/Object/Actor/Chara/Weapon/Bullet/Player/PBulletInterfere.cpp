#include "PBulletInterfere.h"
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


PBulletInterfere::PBulletInterfere(void)
	: PBulletBase::PBulletBase()
	, timeEndActive_(0.0f)
	, scaleUpTime_(0.0f), isScaleUp_(false)
{
}

void PBulletInterfere::Load(void)
{
}

void PBulletInterfere::InitTransform(void)
{
	
}

void PBulletInterfere::InitPost(void)
{
	PBulletBase::InitPost();

}

void PBulletInterfere::UpdatePost(void)
{
	
}

void PBulletInterfere::ChangeBulletStateProc(void)
{
	if (bulletState_ == BULLET_STATE::SHOT)
	{
		transform_.SetScale(RADIUS_BIG);
	}
}

void PBulletInterfere::SetParam(void)
{

	shotSpeedXZ_ = SHOT_SPEED_BIG_XZ;
	shotSpeedY_ = SHOT_SPEED_BIG_Y;

	aliveTime_ = TIME_ALIVE_BIG;

	scaleUpTime_ = 0.0f;

	isScaleUp_ = false;

	power_ = POWER_START + (POWER_INCREMENT * shotCnt_);
	transform_.InitTransform(SCALE_BIG, transform_.quaRot, Quaternion::Identity());
}

void PBulletInterfere::BlastAction(void)
{

}