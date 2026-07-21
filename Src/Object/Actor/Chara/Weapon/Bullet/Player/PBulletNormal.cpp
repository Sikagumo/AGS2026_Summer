#include "PBulletNormal.h"
#include <algorithm>
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../Collision/CollisionController.h"


PBulletNormal::PBulletNormal(float _scale, float _radius, int _power
		, float _shotSpeedXZ, float _shotSpeedY, float _aliveTime
	, int _shotType, bool _isActiveGravity)
	: PBulletBase::PBulletBase(_shotType, _isActiveGravity)
	, ALIVE_TIME(_aliveTime), SCALE(_scale)
{
	shotSpeedXZ_ = _shotSpeedXZ;
	shotSpeedY_ = _shotSpeedY;
	power_ = _power;
	radiusBullet_ = _radius;
}

void PBulletNormal::Load(void)
{
}

void PBulletNormal::PreActiveProcess(void)
{
}

void PBulletNormal::InitTransform(void)
{

}

void PBulletNormal::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		bulletState_ = BULLET_STATE::INACTIVE;
	}
}

void PBulletNormal::ChangeBulletStateProc(void)
{
}

void PBulletNormal::SetParam(void)
{
	transform_.InitTransform(SCALE, transform_.quaRot, Quaternion::Identity());
	aliveTime_ = ALIVE_TIME;
}

void PBulletNormal::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activePowerBullet_ = power_;

	// íeÇè¡ñ≈Ç≥ÇπÇÈ
	isActiveDestroy_ = true;

	// ìñÇΩÇËîªíËñ≥å¯âª
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);
}