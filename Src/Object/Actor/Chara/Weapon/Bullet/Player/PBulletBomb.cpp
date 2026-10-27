#include "PBulletBomb.h"
#include "PBulletBase.h"
#include <algorithm>
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"

PBulletBomb::PBulletBomb(int _shotType)
	: PBulletBase::PBulletBase(_shotType)
{
}

void PBulletBomb::InitTransform(void)
{
	transform_.InitTransform(SCALE_BOMB
		, transform_.quaRot, Quaternion::Identity());
}

void PBulletBomb::UpdatePost(void)
{
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		if (isActiveDestroy_)
		{
			// 爆発を１フレーム有効後、消滅

			// 当たり判定無効化
			CollisionController::GetInstance()
				.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);

			bulletState_ = BULLET_STATE::INACTIVE;
			activePowerBullet_ = 0;
			activePowerBlast_ = 0;

			return;
		}

		// 弾を消滅させる
		isActiveDestroy_ = true;
	}
}

void PBulletBomb::SetParam(void)
{
	shotSpeedXZ_ = SHOT_SPEED_BOMB_XZ;
	shotSpeedY_ = SHOT_SPEED_BOMB_Y;

	aliveTime_ = TIME_ALIVE_BOMB;

	radiusBullet_ = RADIUS_BULLET;
	radiusBlast_ = 0.0f;

	power_ = POWER_BOMB;
}

void PBulletBomb::BlastAction(void)
{
	// 着弾状態へ移行
	BeginBlast();
	activePowerBlast_ = power_;

	// 爆発有効化
	ActivateBlastCollider(RADIUS_BLAST);

	// エフェクト有効化
	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_PLAYER_BLAST
		, transform_.pos, Quaternion::Identity().ToEuler()
		, { BLAST_EFFECT_SCALE , BLAST_EFFECT_SCALE, BLAST_EFFECT_SCALE }
		, BLAST_EFFECT_SPEED, this);
}