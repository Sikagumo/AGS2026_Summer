#include "Camera.h"
#include <DxLib.h>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include "../Utility/UtilityMath.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/System/TimeManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Object/Collision/CollisionController.h"
#include "../Object/Common/Transform.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Object/Collider/ColliderModel.h"
#include "../Object/Collider/ColliderSphere.h"
#include "../Application.h"


// カメラの初期座標
static constexpr VECTOR DERFAULT_POS = { 0.0f, 200.0f, -500.0f };

// カメラの初期角度
static constexpr VECTOR DERFAULT_ANGLES = { 0.0f, 0.0f, 0.0f };

// ロックオン切替イージング終了時間
static constexpr float LOCKON_DURATION = 0.2f;

// カメラの回転量
const float ROT_POW_KEY = UtilityMath::Deg2RadF(5.0f);
const float ROT_POW_RAD = UtilityMath::Deg2RadF(2.5f);
const float ROT_POW_MOUSE = UtilityMath::Deg2RadF(0.05f);

// 追従対象の最大Y軸座標
constexpr float TARGET_POS_MAX_Y = 275.0f;

// 追従有効範囲
constexpr float POS_SPACE_MIN = 300.0f;
constexpr float POS_SPACE_MAX = 1750.0f;

Camera::Camera(void)
	: ActorBase::ActorBase()
	, followTransform_(nullptr)
	, mode_(MODE::NONE)
	, angles_(UtilityMath::VECTOR_ZERO)
	, rotY_(Quaternion::Identity())
	, targetPos_(UtilityMath::VECTOR_ZERO)
	, prePos_(UtilityMath::VECTOR_ZERO)
	, isLockOn_(false)
	, lockOnParam_()
	, lockOnTarget_(LOCKON_TARGET::NONE)
	, followDistScale_(1.0f)
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
	ColliderSphere* colliderSphere = new ColliderSphere
	(ColliderBase::TAG::CAMERA, &transform_, UtilityMath::VECTOR_ZERO,
		COL_CAPSULE_SPHERE);
	ownColliders_[static_cast<int>(ColliderBase::TAG::CAMERA)].push_back(colliderSphere);

	colliderSphere->SetTriger(false);

	CollisionController::GetInstance().RegisterActor(this);
		
}

void Camera::InitPost(void)
{
	ChangeMode(MODE::NONE);

	lockOnTarget_ = LOCKON_TARGET::NONE;
}

void Camera::Update(void)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_E)
		|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_STICK))
	{
		if (!isLockOn_)
		{
			// ロックオン有効化
			LockOnChoice();
		}
		else
		{
			SetIsLockOn(false);
		}
	}

	// ロックオン時、常に追従位置を取得する
	if (isLockOn_)
	{
		if (InputManager::GetInstance().IsTrgMouseRight()
			|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_BUTTON)
			|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_BUTTON))
		{
			LockOnChoice();
		}

		FollowLockOnPosition();
	}

	// ターゲット対象切替時のイージング進行
	if (easingTerm_ < 1.0f)
	{
		easingTerm_ += TimeManager::GetInstance().GetDeltaTime() / LOCKON_DURATION;
		easingTerm_ = std::clamp(easingTerm_, 0.0f, 1.0f);
	}

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
}

void Camera::SetBeforeDraw(void)
{

	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos, 
		targetPos_, 
		transform_.GetUp()
	);
#ifdef _DEBUG
	//DrawSphere3D(targetPos_, 1.0f, 16, 0xffffff, 0xffffff, true);
