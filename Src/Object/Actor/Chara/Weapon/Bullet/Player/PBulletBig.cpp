#include "PBulletBig.h"
#include <algorithm>
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../Collision/CollisionController.h"

PBulletBig::PBulletBig(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
	, radiusMax_(0.0f)
	, scaleMax_(0.0f)
	, scaleUpTime_(0.0f)
	, isScaleUp_(false)
{
}

void PBulletBig::InitTransform(void)
{
	transform_.InitTransform(SCALE_BIG, transform_.quaRot, Quaternion::Identity());
}

void PBulletBig::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		// ’…’eA‘¦Á–Å‚³‚¹‚é
		activePowerBullet_ = 0;
		bulletState_ = BULLET_STATE::INACTIVE;

		// “–‚½‚è”»’è–³Œø‰»
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);
		return;
	}

	// ’e‚ªŠg‘å‚ªI—¹‚Íˆ—I—¹
	if (!isScaleUp_ || radiusBullet_ >= radiusMax_) { return; }

	// ’eŠÛŠg‘åˆ—
	scaleUpTime_ += timeManager_.GetDeltaTime();
	float blendRate_ = (scaleUpTime_ / RADIUS_DURATION);
	blendRate_ = std::clamp(blendRate_, 0.0f, 1.0f);

	// ƒpƒ‰ƒ[ƒ^‚É”½‰f
	radiusBullet_ += ((radiusMax_ - radiusBullet_) * (blendRate_ * blendRate_));
	transform_.SetScale((scaleMax_ * (blendRate_ * blendRate_)));
}

void PBulletBig::SetParam(void)
{
	radiusBullet_ = RADIUS_BIG;
	radiusMax_ = RADIUS_BIG + (RADIUS_INCREMENT * shotCnt_);

	shotSpeedXZ_ = SHOT_SPEED_BIG_XZ;
	shotSpeedY_ = SHOT_SPEED_BIG_Y;

	aliveTime_ = TIME_ALIVE_BIG;

	scaleUpTime_ = 0.0f;

	// Šg‘å–³Œø‰»
	isScaleUp_ = false;

	// UŒ‚—ÍŠ„‚è“–‚Ä
	power_ = POWER_START + (POWER_INCREMENT * shotCnt_);
	scaleMax_ = SCALE_BIG + (SCALE_BIG_INCREMENT * shotCnt_);
}

void PBulletBig::BlastAction(void)
{
	// ƒpƒ‰ƒ[ƒ^–³Œø‰»
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	activePowerBullet_ = power_;

	// ’e‚ğÁ–Å‚³‚¹‚é
	isActiveDestroy_ = true;
}