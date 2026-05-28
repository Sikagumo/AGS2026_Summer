#include "Camera.h"
#include <DxLib.h>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include "../Utility/UtilityMath.h"
#include "../Manager/Generic/InputManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Object/Collider/ColliderModel.h"
#include "../Object/Collider/ColliderSphere.h"


// カメラの初期座標
static constexpr VECTOR DERFAULT_POS = { 0.0f, 200.0f, -500.0f };

// カメラの初期角度
static constexpr VECTOR DERFAULT_ANGLES = { 0.0f, 0.0f, 0.0f };

// カメラの回転量
const float ROT_POW_KEY = UtilityMath::Deg2RadF(5.0f);
const float ROT_POW_RAD = UtilityMath::Deg2RadF(2.5f);
const float ROT_POW_MOUSE = UtilityMath::Deg2RadF(0.05f);


Camera::Camera(void)
	: ActorBase::ActorBase()
	, inputManager_(InputManager::GetInstance())
	, followTransform_(nullptr)
	, mode_(MODE::NONE)
	, angles_(UtilityMath::VECTOR_ZERO)
	, rotY_(Quaternion::Identity())
	, targetPos_(UtilityMath::VECTOR_ZERO)
	, prePos_(UtilityMath::VECTOR_ZERO)
	, isLockOn_(false)
	, fovRate_(1.0f)
	, lockOnPos_(UtilityMath::VECTOR_ZERO)
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
														UtilityMath::VECTOR_ZERO,
														COL_CAPSULE_SPHERE
														);
	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::SPHERE), colliderSphere);


}

void Camera::InitPost(void)
{
	ChangeMode(MODE::NONE);
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
#ifdef _DEBUG
	DrawSphere3D(targetPos_, 1.0f, 16, 0xffffff, 0xffffff, true);
#endif

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::DrawDebug(void)
{
#ifdef _DEBUG

	if (followTransform_ == nullptr) { return; }
	VECTOR target = VSub(lockOnPos_, VGet(followTransform_->pos.x, lockOnPos_.y, followTransform_->pos.z));
	float tan = atan2f(target.x, target.z);

	DrawFormatString(0, 32, 0xffffff, "\nCamera(pos(%.1f, %.1f, %.1f), angle(%.1f, %.1f, %.1f))\n"
		, transform_.pos.x, transform_.pos.y, transform_.pos.z
		, target.x, target.y, target.z);
#endif
}

VECTOR Camera::GetForward(void) const
{
	return UtilityMath::VNormalize((VSub(targetPos_, transform_.pos)));
}

void Camera::ChangeMode(MODE _mode)
{

	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = _mode;

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

void Camera::SetLockOnPosition(const VECTOR& _pos)
{
	isLockOn_ = true;

	VECTOR posFollow = VGet(_pos.x, 0.0f, _pos.z);
	VECTOR posCamera = VGet(transform_.pos.x, 0.0f, transform_.pos.z);

	float sizeXZ = std::abs(VSize(VSub(posFollow, posCamera)));
	constexpr float POS_SPACE_MIN = 200.0f;
	constexpr float POS_SPACE_MAX = 1350.0f;
	if (sizeXZ > POS_SPACE_MAX || sizeXZ < POS_SPACE_MIN)
	{
		isLockOn_ = false;
		return;
	}

	lockOnPos_ = _pos;

	constexpr float POS_MAX_Y = 250.0f;
	lockOnPos_.y = std::clamp(lockOnPos_.y, -POS_MAX_Y, POS_MAX_Y);
}

void Camera::SetDefault(void)
{

	// カメラの初期設定
	transform_.pos = DERFAULT_POS;

	// カメラ角
	angles_ = DERFAULT_ANGLES;
	transform_.quaRot = Quaternion::Identity();

	// 注視点
	targetPos_ = UtilityMath::VECTOR_ZERO;
}

void Camera::SyncFollow(void)
{
	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, UtilityMath::AXIS_Y);

	if (isLockOn_) 
	{
		// 追従位置 = ロックオン位置 - 追従座標
		VECTOR target = followTransform_->pos;
		target.y = lockOnPos_.y;
		VECTOR toTarget = VSub(lockOnPos_, target);
		

		// XZ平面に投影（Y成分を無視）してY軸回転角を求める
		float yAngle = atan2(toTarget.x, toTarget.z);

		rotY_ = Quaternion::AngleAxis(yAngle, UtilityMath::AXIS_Y);
	}
	
	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, UtilityMath::AXIS_X));

	
	// 注視点
	VECTOR localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	targetPos_ = VAdd(pos, localPos);

	if (isLockOn_)
	{
		targetPos_ = VAdd(pos, VSub(lockOnPos_, pos));
	}

	// カメラ位置
	const VECTOR LOCAL_POS = ((isLockOn_) ? FOLLOW_LOCAL_POS_LOCKON : FOLLOW_LOCAL_POS);
	localPos = transform_.quaRot.PosAxis(LOCAL_POS);
	transform_.pos = VAdd(pos, localPos);
}

void Camera::ProcessRot(bool _isLimit)
{

#ifdef _DEBUG
	// 方向回転によるXYZの移動(キーボード)
	RotationKeyboard(_isLimit);
#endif

	RotationMouse(_isLimit);

	// 方向回転によるXYZの移動(ゲームパッド)
	RotationGamePad(_isLimit);
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
	rotY_ = Quaternion::AngleAxis(angles_.y, UtilityMath::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, UtilityMath::AXIS_X));

	// 注視点更新
	targetPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
}