#endif


	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::DrawDebug(void)
{
#ifdef _DEBUG

	if (followTransform_ == nullptr) { return; }
	VECTOR target = VSub(lockOnParam_.pos, VGet(followTransform_->pos.x, lockOnParam_.pos.y, followTransform_->pos.z));
	float tan = atan2f(target.x, target.z);
	VECTOR rotY = rotY_.ToEuler();

	DrawFormatString(0, 32, 0xffffff, "Camera(pos(%.1f, %.1f, %.1f), \nangle(%.1f°, %.1f°, %.1f°)),rotY(%.1f°, %.1f°, %.1f°))"
		, transform_.pos.x, transform_.pos.y, transform_.pos.z
		, UtilityMath::Rad2DegF(angles_.x), UtilityMath::Rad2DegF(angles_.y), UtilityMath::Rad2DegF(angles_.z)
		, UtilityMath::Rad2DegF(rotY.x), UtilityMath::Rad2DegF(rotY.y), UtilityMath::Rad2DegF(rotY.z));

	std::string targetText = "追従対象：";
	if (isLockOn_)
	{
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_BODY)
			? "ボス胴体" : "");
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_WEAPON_MGL_L)
			? "左マシンガン" : "");
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_WEAPON_MGL_R)
			? "右マシンガン" : "");
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_WEAPON_CANNON_L)
			? "左大砲" : "");
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_WEAPON_CANNON_R)
			? "右大砲" : "");
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_WEAPON_MP_L)
			? "左ミサイル" : "");
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_WEAPON_MP_R)
			? "右ミサイル" : "");
		targetText += ((lockOnTarget_ == LOCKON_TARGET::BOSS_WEAPON_RG)
			? "レーザー" : "");
	}
	else if ((lockOnTarget_ == LOCKON_TARGET::NONE))
	{
		targetText += "なし";
	}
	DrawString(Application::SCREEN_SIZE_X - 200, 0, targetText.c_str(), 0xff0000);
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

void Camera::SetLockOnTargets(LOCKON_TARGET _target, const VECTOR& _targetPos, int _targetHp)
{
	if (!targetsParam_.empty())
	{
		if (_targetHp <= 0)
		{
			// 追従対象が無効時、追従リストから除外させる
			if (targetsParam_.contains(_target))
			{
				targetsParam_.erase(_target);
			}

			// ターゲット無効時、再抽選
			if (lockOnTarget_ == _target)
			{
				LockOnChoice();
			}
			return;
		}

		if (targetsParam_.contains(_target))
		{
			// 追従リストに追従対象がある場合、対象にする
			targetsParam_.at(_target)->pos = _targetPos;
			targetsParam_.at(_target)->hp = _targetHp;
			return;
		}
	}

	if (_targetHp <= 0) { return; }

	// 対象をリストに追加
	std::unique_ptr<TargetParam> param = std::make_unique<TargetParam>();
	param->hp = param->maxHp = _targetHp;
	param->pos = _targetPos;
	targetsParam_.emplace(_target, std::move(param));
}

void Camera::LockOnChoice(void)
{
	/* ロックオン対象選択処理 */

	float vecSize = POS_SPACE_MAX;
	LOCKON_TARGET lockTarget = LOCKON_TARGET::NONE;
	
	if (targetsParam_.empty()) { return; }

	if (!isLockOn_)
	{
		for (auto& [target, param] : targetsParam_)
		{
			if (lockOnTarget_ == target) { continue; }

			VECTOR posFollow = VGet(param->pos.x, 0.0f, param->pos.z);
			VECTOR posCamera = VGet(transform_.pos.x, 0.0f, transform_.pos.z);

			// 最も近い対象範囲内にいる対象を追尾
			float sizeXZ = std::abs(VSize(VSub(posFollow, posCamera)));

			if (sizeXZ < vecSize
				&& sizeXZ <= POS_SPACE_MAX && sizeXZ >= POS_SPACE_MIN)
			{
				vecSize = sizeXZ;
				lockTarget = target;
			}
		}
	}
	else
	{
		int moveCount = 0;

		// マウスホイールの回転量
		moveCount += InputManager::GetInstance().GetMouseWheel();

		// パッド
		moveCount -= InputManager::GetInstance().IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::L_BUTTON);

		moveCount += InputManager::GetInstance().IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::R_BUTTON);

		if (moveCount != 0)
		{
			int target = static_cast<int>(lockOnTarget_);
			constexpr int TARGET_MAX = static_cast<int>(LOCKON_TARGET::MAX);

			int step = (moveCount > 0) ? 1 : -1;

			for (int i = 0; i < std::abs(moveCount); ++i)
			{
				do
				{
					target += step;

					if (target < 0)
					{
						target = TARGET_MAX - 1;
					}
					else if (target >= TARGET_MAX)
					{
						target = 0;
					}

				} while (!targetsParam_.contains(static_cast<LOCKON_TARGET>(target)));
			}

			lockTarget = static_cast<LOCKON_TARGET>(target);
		}
	}


	if (lockTarget == LOCKON_TARGET::NONE)
	{
		return;
	}

	// ロックオン切り替え時にイージング開始点を記録
	easingFromPos_ = transform_.pos;
	easingFromTarget_ = targetPos_;
	easingTerm_ = 0.0f;

	isLockOn_ = true;
	lockOnTarget_ = lockTarget;
	lockOnParam_ = *targetsParam_.at(lockOnTarget_);
}

