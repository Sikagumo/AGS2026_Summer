#include "Player.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"
#include "../../Utility/MatrixUtility.h"
#include "../Common/AnimationController.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderCapsule.h"


Player::Player(void)
	:CharaBase::CharaBase(),
	isDash_(false)
{
}

void Player::InitLoadPost(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER));
}

void Player::InitTransform(void)
{
	transform_.InitTransform(1.0f,
							 Quaternion::Identity(), Quaternion::AngleAxis(180.0f, AsoUtility::AXIS_Y),
							 AsoUtility::VECTOR_ZERO);
}

void Player::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::PLAYER, &transform_,
											 COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(ColliderBase::TAG::PLAYER, &transform_,
													  COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
													  COL_CAPSULE_RADIUS);

	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);


	// 当たり判定リストに格納
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);
}

void Player::InitAnimationPost(void)
{
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::IDLE), 30.0f,
					   resMng_.GetHandleId(ResourceManager::SRC::ANIM_PLAYER_IDLE));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::RUN), 30.0f,
					   resMng_.GetHandleId(ResourceManager::SRC::ANIM_PLAYER_RUN));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::FAST_RUN), 30.0f,
					   resMng_.GetHandleId(ResourceManager::SRC::ANIM_PLAYER_RUN_FAST));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::JUMP), 30.0f,
					   resMng_.GetHandleId(ResourceManager::SRC::ANIM_PLAYER_JUMPING));

	PlayAnim(ANIM_TYPE::IDLE);
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

void Player::CollisionReserve(void)
{
	/* アニメーションごとの衝突位置調整 */

	if (animation_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
	{
		// ジャンプ中は線分を伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_JUMP_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_JUMP_END_LOCAL_POS);
		}

		// ジャンプ中はカプセルを伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_JUMP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_JUMP_LOCAL_POS);
		}
	}
	else
	{
		// 通常時の線分に戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
		}

		// 通常時のカプセルに戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		}
	}
}

void Player::ProcessMove(void)
{
	InputManager& input = InputManager::GetInstance();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	bool isInputPad = (input.PadIsBtnNew(1, static_cast<int>(InputManager::PAD_BTN::R_TRIGGER)) && GetJoypadNum() > 0 ||
					   input.PadIsBtnNew(1, static_cast<int>(InputManager::PAD_BTN::L_STICK)) && GetJoypadNum() > 0);
	// ダッシュ処理
	isDash_ = (input.KeyIsNew(KEY_INPUT_LSHIFT) && GetJoypadNum() <= 0
			   || isInputPad);


	if (GetJoypadNum() > 0)
	{
		dir = input.GetAlgKeyDirXZ(InputManager::PAD_NO::PAD1, InputManager::JOYPAD_ALGKEY::LEFT);
	}
	else
	{
		if (input.KeyIsNew(KEY_INPUT_W)) { dir.z += 1.0f; }
		if (input.KeyIsNew(KEY_INPUT_S)) { dir.z += -1.0f; }
		if (input.KeyIsNew(KEY_INPUT_A)) { dir.x += -1.0f; }
		if (input.KeyIsNew(KEY_INPUT_D)) { dir.x += 1.0f; }
	}

	if (!AsoUtility::EqualsVZero(dir))
	{
		//movePow_ = AsoUtility::VECTOR_ZERO;

		// ダッシュ入力時にダッシュ加速度にする
		moveSpeed_ = ((isDash_) ? SPEED_DASH : SPEED_MOVE);

		if (!isJump_)
		{
			if (isDash_)
			{
				PlayAnim(ANIM_TYPE::FAST_RUN);
			}
			else
			{
				PlayAnim(ANIM_TYPE::RUN);
			}
		}

		// カメラの方向で進行
		Quaternion cameraRot = sceneMng_.GetCamera()->GetQuaRotY();

		// 移動方向を取得
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		// 加速度に割り当て
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	else
	{
		movePow_ = AsoUtility::VECTOR_ZERO;

		if (!isJump_)
		{
			PlayAnim(ANIM_TYPE::IDLE);
		}
	}
}

void Player::ProcessJump(void)
{
	auto& input = InputManager::GetInstance();

	bool isHitKeyNew = input.KeyIsNew(KEY_INPUT_SPACE)
		|| input.PadIsBtnNew(InputManager::PAD_NO::PAD1, 
								 InputManager::PAD_BTN::DOWN);
	if (isHitKeyNew)
	{
		// ジャンプの入力受付時間を減少
		stepJump_ += sceneMng_.GetDeltaTime();
		if (stepJump_ <= TIME_JUMP_INPUT)
		{
			// ジャンプ量の計算
			float jumpSpeed = POW_JUMP_KEEP * sceneMng_.GetDeltaTime();
			jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_UP, jumpSpeed));
		}
	}
	else
	{
		stepJump_ = 0.0f;
	}

	bool isHitKey = input.KeyIsTrgDown(KEY_INPUT_SPACE)
		|| input.PadIsBtnTrgDown(InputManager::PAD_NO::PAD1, 
								 InputManager::PAD_BTN::DOWN);
	// ジャンプ
	if (isHitKey && !isJump_)
	{
		// ジャンプ量の計算
		float jumpSpeed = (POW_JUMP_INIT * sceneMng_.GetDeltaTime());
		jumpPow_ = VScale(AsoUtility::DIR_UP, jumpSpeed);

		isJump_ = true;

		// アニメーション再生
		PlayAnim(ANIM_TYPE::JUMP, false);
	}

	// Y軸制限
	const float LIMIT_POS_Y = -1500.0f;
	if (transform_.pos.y < LIMIT_POS_Y)
	{
		transform_.pos.y = -(LIMIT_POS_Y);
	}
}

void Player::PlayAnim(Player::ANIM_TYPE _type, bool _isLoop)
{
	int type = static_cast<int>(_type);

	// 指定したアニメーションが割り当てられているとき、処理終了
	if (type == animation_->GetPlayType()) { return; }

	animation_->Play(type, _isLoop);
}