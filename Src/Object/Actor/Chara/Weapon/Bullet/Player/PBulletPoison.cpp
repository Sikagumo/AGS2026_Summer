#include "PBulletPoison.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Manager/Decoration/SoundManager.h"

PBulletPoison::PBulletPoison(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
	, activeTimePoison_(0.0f)
{
}

PBulletPoison::~PBulletPoison(void)
{
	// メモリ解放
	Release();
}

void PBulletPoison::Load(void)
{
	// 効果音読み込み
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
			// 当たり判定無効化
			CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
			bulletState_ = BULLET_STATE::INACTIVE;
			activePowerBlast_ = 0;
			return;
		}

		if (activeTimePoison_ > 0.0f)
		{
			activeTimePoison_ -= TimeManager::GetInstance().GetDeltaTime();
		}
		else
		{
			// 毒の継続時間が終了したら消滅させる
			isActiveDestroy_ = true;
		}
	}
}

void PBulletPoison::SetParam(void)
{
	// 発射速度割り当て
	shotSpeedXZ_ = SHOT_SPEED_POISON_XZ;
	shotSpeedY_ = SHOT_SPEED_POISON_Y;

	// 生存時間割り当て
	aliveTime_ = TIME_ALIVE_POISON;

	// 当たり判定の半径割り当て
	radiusBullet_ = RADIUS_BULLET;
	radiusBlast_ = 0.0f;

	// 攻撃力割り当て
	power_ = POWER_POISON;
}

void PBulletPoison::BlastAction(void)
{
	// 着弾状態へ移行(弾の当たり判定は無効化)
	BeginBlast();
	
	// 攻撃有効化
	activePowerBlast_ = power_;
	activeTimePoison_ = ACTIVE_TIME;

	// 毒範囲有効化
	ActivateBlastCollider(RADIUS_POISON);

	// エフェクト再生
	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_PLAYER_POISON
		, transform_.pos, Quaternion::Identity().ToEuler()
		, { POISON_EFFECT_SCALE, POISON_EFFECT_SCALE, POISON_EFFECT_SCALE }
		, POISON_EFFECT_SPEED, this);

	// SE再生
	SoundManager::GetInstance().Play(SoundManager::SOUND::SE_PBULLET_POISON);
}