void Camera::FollowLockOnPosition(void)
{
	if (targetsParam_.empty()) { return; }

	lockOnParam_ = *targetsParam_.at(lockOnTarget_);
}

void Camera::SetIsLockOn(bool _isLockOn)
{
	// ロックオン解除時、アングルとY軸回転を同期
	if (isLockOn_ && !_isLockOn)
	{
		SyncAngleYFromRotY();
	}

	// 切り替え時、現在の位置と注視点をイージング開始点として記録
	if (isLockOn_ != _isLockOn)
	{
		easingFromPos_ = transform_.pos;
		easingFromTarget_ = targetPos_;
		easingTerm_ = 0.0f;
	}

	isLockOn_ = _isLockOn;
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
		target.y = lockOnParam_.pos.y;
		VECTOR toTarget = VSub(lockOnParam_.pos, target);
		

		// XZ平面に投影（Y成分を無視）してY軸回転角を求める
		float yAngle = atan2(toTarget.x, toTarget.z);

		rotY_ = Quaternion::AngleAxis(yAngle, UtilityMath::AXIS_Y);
	}
	
	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, UtilityMath::AXIS_X));

	
	// 注視点
	VECTOR localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	VECTOR newTarget = VAdd(pos, localPos);


	if (isLockOn_)
	{
		VECTOR lockOn = VAdd(pos, VSub(lockOnParam_.pos, pos));
		lockOn.y = std::clamp(lockOn.y, -TARGET_POS_MAX_Y, TARGET_POS_MAX_Y);
		newTarget = lockOn;
	}

	// カメラ位置
	const VECTOR LOCAL_POS = ((isLockOn_) ? FOLLOW_LOCAL_POS_LOCKON : FOLLOW_LOCAL_POS);
	VECTOR scaledLocalPos = VScale(LOCAL_POS, followDistScale_);
	localPos = transform_.quaRot.PosAxis(scaledLocalPos);
	VECTOR newCamPos = VAdd(pos, localPos);

    // イージング中は EasingChangeTarget() で補間、完了後はそのまま代入
    if (easingTerm_ < 1.0f)
    {
        transform_.pos = EasingChangeTarget(easingFromPos_,    newCamPos, easingTerm_);
        targetPos_     = EasingChangeTarget(easingFromTarget_, newTarget, easingTerm_);
    }
    else
    {
        transform_.pos = newCamPos;
        targetPos_     = newTarget;
    }
}

void Camera::SyncAngleYFromRotY(void)
{
	// rotY_が示す前方ベクトルをXZ平面上で求める
	VECTOR forward = rotY_.PosAxis(UtilityMath::DIR_FORWARD);

	// atan2でY軸角度を逆算してangles_.yに書き戻す
	angles_.y = atan2f(forward.x, forward.z);

	SyncFollow();
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
	//Collision();

	// 地面下に移動制限を掛ける
	constexpr float FOLLOW_POS_MIN_Y = -5.0f;
	if (transform_.pos.y < FOLLOW_POS_MIN_Y)
	{
		transform_.pos.y = FOLLOW_POS_MIN_Y;
	}
}

