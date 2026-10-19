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

void WeaponBase::SetBone(int _id, const Transform& _trans, ColliderBase::TAG _tag, const VECTOR& _playerPos)
{
	bone_.id = _id;
	bone_.transform = _trans;
	bone_.playerPos = _playerPos;
	tag_ = _tag;
}

const VECTOR WeaponBase::GetPos(void) const
{
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(localPos_);

	// 位置を加算して最終的なワールド座標にする
	return VAdd(transform_.pos, localRotPos);
}

void WeaponBase::SetDamage(int _damage)
{
	hp_ -= _damage;
}

bool WeaponBase::GetIsAlive(void)
{
	return isAlive_;
}

int WeaponBase::GetHp(void)
{
	return hp_;
}

void WeaponBase::SetHp(int _hp)
{
	hp_ = _hp;
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
	state_ = _state;
	ChangeState(static_cast<int>(state_));
}

void WeaponBase::ChangeStateAttackWindup(void)
{
}

void WeaponBase::ChangeState(int _state)
{
	stateBase_ = _state;

	// ステート遷移の関数マップから該当する初期化関数を探して実行
	auto it = stateChanges_.find(stateBase_);
	if (it != stateChanges_.end())
	{
		it->second();
	}
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
	speed_ = MOVE_SPEED;
	VECTOR movePow = VScale(moveDir_, speed_);

	// ジャンプ（空中）状態なら吹っ飛び移動を適用する
	if (isJump_)
	{
		transform_.pos = VAdd(transform_.pos, movePow);
	}
}

void WeaponBase::UpdateAttackWindup(void)
{
}
