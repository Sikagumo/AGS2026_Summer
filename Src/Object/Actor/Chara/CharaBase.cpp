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
void CharaBase::DrawShadowRound(void)
{
	/* 丸影 */

	const float PLAYER_SHADOW_HEIGHT = 700.0f;
	const float PLAYER_SHADOW_SIZE = 50.0f;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// 影を落とすモデルの数だけ繰り返し
	for (auto& col : hitColliders_)
	{
		/* 丸影の描画処理 */

		const float PLAYER_SHADOW_HEIGHT = 700.0f; // 影が届く最大の高さ
		const float PLAYER_SHADOW_SIZE = 50.0f;   // 影の基本サイズ

		// 1. 自分自身の「足元から下方向」への線分（レーザー）を作る
		VECTOR startPos = transform_.pos;
		VECTOR endPos = VAdd(transform_.pos, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f));


		int targetModelHandle = -1;


		targetModelHandle = transform_.modelId;

		if (targetModelHandle == -1) return;


		MV1_COLL_RESULT_POLY_DIM HitResDim;
		HitResDim = MV1CollCheck_Capsule(targetModelHandle, -1, startPos, endPos, PLAYER_SHADOW_SIZE);

		if (HitResDim.HitNum == 0)
		{
			// 地面が遥か彼方、あるいは何もなければ後始末をして終了
			MV1CollResultPolyDimTerminate(HitResDim);
			return;
		}

		// 4. 描画環境のセットアップ（ここは元のコードのままでOK！）
		SetUseLighting(FALSE);
		SetUseZBuffer3D(TRUE);
		SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

		VERTEX3D Vertex[3];
		VECTOR SlideVec;

		// 頂点データの初期化
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		// 5. 検出された床ポリゴンの数だけ影を描画
		MV1_COLL_RESULT_POLY* HitRes = HitResDim.Dim;
		for (int i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			// ポリゴンの座標を設定
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			// Zファイティング（床と影のチカチカ）を防ぐため、法線方向に少し浮かせる
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			// 高さに応じて影の薄さ（アルファ値）を計算
			for (int v = 0; v < 3; ++v)
			{
				Vertex[v].dif.a = 0;
				if (HitRes->Position[v].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				{
					float heightDiff = fabs(HitRes->Position[v].y - transform_.pos.y);
					Vertex[v].dif.a = static_cast<BYTE>(128 * (1.0f - heightDiff / PLAYER_SHADOW_HEIGHT));
				}
			}

			// UV座標の計算（プレイヤーの中心からの相対距離でテクスチャをマッピング）
			for (int v = 0; v < 3; ++v)
			{
				Vertex[v].u = (HitRes->Position[v].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
				Vertex[v].v = (HitRes->Position[v].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			}

			// 影ポリゴンを描画（shadowHandle_ は事前に読み込んである丸影テクスチャ）
			DrawPolygon3D(Vertex, 1, shadowHandle_, TRUE);
		}

		// 6. 後始末
		MV1CollResultPolyDimTerminate(HitResDim);

		// グラフィック設定を元に戻す
		SetUseLighting(TRUE);
		SetUseZBuffer3D(FALSE);
	}
}

void CharaBase::DrawPre(void)
{

	DrawShadowRound();

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