void Camera::ProcessMove(void)
{
	// カメラの移動スピード
	constexpr float CAMERA_MOVE_SPEED = 50.0f;
	InputManager& inputManager = InputManager::GetInstance();
	VECTOR moveDir = UtilityMath::VECTOR_ZERO;


	if (GetJoypadNum() != 0)
	{
		InputManager::JOYPAD_IN_STATE padState = inputManager.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// 左スティックの傾き
		moveDir = inputManager.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
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
	InputManager& inputManager = InputManager::GetInstance();

	// カメラ回転
	if (inputManager.IsNew(KEY_INPUT_RIGHT))
	{
		// 右回転
		//angles_.y += ROT_POW_RAD;
	}
	if (inputManager.IsNew(KEY_INPUT_LEFT))
	{
		// 左回転
		//angles_.y -= ROT_POW_RAD;
	}

	// 上回転
	if (inputManager.IsNew(KEY_INPUT_UP))
	{
		//angles_.x += ROT_POW_RAD;
		if (_isLimit && angles_.x > LIMIT_X_UP)
		{
			//angles_.x = LIMIT_X_UP;
		}
	}

	// 下回転
	if (inputManager.IsNew(KEY_INPUT_DOWN))
	{
		//angles_.x -= ROT_POW_RAD;
		if (_isLimit && angles_.x < -LIMIT_X_DOWN)
		{
			//angles_.x = -LIMIT_X_DOWN;
		}
	}
}
void Camera::RotationMouse(bool _isLimit)
{
	// マウス感度倍率
	constexpr float ROT_SENS = (1.0 - 0.0f);
	constexpr float MOUSE_MOVE_THRESHOLD = 0.0f;

	// マウス移動量
	Vector2F mouseMove = InputManager::GetInstance().GetMouseVelocityAndFixCenter();

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
	InputManager& inputManager = InputManager::GetInstance();
	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState = inputManager.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	VECTOR dir = inputManager.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);


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
	const auto& collisionController = CollisionController::GetInstance();

	using TAG = ColliderBase::TAG;

	followDistScale_ = std::clamp(followDistScale_, FOLLOW_DIST_MIN_SCALE, 1.0f);

	// ステージに当たっているかチェック
	if (collisionController.IsTagCollidingWithTag(TAG::CAMERA, TAG::STAGE))
	{
		VECTOR hitPosition = collisionController.IsActorHitPosWithTag(this, TAG::STAGE);

		if (VSize(hitPosition) > 0.0f)
		{
			if (transform_.pos.y < prePos_.y)
			{
				transform_.pos.y = prePos_.y;
			}
		}
	}
}

void Camera::ResolveCollision(void)
{
	if (mode_ != MODE::FOLLOW)
	{
		return;
	}

	// 衝突判定
	Collision();

	// 地面下に移動制限を掛ける
	constexpr float FOLLOW_POS_MIN_Y = -5.0f;
	if (transform_.pos.y < FOLLOW_POS_MIN_Y)
	{
		transform_.pos.y = FOLLOW_POS_MIN_Y;
	}
}

VECTOR Camera::EasingChangeTarget(const VECTOR& _fromVec, const VECTOR& _toVec, float _term)
{
	 /* QuadInOut イージング
	   t < 0.5 : 加速フェーズ (2t^2)
	   t >= 0.5: 減速フェーズ (1 - (-2t+2)^2 / 2)
	*/
	_term = std::clamp(_term, 0.0f, 1.0f);

	// 開始イージング
	const float QUAD_IN = (2.0f * _term * _term);

	// 終了イージング
	const float QUAD_OUT = (1.0f - (-2.0f * _term + 2.0f) * (-2.0f * _term + 2.0f) / 2.0f);

	// イージング位置が中間を超えたらイージング方法を切り替える
	const float MIDDLE = 0.5f;
	const float easedT = ((_term < MIDDLE) ? QUAD_IN : QUAD_OUT);

	return VGet(_fromVec.x + (_toVec.x - _fromVec.x) * easedT,
				_fromVec.y + (_toVec.y - _fromVec.y) * easedT,
				_fromVec.z + (_toVec.z - _fromVec.z) * easedT);
}
