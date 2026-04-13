#include "Camera.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Object/Collider/ColliderModel.h"
#include "../Object/Collider/ColliderSphere.h"


Camera::Camera(void)
	: ActorBase::ActorBase(),
	followTransform_(nullptr),
	mode_(MODE::NONE),
	angles_(AsoUtility::VECTOR_ZERO),
	rotY_(Quaternion::Identity()),
	targetPos_(AsoUtility::VECTOR_ZERO)
{
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
}

void Camera::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere = new ColliderSphere(ColliderBase::TAG::CAMERA,
														&transform_,
														AsoUtility::VECTOR_ZERO,
														COL_CAPSULE_SPHERE
														);
	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::SPHERE), colliderSphere);


}

void Camera::InitPost(void)
{
	ChangeMode(MODE::FOLLOW);
}

void Camera::Update(void)
{

}

void Camera::SetBeforeDraw(void)
{

	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// 更新前情報
	prePos_ = transform_.pos;

	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	}

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos, 
		targetPos_, 
		transform_.GetUp()
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::DrawDebug(void)
{
}

void Camera::Release(void)
{
}

void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

const VECTOR& Camera::GetPos(void) const
{
	return transform_.pos;
}

const Quaternion& Camera::GetQuaRot(void) const
{
	return transform_.quaRot;
}

const Quaternion& Camera::GetQuaRotY(void) const
{
	return rotY_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, transform_.pos));
}

void Camera::ChangeMode(MODE mode)
{

	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::FOLLOW:
		break;
	}

}

void Camera::SetDefault(void)
{

	// カメラの初期設定
	transform_.pos = DERFAULT_POS;

	// カメラ角
	angles_ = DERFAULT_ANGLES;
	transform_.quaRot = Quaternion::Identity();

	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;
}

void Camera::SyncFollow(void)
{

	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	// 注視点
	localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	targetPos_ = VAdd(pos, localPos);

	// カメラ位置
	localPos = transform_.quaRot.PosAxis(FOLLOW_CAMERA_LOCAL_POS);
	transform_.pos = VAdd(pos, localPos);
}

void Camera::ProcessRot(bool isLimit)
{

	if (GetJoypadNum() == 0)
	{
		// 方向回転によるXYZの移動(キーボード)
		RotKeyboard(isLimit);
	}
	else
	{
		// 方向回転によるXYZの移動(ゲームパッド)
		RotGamePad(isLimit);
	}

}

void Camera::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	if (GetJoypadNum() == 0)
	{
		if (ins.KeyIsNew(KEY_INPUT_W)) { moveDir = AsoUtility::DIR_F; }
		if (ins.KeyIsNew(KEY_INPUT_S)) { moveDir = AsoUtility::DIR_B; }
		if (ins.KeyIsNew(KEY_INPUT_A)) { moveDir = AsoUtility::DIR_L; }
		if (ins.KeyIsNew(KEY_INPUT_D)) { moveDir = AsoUtility::DIR_R; }
	}
	else
	{
		// 左スティックの傾き
		moveDir = ins.GetAlgKeyDirXZ(InputManager::PAD_NO::PAD1, InputManager::JOYPAD_ALGKEY::RIGHT);
	}

	// 移動処理
	if (!AsoUtility::EqualsVZero(moveDir))
	{

		// 移動させたい方向(ベクトル)に変換

		// 現在の向きからの進行方向を取得
		VECTOR direction = VNorm(transform_.quaRot.PosAxis(moveDir));

		// 移動させたい方向に移動量をかける(=移動量)
		VECTOR movePow = VScale(direction, ROT_SPEED);

		// カメラ位置も注視点も移動させる
		transform_.pos = VAdd(transform_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);

	}

}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
}

void Camera::SetBeforeDrawFree(void)
{

	// カメラ操作(回転)
	ProcessRot(false);
	
	// カメラ操作(移動)
	ProcessMove();

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	// 注視点更新
	targetPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
}

void Camera::SetBeforeDrawFollow(void)
{
	// カメラ位置の補間
	transform_.pos = AsoUtility::Lerp(prePos_,
									  transform_.pos, LERP_RATE_MOVE);

	// カメラ操作(回転)
	ProcessRot(true);

	// 追従対象との相対位置を同期
	SyncFollow();

	// 衝突判定
	Collision();
}

