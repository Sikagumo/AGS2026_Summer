#include "Player.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Common/AnimationController.h"
#include "../../Utility/UtilityMath.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Camera/Camera.h"
#include "../../Common/Quaternion.h"


Player::Player(int _playerNo)
	: CharaBase::CharaBase()
	//: PlayerBase::PlayerBase(_playerNo)
	, inputManager_(InputManager::GetInstance())
	, sceneManager_(SceneManager::GetInstance())
	
{
}


void Player::InitLoad(void)
{
	transform_.modelId = resMng_.LoadHandleId(ResourceManager::SRC::MODEL_PLAYER);
}
void Player::InitTransform(void)
{
	transform_.InitTransform(1.0f,
		Quaternion::Identity(), Quaternion::AngleAxis(180.0f, UtilityMath::AXIS_Y),
		UtilityMath::VECTOR_ZERO);
}
void Player::InitCollider(void)
{

}
void Player::InitPost(void)
{
}


void Player::UpdateProcess(void)
{
	ProcessJump();

	// 移動操作
	ProcessMove();
}

void Player::UpdateProcessPost(void)
{
}

void Player::Release(void)
{
}

void Player::ProcessMove(void)
{
	VECTOR dir = UtilityMath::VECTOR_ZERO;

	if (GetJoypadNum() > 0)
	{
		//dir = inputManager_.GetDirectionXZAKey(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_ALGKEY::LEFT);
	}
	else
	{
		
	}
	if (inputManager_.IsNew(KEY_INPUT_W)) { dir.z += 1.0f; }
	if (inputManager_.IsNew(KEY_INPUT_S)) { dir.z += -1.0f; }
	if (inputManager_.IsNew(KEY_INPUT_A)) { dir.x += -1.0f; }
	if (inputManager_.IsNew(KEY_INPUT_D)) { dir.x += 1.0f; }

	if (!UtilityMath::EqualsVZero(dir))
	{
		//movePow_ = UtilityMath::VECTOR_ZERO;

		if (!isJump_)
		{
			//PlayAnim(ANIM_TYPE::RUN);
		}

		// カメラの方向で進行
		Quaternion cameraRot = sceneManager_.GetCamera()->GetQuaRotY();

		// 移動方向を取得
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		// 加速度に割り当て
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	else
	{
		movePow_ = UtilityMath::VECTOR_ZERO;

		if (!isJump_)
		{
			//PlayAnim(ANIM_TYPE::IDLE);
		}
	}
}

void Player::ProcessJump(void)
{
	auto& input = InputManager::GetInstance();

	bool isHitKeyNew = input.IsNew(KEY_INPUT_SPACE)
		|| input.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::RB_BOTTOM);
	if (isHitKeyNew)
	{
		// ジャンプの入力受付時間を減少
		stepJump_ += sceneMng_.GetDeltaTime();
		if (stepJump_ <= TIME_JUMP_INPUT)
		{
			// ジャンプ量の計算
			float jumpSpeed = POW_JUMP_KEEP * sceneMng_.GetDeltaTime();
			jumpPow_ = VAdd(jumpPow_, VScale(UtilityMath::DIR_UP, jumpSpeed));
		}
	}
	else
	{
		stepJump_ = 0.0f;
	}

	bool isHitKey = input.IsTrgDown(KEY_INPUT_SPACE)
		|| input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::RB_BOTTOM);
	// ジャンプ
	if (isHitKey && !isJump_)
	{
		// ジャンプ量の計算
		float jumpSpeed = (POW_JUMP_INIT * sceneMng_.GetDeltaTime());
		jumpPow_ = VScale(UtilityMath::DIR_UP, jumpSpeed);

		isJump_ = true;

		// アニメーション再生
		//PlayAnim(ANIM_TYPE::JUMP, false);
	}

	// Y軸制限
	const float LIMIT_POS_Y = -1500.0f;
	if (transform_.pos.y < LIMIT_POS_Y)
	{
		transform_.pos.y = -(LIMIT_POS_Y);
	}
}