void Camera::SetBeforeDrawFollow(void)
{
	// カメラ位置の補間
	transform_.pos = UtilityMath::Lerp(prePos_, transform_.pos, LERP_RATE_MOVE);

	// カメラ操作(回転)
	if (!isLockOn_)
	{
		ProcessRot(true);
	}

	// 追従対象との相対位置を同期
	SyncFollow();

	// 衝突判定
	Collision();
}

void Camera::ProcessMove(void)
{
	// カメラの移動スピード
	constexpr float CAMERA_MOVE_SPEED = 50.0f;

	VECTOR moveDir = UtilityMath::VECTOR_ZERO;


	if (GetJoypadNum() == 0)
	{
		if (inputManager_.IsNew(KEY_INPUT_UP)) { moveDir = UtilityMath::DIR_F; }
		if (inputManager_.IsNew(KEY_INPUT_DOWN)) { moveDir = UtilityMath::DIR_B; }
		if (inputManager_.IsNew(KEY_INPUT_LEFT)) { moveDir = UtilityMath::DIR_L; }
		if (inputManager_.IsNew(KEY_INPUT_RIGHT)) { moveDir = UtilityMath::DIR_R; }
	}
	else
	{
		InputManager::JOYPAD_IN_STATE padState = inputManager_.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// 左スティックの傾き
		moveDir = inputManager_.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	}

	// 移動処理
	if (!UtilityMath::EqualsVZero(moveDir))
	{
		// 現在の向きからの進行方向を取得
		VECTOR direction = UtilityMath::VNormalize((transform_.quaRot.PosAxis(moveDir)));

		// 移動させたい方向に移動量をかける(=移動量)
		VECTOR movePow = VScale(direction, CAMERA_MOVE_SPEED);

		// カメラ位置も注視点も移動させる
		transform_.pos = VAdd(transform_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);
	}
}


void Camera::RotationKeyboard(bool _isLimit)
{
	// カメラ回転
	if (inputManager_.IsNew(KEY_INPUT_RIGHT))
	{
		// 右回転
		angles_.y += ROT_POW_RAD;
	}
	if (inputManager_.IsNew(KEY_INPUT_LEFT))
	{
		// 左回転
		angles_.y -= ROT_POW_RAD;
	}

	// 上回転
	if (inputManager_.IsNew(KEY_INPUT_UP))
	{
		angles_.x += ROT_POW_RAD;
		if (_isLimit && angles_.x > LIMIT_X_UP)
		{
			angles_.x = LIMIT_X_UP;
		}
	}

	// 下回転
	if (inputManager_.IsNew(KEY_INPUT_DOWN))
	{
		angles_.x -= ROT_POW_RAD;
		if (_isLimit && angles_.x < -LIMIT_X_DOWN)
		{
			angles_.x = -LIMIT_X_DOWN;
		}
	}
}
void Camera::RotationMouse(bool _isLimit)
{
	// マウス感度倍率
	constexpr float ROT_SENS = (1.0 - 0.0f);
	constexpr float MOUSE_MOVE_THRESHOLD = 0.0f;

	// マウス移動量
	Vector2F mouseMove = inputManager_.GetMouseVelocityAndFixCenter();

	// マウス移動量がしきい値未満の場合０にする
	mouseMove.x = ((std::abs(mouseMove.x) > MOUSE_MOVE_THRESHOLD) ? mouseMove.x : 0.0f);
	mouseMove.y = ((std::abs(mouseMove.y) > MOUSE_MOVE_THRESHOLD) ? mouseMove.y : 0.0f);

	// 感度倍率を掛ける
	mouseMove *= ROT_SENS;

	if (!UtilityMath::EqualsVZero(mouseMove))
	{
		angles_.x += (mouseMove.y * ROT_POW_MOUSE);
		angles_.y += (mouseMove.x * ROT_POW_MOUSE);

		angles_.x = std::clamp(angles_.x, -LIMIT_X_DOWN, LIMIT_X_UP);
	}
}
void Camera::RotationGamePad(bool _isLimit)
{
	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState = inputManager_.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	VECTOR dir = inputManager_.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);


	if (!UtilityMath::EqualsVZero(dir))
	{
		// 右スティック左右の傾き
		angles_.y += (dir.x * ROT_POW_RAD);

		// 右スティック上下の傾き
		angles_.x += (dir.z * ROT_POW_RAD);

		if (_isLimit)
		{
			// 角度制限
			angles_.x = std::clamp(angles_.x, -LIMIT_X_DOWN, LIMIT_X_UP);
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
		if (hitCol->GetShapeType() != ColliderBase::SHAPE::MODEL) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel = dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		// 線分で衝突判定
		//auto hitPoly = colliderModel->GetNearestHitPolyLine(transform_.pos, start, true);
		
		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollowTarget()->modelId,
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
			if (colliderModel->IsExcludedFrame(hit.FrameIndex)) { continue; }
			// 
			// 対象フレームは無視する
			if (!colliderModel->IsExcludedFrame(hit.FrameIndex)) { continue; }


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
		VECTOR dirToTarget = UtilityMath::VNormalize(VSub(targetPos_, transform_.pos));

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
		//transform_.pos = colliderSphere->GetPosPushBackAlongNormal(hitPoly, CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS);
	}
}