#include "PBulletNormal.h"
#include <algorithm>
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../Collision/CollisionController.h"


PBulletNormal::PBulletNormal(float _scale, float _radius, int _power
    , float _shotSpeedXZ, float _shotSpeedY, float _aliveTime
	, int _shotType, bool _isActiveGravity)
	: PBulletBase::PBulletBase(_shotType, _isActiveGravity)
	, ALIVE_TIME(_aliveTime)
	, SCALE(_scale)
{
	shotSpeedXZ_ = _shotSpeedXZ;
	shotSpeedY_ = _shotSpeedY;
	power_ = _power;
	radiusBullet_ = _radius;
}

void PBulletNormal::InitTransform(void)
{
	transform_.InitTransform(SCALE, transform_.quaRot, Quaternion::GetIdentity());
}

void PBulletNormal::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		// 着弾時、即無効化
		bulletState_ = BULLET_STATE::INACTIVE;
		activePowerBullet_ = 0;
	}
}
void PBulletNormal::SetParam(void)
{
	// パラメータ割り当て
	aliveTime_ = ALIVE_TIME;
}

void PBulletNormal::BlastAction(void)
{
	// 着弾状態へ移行(弾の当たり判定は無効化)
	BeginBlast();
	activePowerBullet_ = power_;

	// 弾を消滅させる
	isActiveDestroy_ = true;
}