void Camera::RotKeyboard(bool isLimit)
{

	const auto& ins = InputManager::GetInstance();

	// カメラ回転
	if (ins.KeyIsNew(KEY_INPUT_RIGHT))
	{
		// 右回転
		angles_.y += ROT_POW_RAD;
	}
	if (ins.KeyIsNew(KEY_INPUT_LEFT))
	{
		// 左回転
		angles_.y -= ROT_POW_RAD;
	}

	// 上回転
	if (ins.KeyIsNew(KEY_INPUT_UP))
	{
		angles_.x += ROT_POW_RAD;
		if (isLimit && angles_.x > LIMIT_X_UP_RAD)
		{
			angles_.x = LIMIT_X_UP_RAD;
		}
	}

	// 下回転
	if (ins.KeyIsNew(KEY_INPUT_DOWN))
	{
		angles_.x -= ROT_POW_RAD;
		if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
		{
			angles_.x = -LIMIT_X_DW_RAD;
		}
	}

}

void Camera::RotGamePad(bool isLimit)
{

	auto& ins = InputManager::GetInstance();

	// 右スティックの傾き
	VECTOR dir = ins.GetAlgKeyDirXZ(InputManager::PAD_NO::PAD1, InputManager::JOYPAD_ALGKEY::RIGHT);

	if (!AsoUtility::EqualsVZero(dir))
	{
		// 右スティック左右の傾き
		angles_.y += dir.x * ROT_POW_RAD;

		// 右スティック上下の傾き
		angles_.x += dir.z * ROT_POW_RAD;

		// 角度制限
		if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
		{
			angles_.x = -LIMIT_X_DW_RAD;
		}
		if (isLimit && angles_.x > LIMIT_X_UP_RAD)
		{
			angles_.x = LIMIT_X_UP_RAD;
		}
	}
}

void Camera::Collision(void)
{
	// プレイヤーのルートフレーム
	VECTOR start = MV1GetFramePosition(followTransform_->modelId, 1);

	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel = dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		// 線分で衝突判定
		//auto hitPoly = colliderModel->GetNearestHitPolyLine(transform_.pos, start, true);
		
		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollow()->modelId,
			-1,
			transform_.pos,
			start
		);

		// 追従対象に一番近い衝突点を探す
		bool isCollision = false;
		MV1_COLL_RESULT_POLY hitPoly;
		double minDist = DBL_MAX;

		for (int i = 0; i < hits.HitNum; i++)
		{
			const auto& hit = hits.Dim[i];


			// 除外フレームは無視する
			if (colliderModel->IsExcludeFrame(hit.FrameIndex)) { continue; }
			// 
			// 対象フレームは無視する
			if (!colliderModel->IsTargetFrame(hit.FrameIndex)) { continue; }


			// 衝突判定
			isCollision = true;

			// 距離判定
			float dist = VSize(VSub(hit.HitPosition, transform_.pos));

			if (minDist > dist && minDist > VIEW_NEAR)
			{
				// 追従対象に一番近い衝突点を優先
				minDist = dist;
				hitPoly = hit;
			}
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);

		if (!isCollision)
		//if (hitPoly.HitFlag == 0)
		{
			// 衝突していなければ次のコライダへ
			return;
		}
		// カメラ位置から注視点への方向
		VECTOR dirToTarget = AsoUtility::VNormalize(VSub(targetPos_, transform_.pos));

		// 衝突点の少し手前にカメラを置く
		transform_.pos =
		VAdd(hitPoly.HitPosition, VScale(dirToTarget, COLLISION_BACK_DIS));


		// カメラ位置の球体コライダ
		int typeSphere = static_cast<int>(COLLIDER_TYPE::SPHERE);

		// 球体コライダが無ければ処理を抜ける
		if (ownColliders_.count(typeSphere) == 0) continue;

		// 球体コライダ情報
		ColliderSphere* colliderSphere =
			dynamic_cast<ColliderSphere*>(ownColliders_.at(typeSphere));

		if (colliderSphere == nullptr) { return; }

		// 反発処理
		transform_.pos = colliderSphere->GetPosPushBackAlongNormal(hitPoly, CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS);
	}
}