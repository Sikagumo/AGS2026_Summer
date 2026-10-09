#include <DxLib.h>
#include "../../../../Application.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../Collision/CollisionController.h"
#include "WeaponBase.h"

WeaponBase::WeaponBase(void)
	: hp_(500)
	, isAlive_(true)
	, movePow_(VGet(0.0f, 0.0f, 0.0f))
	, jumpPow_(0.0f)
	, isJump_(false)
	, moveDir_(VGet(0.0f, 0.0f, 0.0f))
	, speed_(0.0f)
	, localBackPos_(VGet(0.0f, 0.0f, 0.0f))
	, localFrontPos_(VGet(0.0f, 0.0f, 0.0f))
	, localPos_(VGet(0.0f, 0.0f, 0.0f))
	, tag_(ColliderBase::TAG::STAGE)
	, state_(STATE::IDLE)
	, stateBase_(0)
	, timeCount_(0.0f)
{
}

void WeaponBase::Update(void)
{
	// 各武装ごとの固有更新処理
	UpdateProcess();

	// 死亡時（破壊時）の重力・物理計算
	if (!isAlive_)
	{
		CalcGravityPow();
	}

	// 衝突判定と移動適用
	Collision();

	// トランスフォーム行列の更新
	transform_.Update();

	// 更新後の後処理
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
	// 重力の計算
	float gravityPow = Application::GetInstance().GetGravityPow() * timeManager_.GetDeltaTime();
	const VECTOR GRAVITY_POW = VScale(UtilityMath::DIR_DOWN, gravityPow);

	jumpPow_ += GRAVITY_POW.y;

	// 落下速度制限
	if (jumpPow_ < MAX_FALL_SPEED)
	{
		jumpPow_ = MAX_FALL_SPEED;
	}
}

void WeaponBase::Collision(void)
{
	// 移動量の加算
	transform_.pos = VAdd(transform_.pos, movePow_);

	// 床判定
	CollisionGravity();

	// ジャンプ（着地・落下）移動量を座標へ反映
	transform_.pos.y += jumpPow_;
}

void WeaponBase::CollisionGravity(void)
{
	bool isHitStage = CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE);

	// 床に触れていて、かつ下方向に落下している（または静止している）なら着地
	if (isHitStage && jumpPow_ <= 0.0f)
	{
		isJump_ = false;
		jumpPow_ = 0.0f; // 落下速度をリセット
	}
}

void WeaponBase::ChangeState(STATE _state)
{
}

void WeaponBase::ChangeStateAttackWindup(void)
{
}

void WeaponBase::ChangeState(int _state)
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

void WeaponBase::UpdateAttackWindup(void)
{
}
