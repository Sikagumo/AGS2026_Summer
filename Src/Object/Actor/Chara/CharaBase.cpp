#include "CharaBase.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Manager/Generic/SceneManager.h"
#include "../../../Manager/System/TimeManager.h"
#include "../../Collision/CollisionController.h"
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
	, animation_(nullptr)
{
	shadowHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_SHADOW);

	const VECTOR INIT_NORM = VGet(0.0f, 1.0f, 0.0f);
	const COLOR_U8 INIT_DIFUSECOLOR = GetColorU8(255, 255, 255, 255);

	for (int i = 0; i < 4; ++i)
	{
		imageVertex_[i].norm = INIT_NORM;
		imageVertex_[i].dif = INIT_DIFUSECOLOR;
	}

	// UV座標の割り当て
	imageVertex_[LEFT_BACK].u = 0.0f; imageVertex_[LEFT_BACK].v = 1.0f;
	imageVertex_[LEFT_FORWARD].u = 0.0f; imageVertex_[LEFT_FORWARD].v = 0.0f;
	imageVertex_[RIGHT_BACK].u = 1.0f; imageVertex_[RIGHT_BACK].v = 1.0f;
	imageVertex_[RIGHT_FORWARD].u = 1.0f; imageVertex_[RIGHT_FORWARD].v = 0.0f;
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



void CharaBase::Collision(void)
{
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	CollisionCapsule();

	// 任意の値を加算
	transform_.pos = VAdd(transform_.pos, CalcAddPosition());

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
void CharaBase::DrawShadowRound(float shadowScl)
{
	
	/* 【テスト用】衝突判定を通さず強制描画 */
	const float SHADOW_SIZE =shadowScl;   // 影の基本サイズ（半径）

	// ステージの表面の高さ（デバッグ表示の -1.0f に合わせる）
	// チラつき防止で 0.5f 浮かせた値を設定
	float shadowY = SHADOW_POS_Y;

	// 影の濃さを設定
	
	
	float distance = transform_.pos.y - shadowY;
	if (distance < 0) distance = 0;
	if (distance > SHADOW_FADE_HEIGHT) distance = SHADOW_FADE_HEIGHT;
	int alpha = (int)((1.0f - (distance / SHADOW_FADE_HEIGHT)) * MAX_SHADOW_COL);

	// キャラクターの現在位置（XZ）と影の半径を基に、4つの頂点座標を更新
	imageVertex_[LEFT_BACK].pos = VGet(transform_.pos.x - SHADOW_SIZE, shadowY, transform_.pos.z - SHADOW_SIZE);
	imageVertex_[LEFT_FORWARD].pos = VGet(transform_.pos.x - SHADOW_SIZE, shadowY, transform_.pos.z + SHADOW_SIZE);
	imageVertex_[RIGHT_BACK].pos = VGet(transform_.pos.x + SHADOW_SIZE, shadowY, transform_.pos.z - SHADOW_SIZE);
	imageVertex_[RIGHT_FORWARD].pos = VGet(transform_.pos.x + SHADOW_SIZE, shadowY, transform_.pos.z + SHADOW_SIZE);

	// アルファ値を各頂点に適用
	for (int i = 0; i < 4; ++i)
	{
		imageVertex_[i].dif.a = alpha;
	}

	// 描画環境のセットアップ
	SetUseLighting(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(FALSE);
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	// インデックス配列の定義
	const int POINT_CNT = 6;
	const int TRIANGLE_CNT = 2;
	WORD index[POINT_CNT];

	index[0] = LEFT_BACK; index[1] = LEFT_FORWARD; index[2] = RIGHT_BACK;
	index[3] = RIGHT_FORWARD; index[4] = RIGHT_BACK; index[5] = LEFT_FORWARD;

	// 描画
	DrawPolygonIndexed3D(imageVertex_, 4, index, TRIANGLE_CNT, shadowHandle_, TRUE);

	// グラフィック設定の復元
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
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
