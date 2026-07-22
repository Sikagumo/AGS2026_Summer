#include <DxLib.h>
#include "../../../../Application.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../Collision/CollisionController.h"
#include "WeaponBase.h"

WeaponBase::WeaponBase(void)
{
}


void WeaponBase::Update(void)
{
	// 各キャラクターごとの更新処理


	UpdateProcess();

	// 重力処理
	if (!isAlive_)
	{
		CalcGravityPow();
	}
	// 衝突判定
	Collision();



	transform_.Update();

	UpdateProcessPost();
}





void WeaponBase::ReleasePost(void)
{
}

void WeaponBase::DrawPre(void)
{
	
}

void WeaponBase::CalcGravityPow(void)
{
	// 重力の強さ
	float gravityPow = Application::GetInstance().GetGravityPow() * timeManager_.GetDeltaTime();
	const VECTOR GRAVITY_POW = VScale(UtilityMath::DIR_DOWN, gravityPow);

	// 重力
	jumpPow_ += GRAVITY_POW.y;

	// 重力制限	
	jumpPow_ = ((jumpPow_ < MAX_FALL_SPEED)
		? MAX_FALL_SPEED
		: jumpPow_);
}

void WeaponBase::Collision(void)
{
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	// 衝突(重力)
	CollisionGravity();

	// ジャンプ量を加算
	transform_.pos.y += jumpPow_;
}

void WeaponBase::CollisionGravity(void)
{
	bool isHitStage = CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE);

	// 床に触れていて、かつ下方向に落下している（または静止している）なら着地
	if (isHitStage && jumpPow_ <= 0.0f)
	{
		isJump_ = false;
		jumpPow_ = 0.0f; // 落下速度を止める
	}
}

void WeaponBase::ChangeState(STATE _state)
{
}

void WeaponBase::ChangeState(int state)
{
}

void WeaponBase::ChangeStateIdle(void)
{
}

void WeaponBase::ChangeStateAttack(void)
{
}

void WeaponBase::ChangeStateEnd(void)
{
}

void WeaponBase::UpdateAttack(void)
{
}

void WeaponBase::UpdateIdle(void)
{
}

void WeaponBase::UpdateEnd(void)
{
}
