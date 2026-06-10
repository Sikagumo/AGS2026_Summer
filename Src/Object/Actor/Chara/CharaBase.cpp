#include "CharaBase.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Manager/Generic/SceneManager.h"
#include "../../../Manager/System/TimeManager.h"
#include "../../Manager/CollisionController.h"
#include "../../Collider/ColliderBase.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderModel.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../../Camera/Camera.h"
#include "../../../Application.h"



CharaBase::CharaBase(void)
	: ActorBase::ActorBase()
	, isJump_(false), jumpPow_(0.0f)
	, moveSpeed_(0.0f)
	, prevPos_(UtilityMath::VECTOR_ZERO)
	, moveDir_(UtilityMath::VECTOR_ZERO)
	, movePow_(UtilityMath::VECTOR_ZERO)
	, knockPow_(UtilityMath::VECTOR_ZERO)
	, animation_(nullptr)
{
}


void CharaBase::InitAnimation(void)
{
	if (transform_.modelId != -1)
	{
		animation_ = std::make_unique<AnimationController>(transform_.modelId);
	}
}

void CharaBase::Update(void)
{
	// 移動前座標を更新
	prevPos_ = transform_.pos;

	// 各キャラクターごとの更新処理
	UpdateProcess();

	// 移動方向に応じた遅延回転
	DelayRotate();

	// 重力処理
	CalcGravityPow();

	// 衝突判定前準備
	CollisionReserve();

	// 衝突判定
	Collision();

	transform_.Update();

	// アニメーション再生
	if (animation_)
	{
		animation_->Update();
	}

	// 各キャラクターごとの更新後処理
	UpdateProcessPost();

}

void CharaBase::DrawDebug(void)
{
#ifdef _DEBUG
	DrawFormatString(0, (16 * 12), 0xffff00, "jumpPow(%.2f), movePow(%.2f,%.2f,%.2f)"
		, jumpPow_, movePow_.x, movePow_.y, movePow_.z);
#endif
}

void CharaBase::CalcGravityPow(void)
{
	// 重力方向
	const VECTOR DIR_GRAVITY = UtilityMath::DIR_DOWN;

	// 重力の強さ
	float gravityPow = Application::GetInstance().GetGravityPow() * timeManager_.GetDeltaTime();
	
	// 重力
	VECTOR gravity = VScale(DIR_GRAVITY, gravityPow);

	jumpPow_ += gravity.y;

	// 重力制限	
	jumpPow_ = ((jumpPow_ < MAX_FALL_SPEED) ? MAX_FALL_SPEED : jumpPow_);


	/* 吹っ飛ばしの重力加算 */
	if (!UtilityMath::EqualsVZero(knockPow_))
	{
		// 重力加速
		knockPow_.y -= Application::GRAVITY_SCALE;

		if (knockPow_.y > Application::GRAVITY)
		{
			knockPow_.y = Application::GRAVITY;
		}

		if (knockPow_.y < 0.0f)
		{
			knockPow_ = UtilityMath::VECTOR_ZERO;
		}
	}

}

void CharaBase::Collision(void)
{
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	CollisionCapsule();

	// 吹っ飛ばし量を加算
	transform_.pos = VAdd(transform_.pos, knockPow_);

	// ジャンプ量を加算
	transform_.pos.y += jumpPow_;

	// 衝突(重力)
	CollisionGravity();

}

void CharaBase::CollisionGravity(void)
{
	bool isHitStage = CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE);

	// 床に触れていて、かつ下方向に落下している（または静止している）なら着地
	if (isHitStage && jumpPow_ <= 0.0f)
	{
   		isJump_ = false;
		jumpPow_ = 0.0f; // 落下速度を止める
	}
}


void CharaBase::CollisionCapsule(void)
{
	
}

void CharaBase::DrawPre(void)
{
#ifdef _DEBUG
		// モデル向き描画
		transform_.DrawModelDir();
#endif
}

void CharaBase::DelayRotate(void)
{
	constexpr float ROT_TERM = 0.2f;

	// 移動方向から回転に変換する
	if (UtilityMath::EqualsVZero(moveDir_)) { return; }

	// 回転の補間
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot
							, Quaternion::LookRotation(moveDir_)
							, ROT_TERM);
}
