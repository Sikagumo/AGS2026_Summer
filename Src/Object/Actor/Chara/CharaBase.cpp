#include "CharaBase.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Manager/Generic/SceneManager.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../Collider/ColliderBase.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderModel.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../../Camera/Camera.h"
#include "../../../Application.h"



CharaBase::CharaBase(void)
	: ActorBase::ActorBase()
	, isJump_(false), jumpPow_(UtilityMath::VECTOR_ZERO), stepJump_(0.0f)
	, moveSpeed_(0.0f)
	, prevPos_(UtilityMath::VECTOR_ZERO)
	, moveDir_(UtilityMath::VECTOR_ZERO)
	, movePow_(UtilityMath::VECTOR_ZERO)
	, isDirRotActive_(true)
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

	// 重力による移動量
	//CalcGravityPow();

	// 衝突判定前準備
	CollisionReserve();

	// 衝突判定
	Collision();

	// モデル制御更新
	transform_.Update();

	// アニメーション再生
	//animation_->Update();

	// 各キャラクターごとの更新後処理
	UpdateProcessPost();

}

void CharaBase::Release(void)
{
	
}

void CharaBase::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = UtilityMath::DIR_DOWN;

	// 重力の強さ
	float gravityPow = Application::GetInstance().GetGravityPow() * sceneManager_.GetDeltaTime();

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 重力制限	
	jumpPow_.y = ((jumpPow_.y < MAX_FALL_SPEED) ? MAX_FALL_SPEED : jumpPow_.y);
}

void CharaBase::Collision(void)
{
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	CollisionCapsule();

	// ジャンプ量を加算
	transform_.pos = VAdd(transform_.pos, jumpPow_);

	// 衝突(重力)
	CollisionGravity();

}

void CharaBase::CollisionGravity(void)
{
	// 落下中しか判定しない
	if (!(VDot(UtilityMath::DIR_DOWN, jumpPow_) > 0.9f)) { return; }

	// 線分コライダ
	int lineType = static_cast<int>(COLLIDER_TYPE::LINE);

	// 線分コライダが無ければ処理を抜ける
	if (ownColliders_.count(lineType) == 0) { return; }

	// 線分コライダ情報
	ColliderLine* colliderLine_ =
		dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));

	if (colliderLine_ == nullptr) { return; }

	// 線分の始点と終点を取得
	VECTOR s = colliderLine_->GetWorldStartPos();
	VECTOR e = colliderLine_->GetWorldEndPos();

	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// ステージ以外は処理を飛ばす
		if (hitCol->GetCollisionTag() != ColliderBase::TAG::STAGE) { continue; }

		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) { continue; }

		// ステージモデル(地面)との衝突
		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollowTarget()->modelId, -1, s, e);

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			// 除外フレームは無視する
			if (colliderModel->IsExcludedFrame(hit.FrameIndex)) { continue; }

			// 衝突地点から、少し上に移動
			if (transform_.pos.y < hit.HitPosition.y)
			{
				// 衝突物より、下側にいる場合のみ、位置を修正する
				transform_.pos =
					VAdd(hit.HitPosition, VScale(UtilityMath::DIR_UP, 2.0f));
			}

			// ジャンプ判定
			isJump_ = false;
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
	if (!isJump_)
	{
		// ジャンプリセット
		jumpPow_ = UtilityMath::VECTOR_ZERO;

		// ジャンプの入力受付時間をリセット
		stepJump_ = 0.0f;
	}
}


void CharaBase::CollisionCapsule(void)
{
	//// カプセルコライダ
	//int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);

	//// カプセルコライダが無ければ処理を抜ける
	//if (ownColliders_.count(capsuleType) == 0) { return; }

	//// カプセルコライダ情報
	//ColliderCapsule * colliderCapsule = dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));

	//if (colliderCapsule == nullptr) { return; }

	//// 登録されている衝突物を全てチェック
	//for (const auto& hitCol : hitColliders_)
	//{
	//	// モデル以外はスキップ
	//	if (hitCol->GetShapeType() != ColliderBase::SHAPE::MODEL) { continue; }

	//	// 派生クラスへキャスト
	//	const ColliderModel* colliderModel = dynamic_cast<const ColliderModel*>(hitCol);

	//	if (colliderModel == nullptr) { continue; }

	//	
	//	// 衝突するオブジェクトの全てのポリゴンを判定
	//	auto hits = MV1CollCheck_Capsule(colliderModel->GetFollowTarget()->modelId, -1,
	//									 colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(),
	//									 colliderCapsule->GetRadius());
	//	
	//	// 衝突した複数のポリゴンと衝突回避するまで、
	//	// プレイヤーの位置を移動させる
	//	for (int i = 0; i < hits.HitNum; i++)
	//	{
	//		auto hit = hits.Dim[i];

	//		// 衝突したフレームが除外対象時、スキップ
	//		if (colliderModel->IsExcludeFrame(hit.FrameIndex)) { continue; }


	//		// 指定された回数と距離で三角形の法線方向に押し戻す
	//		transform_.pos = colliderCapsule->GetPosPushBackAlongNormal(hit, CNT_TRY_COLLISION, COLLISION_BACK_DIS);

	//		/*
	//		// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
	//		// この時、移動させる方向は、移動前座標に向いた方向であったり、
	//		// 衝突したポリゴンの法線方向だったりする
	//		for (int tryCnt = 0; tryCnt < CNT_TRY_COLLISION; tryCnt++)
	//		{
	//			// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
	//			// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
	//			int pHit = HitCheck_Capsule_Triangle(colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(),
	//												 colliderCapsule->GetRadius(),
	//												 hit.Position[0], hit.Position[1], hit.Position[2]);

	//			if (pHit)
	//			{
	//				// 法線の方向にちょっとだけ移動させる
	//				transform_.pos = VAdd(transform_.pos, VScale(hit.Normal, COLLISION_BACK_DIS));

	//				continue;
	//			}
	//			break;
	//		}*/
	//	}

	//	// 検出した地面ポリゴン情報の後始末
	//	MV1CollResultPolyDimTerminate(hits);
	//}
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
	Quaternion goalRot = Quaternion::Identity();
	
	if (isDirRotActive_)
	{
		// 移動方向から回転に変換する
		if (!UtilityMath::EqualsVZero(moveDir_))
		{
			goalRot = Quaternion::LookRotation(moveDir_);
		}
	}
	else
	{
		// カメラのY軸回転を回転に変換する
		goalRot = SceneManager::GetInstance().GetCamera()->GetQuaRotY();

		goalRot.x = 0.0f;
	}

	constexpr float ROT_TERM = 0.2f;
	// 回転の補間
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, goalRot, ROT_TERM);
}
