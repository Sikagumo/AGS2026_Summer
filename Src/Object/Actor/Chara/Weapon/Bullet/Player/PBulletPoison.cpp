#include "PBulletPoison.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Manager/Decoration/SoundManager.h"

constexpr float RADIUS_BULLET = 5.0f;
constexpr float RADIUS_POISON = 100.0f;
constexpr float SCALE_POISON = 1.0f;
constexpr float TIME_ALIVE_POISON = 7.5f;
constexpr int POWER_POISON = 1;
constexpr float ACTIVE_TIME = 2.5f;


PBulletPoison::PBulletPoison(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
	, activeTime_(0.0f)
{
}

void PBulletPoison::Load(void)
{
	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_PBULLET_POISON
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_PBULLET_POISON));
}

void PBulletPoison::InitTransform(void)
{
	transform_.InitTransform(SCALE_POISON
		, transform_.quaRot, Quaternion::Identity());
}

void PBulletPoison::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		if (isActiveDestroy_) 
		{
			// ìñÇΩÇËîªíËñ≥å¯âª
			CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
			bulletState_ = BULLET_STATE::INACTIVE;
			activePowerBlast_ = 0;
			return;
		}

		if (activeTime_ > 0.0f)
		{
			activeTime_ -= TimeManager::GetInstance().GetDeltaTime();
		}
		else
		{
			// íeÇè¡ñ≈Ç≥ÇπÇÈ
			isActiveDestroy_ = true;
		}
	}
}

void PBulletPoison::PreActiveProcess(void)
{
}

void PBulletPoison::ChangeBulletStateProc(void)
{
}

void PBulletPoison::SetParam(void)
{
	shotSpeedXZ_ = SHOT_SPEED_POISON_XZ;
	shotSpeedY_ = SHOT_SPEED_POISON_Y;

	aliveTime_ = TIME_ALIVE_POISON;

	radiusBullet_ = RADIUS_BULLET;
	radiusBlast_ = 0.0f;


	power_ = POWER_POISON;
}

void PBulletPoison::BlastAction(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;

	activePowerBlast_ = power_;
	activeTime_ = ACTIVE_TIME;

	// ìñÇΩÇËîªíËñ≥å¯âª
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, false);

	// îöî≠óLå¯âª
	radiusBlast_ = RADIUS_POISON;

	ownColliders_[static_cast<int>(COLLISION_TYPE::BLAST)]
		.at(0)->SetRadius(radiusBlast_);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, true);

	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_PLAYER_POISON,
		transform_.pos, Quaternion::Identity().ToEuler(),
		{ 35.0f , 35.0f, 35.0f }, 1.0f, this);

	SoundManager::GetInstance().Play(SoundManager::SOUND::SE_PBULLET_POISON);
}