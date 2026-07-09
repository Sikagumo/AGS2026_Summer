#include "Player.h"
#include <memory>
#include <cassert>
#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../Common/AnimationController.h"

#include "../../../../Utility/UtilityMath.h"
#include "../../../../Manager/Generic/InputManager.h"
#include "../../../../Manager/Generic/SceneManager.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../../Camera/Camera.h"
#include "../../../../Common/Quaternion.h"
#include "../../../Collision/CollisionController.h"
#include "../../../Collider/ColliderBase.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderLine.h"
#include "../Weapon/Bullet/Player/PBulletBig.h"
#include "../Weapon/Bullet/Player/PBulletBomb.h"
#include "../Weapon/Bullet/Player/PBulletRecovery.h"
#include "../Weapon/Bullet/Player/PBulletNormal.h"
#include "../../../../Application.h"

namespace
{
	// 衝突判定用線分位置
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, 0.0f, 0.0f };

	// 衝突判定用カプセル位置
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 18.0f, 0.0f };

	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 10.0f;

	// ジャンプ力
	static constexpr float JUMP_POW = 5.0f;

	// 回避力
	static constexpr float DODGE_POW = 10.0f;
	constexpr float TIME_DODGE = 0.65f;
	constexpr float TIME_WAIT_DODGE = 1.75f;

	constexpr float BODY_POS_OFFSET_Y = 25.0f;

	constexpr float MOVE_SPEED = 8.5f;
	constexpr float MOVE_SPEED_SHOT = (MOVE_SPEED * 0.3f);
}


Player::Player(int _playerNo, JOB_TYPE _jobType, const VECTOR& _startPos)
	: PlayerBase::PlayerBase(_playerNo, _jobType, _startPos)
	, shadowHandle_(-1)
	, animType_(ANIM_TYPE::IDLE)
	, curAttackNum_(0)
	, throwPos_(UtilityMath::VECTOR_ZERO), throwDir_(UtilityMath::VECTOR_ZERO)
	, shotIndex_(-1)
	, isCameraRotActive_(false)
	, curTimeWaitDodge_(0.0f)
	, attackNumMax_(0)
	, knockPowXZ_(UtilityMath::VECTOR2F_ZERO)
	, dodgePowXZ_(UtilityMath::VECTOR2F_ZERO)
	, shotTerm_(0.0f)
{
	if (_jobType == JOB_TYPE::CANNON)
	{
		constexpr int BULLET_MAX = 3;
		attackNumMax_ = BULLET_MAX;
	}

	std::array< SHOT_TYPE, static_cast<int>(JOB_TYPE::MAX)>
		JOB_SHOT_TYPE
	{ SHOT_TYPE::BOMB, SHOT_TYPE::BIG, SHOT_TYPE::RAPID_FIRE, SHOT_TYPE::RECOVERY };

	shotType_ = JOB_SHOT_TYPE[static_cast<int>(jobType_)];

	moveSpeed_ = MOVE_SPEED;
}


void Player::Load(void)
{
	transform_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_HUMAN);

	//SetPlayerType(PLAYER_TYPE::BIRD);
	//SetPlayerType(playerType_);
}
void Player::SetPlayerType(PLAYER_TYPE _type)
{
	using SRC = ResourceManager::SRC;

	const std::array<SRC, static_cast<int>(PLAYER_TYPE::MAX)> MODEL_RESOURCES
		= { SRC::MODEL_PLAYER_HUMAN, SRC::MODEL_PLAYER_DOG, SRC::MODEL_PLAYER_MONKEY, SRC::MODEL_PLAYER_BIRD };

	PLAYER_TYPE pType = _type;
	if (_type == PLAYER_TYPE::MAX)
	{
		int rand = GetRand(static_cast<int>(PLAYER_TYPE::MAX));
		pType = static_cast<PLAYER_TYPE>(rand);
	}

	playerType_ = pType;


	transform_.SetModel(ResourceManager::GetInstance()
		.LoadModelDuplicate(MODEL_RESOURCES.at(static_cast<int>(playerType_))));
}

void Player::InitAnimation(void)
{
	ResourceManager& resMng = ResourceManager::GetInstance();

	animation_ = std::make_unique<AnimationController>(transform_.modelId);

	constexpr float SPEED_IDLE = 30.0f;
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::IDLE)
		, SPEED_IDLE, resMng.LoadHandleId(ResourceManager::SRC::ANIM_IDLE));

	constexpr float SPEED_RUN = 32.5f;
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::RUN)
		, SPEED_RUN, resMng.LoadHandleId(ResourceManager::SRC::ANIM_RUN));

	float throwSpeed = 0.0f;

	if (jobType_ == JOB_TYPE::CANNON)
	{
		constexpr float THROW_SPEED_BIG = 20.0f;
		throwSpeed = THROW_SPEED_BIG;
	}
	else if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		constexpr float THROW_SPEED_RAPID = 75.0f;
		 throwSpeed = THROW_SPEED_RAPID;
		//animSpeedRapid_ = throwSpeed = THROW_SPEED_RAPID_START;
	}
	else
	{
		constexpr float THROW_SPEED_BOMB = 35.0f;
		throwSpeed = THROW_SPEED_BOMB;
	}

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_LEFT)
		, throwSpeed, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_LEFT));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RIGHT)
		, throwSpeed, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RIGHT));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RUN)
		, 20.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RUN));


	constexpr float SPEED_JUMP = 50.0f;
	constexpr VECTOR LOCAL_POS_JUMP = { 0.0f, 50.0f, 0.0f };
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::JUMP)
		, SPEED_JUMP, resMng.LoadHandleId(ResourceManager::SRC::ANIM_JUMP)
		, true, LOCAL_POS_JUMP);

	constexpr float SPEED_DODGE = 50.0f;
	constexpr VECTOR LOCAL_POS_DODGE = { 0.0f, 22.5f, 0.0f };
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::DODGE)
		, SPEED_DODGE, resMng.LoadHandleId(ResourceManager::SRC::ANIM_DODGE)
		, true, LOCAL_POS_DODGE);


	animType_ = ANIM_TYPE::IDLE;
	animation_->Play(static_cast<int>(animType_));
}
void Player::InitTransform(void)
{
	constexpr float MODEL_SCALE = 0.625f;
	constexpr float LOCAL_ROT_Y = 180.0f;

	transform_.InitTransform(MODEL_SCALE
		, Quaternion::Identity()
		, Quaternion::AngleAxis(UtilityMath::Deg2RadF(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
		, UtilityMath::VECTOR_ZERO);

	transform_.Update();
}
void Player::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::PLAYER, &transform_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCap = new ColliderCapsule(ColliderBase::TAG::PLAYER
									, &transform_, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);

	ownColliders_.emplace(0, colCap);

	ownColliders_.at(0)->SetTriger(false);

	// 衝突判定マネージャに登録
	CollisionController::GetInstance().RegisterActor(this);
}
void Player::InitPost(void)
{
	constexpr bool IS_RAPID_FIRE = false;
	actionController_ = std::make_unique<PActionController>(animation_, IS_RAPID_FIRE);


	constexpr float SHOT_TIME_ACTIVE_INPUT = 1.725f; // 入力可能時間
	constexpr float SHOT_TIME_END = 0.25f; // 終了時間

	float timeActive = 0.0f, timeActionActive = 0.0f, timeInput = 0.0f, timeEnd = 0.0f, timeStop = 0.0f, timeStopActive = 0.0f;

	timeInput = SHOT_TIME_ACTIVE_INPUT;
	timeEnd = SHOT_TIME_END;

	curAttackNum_ = 0;

	if (jobType_ == JOB_TYPE::CANNON)
	{
		constexpr float SHOT_TIME_INCREMENT = 0.3f; // 行動間隔上昇値
		constexpr float SHOT_TIME_INC_INPUT = 0.2f; // 行動間隔上昇値

		constexpr float SHOT_TIME_ACTIVE = 2.5f; // 有効時間
		constexpr float SHOT_TIME_ACTION_ACTIVE = 1.25f; // 行動有効時間

		constexpr float SHOT_TIME_STOP = 1.85f; // 停止時間
		constexpr float SHOT_TIME_STOP_ACTIVE = 1.15f; // 停止有効化時間

		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(0, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this)
			, timeStop, timeStopActive, timeInput);


		timeActive += SHOT_TIME_INCREMENT;
		timeInput += SHOT_TIME_INC_INPUT;
		timeEnd += SHOT_TIME_INCREMENT;
		timeStop = SHOT_TIME_STOP;
		timeStopActive = SHOT_TIME_STOP_ACTIVE;

		actionController_->SetAction(1, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this)
			, timeStop, timeStopActive, timeInput);


		timeActive += (SHOT_TIME_INCREMENT * 2);
		timeInput = 0.0f;

		actionController_->SetAction(2, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this)
			, timeStop, timeStopActive, timeInput);
	}
	else if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		constexpr float SHOT_TIME_ACTIVE = 0.4f; // 有効時間
		constexpr float SHOT_TIME_ACTION_ACTIVE = 0.325f;
		timeEnd = 0.0f;
		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(0, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this));
	}
	else
	{
		constexpr float SHOT_TIME_ACTIVE = 1.0f; // 有効時間
		constexpr float SHOT_TIME_ACTION_ACTIVE = 0.75f; // 行動有効時間

		constexpr float SHOT_TIME_STOP = 0.5f; // 停止時間
		constexpr float SHOT_TIME_STOP_ACTIVE = 0.25f; // 停止有効化時間

		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(0, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this));
	}

	// ジャンプ
	timeActive = 0.2f;
	timeEnd = 0.0f;
	timeActionActive = 0.1f;
	timeEnd = SHOT_TIME_END;
	timeStop = 0.035f;
	timeStopActive = 0.175f;
	actionController_->SetAction(3, timeActive, timeActionActive, timeEnd
								 , std::bind(&Player::Jump, this)
								 , timeStop, timeStopActive);

	// 回避
	timeActive = 0.5f;
	timeActionActive = 0.1f;
	timeEnd = 0.0f;
	timeStop = 0.025f;
	timeStopActive = 0.15f;
	actionController_->SetAction(4, timeActive, timeActionActive, timeEnd
								 , std::bind(&Player::Dodge, this)
								 , timeStop, timeStopActive);
}

void Player::Draw(void)
{
	COLOR_F material = COLOR_F();

	// 発射方向描画
	DrawShotOrbit();

	if (timeInv_ > 0.0f)
	{
		constexpr COLOR_F DAMAGE_COLOR = COLOR_F(1.0f, 0.0f, 0.0f, 1.0f);
		material = DAMAGE_COLOR;
	}
	else
	{
		material = COLOR_F(1.0f, 1.0f, 1.0f, 1.0f);
	}
	MV1SetMaterialDifColor(transform_.modelId, 0, material);

	ActorBase::Draw();
}

void Player::DrawDebug(void)
{
	CharaBase::DrawDebug();


}
void Player::SetKnock(const VECTOR& _knockDirXZ, float _knockPowXZ, bool _isStan, float _knockPowY)
{
	Vector2F knockVelo = UtilityMath::VECTOR2F_ZERO;

	knockVelo.x = (_knockDirXZ.x * _knockPowXZ);
	knockVelo.y = (_knockDirXZ.z * _knockPowXZ);

	if (_isStan)
	{
		movePow_ = UtilityMath::VECTOR_ZERO;
	}

	// 吹っ飛ばし量に加算
	knockPowXZ_ = knockVelo;

	isJump_ = true;
	jumpPow_ = _knockPowY;
}

void Player::SetRespawn(void)
{
	hp_ = HP_MAX;
	transform_.pos = START_POS;

	timeInv_ = TIME_INVINCIBLE;
}


void Player::UpdateProcess(void)
{
	float delta = TimeManager::GetInstance().GetDeltaTime();

	// 無敵時間導入
	timeInv_ = ((timeInv_ > 0.0f) ? (timeInv_ - delta) : 0.0f);

	ProcessJump();

	// 移動操作
	ProcessMove();

	ProcessAttack();

	ProcessDodge();

	UpdateBullets();

	// 吹っ飛ばし処理
	ProcessKnock();
	
	// 移動位置制限
	//MoveLimit();

	// 胴体位置更新
	bodyPos_ = transform_.pos;
	bodyPos_.y += BODY_POS_OFFSET_Y;


	UpdateSound();

	if (CollisionController::GetInstance()
			.IsTagCollidingWithTag(ColliderBase::TAG::PLAYER, ColliderBase::TAG::PLAYER_RECOVERY)
		&& hp_ <= HP_MAX)
	{
		float recovery = (static_cast<float>(HP_MAX) * PBulletRecovery::RECOVERY_RATE);
		hp_ += static_cast<int>(recovery);
		hp_ = ((hp_ > HP_MAX) ? HP_MAX : hp_);
	}
}

void Player::UpdateProcessPost(void)
{
}

void Player::DrawPre(void)
{
	for (auto& bullet : bullets_)
	{
		bullet->Draw();
	}
}

void Player::DrawLate(void)
{
#ifdef _DEBUG

	// 胴体位置
	DrawSphere3D(bodyPos_, 10.0f, 10, 0x00ffff, 0xffffff, true);

	UtilityMath::DrawLineXYZ(transform_.pos, transform_.quaRot);

	actionController_->DrawDebug();

	animation_->DrawDebug();

	for (auto& collider : ownColliders_)
	{
		collider.second->Draw();
	}
#endif
}

VECTOR Player::CalcAddPosition(void)
{
	VECTOR ret = UtilityMath::VECTOR_ZERO;

	// 吹っ飛ばし量を加算
	const VECTOR knockVec = VGet(knockPowXZ_.x, 0.0f, knockPowXZ_.y);
	ret = VAdd(ret, knockVec);

	// 回避移動量を加算
	const VECTOR dodgeVec = VGet(dodgePowXZ_.x, 0.0f, dodgePowXZ_.y);
	ret = VAdd(ret, dodgeVec);

	return ret;
}

void Player::SetSoundData(VECTOR _pos, float _radius, bool _isLanging,bool _isMGFire, bool _isRoad)
{
	PlayerBase::SetSoundData(_pos, _radius, _isLanging, _isMGFire, _isRoad);
}

void Player::ReleasePost(void)
{
}


void Player::ProcessMove(void)
{
	VECTOR dir = UtilityMath::VECTOR_ZERO;

	InputManager& input = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	dir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	if (input.IsNew(KEY_INPUT_W)) { dir.z += 1.0f; }
	if (input.IsNew(KEY_INPUT_S)) { dir.z += -1.0f; }
	if (input.IsNew(KEY_INPUT_A)) { dir.x += -1.0f; }
	if (input.IsNew(KEY_INPUT_D)) { dir.x += 1.0f; }


	// ジャンプ行動有効時に行動前の時、処理終了
	if (!actionController_->IsEndActionActive()
		&& actionController_->GetCurActionNum() == 3
		||!actionController_->IsEndActionActive()
		&& actionController_->GetCurActionNum() == 4)
	{
		movePow_ = UtilityMath::VECTOR_ZERO;
		return;
	}

	// 無敵中、移動無効
	else if (timeInvDodge_ > 0.0f) { return; }

	// カメラの方向で進行
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotY();

	if (!UtilityMath::EqualsVZero(dir))
	{
		// 攻撃中は移動速度を低下
		moveSpeed_ = ((shotIndex_ != -1)
			? MOVE_SPEED_SHOT : MOVE_SPEED);

		dir = UtilityMath::VNormalize(dir);
		movePow_ = UtilityMath::VECTOR_ZERO;

		// 移動方向を取得
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);
		moveDir_.y = 0.0f;

		// 加速度に割り当て
		movePow_ = VScale(UtilityMath::VNormalize(moveDir_), moveSpeed_);
	}
	else
	{
		movePow_ = UtilityMath::VECTOR_ZERO;
	}

	if (!isJump_
		&& actionController_->GetActionState() == PActionController::PACTION_STATE::NONE)
	{
		if (!UtilityMath::EqualsVZero(dir))
		{
			PlayAnimation(ANIM_TYPE::RUN);
		}
		else
		{
			PlayAnimation(ANIM_TYPE::IDLE);
		}
	}
}
void Player::MoveLimit(void)
{
	/* 範囲外の移動制限 */
	const float RADIUS = 1750.0f;
	VECTOR limitPos = transform_.pos;
	limitPos.y = 0.0f;
	float curRange = VSize(VSub(limitPos, UtilityMath::VECTOR_ZERO));

	// 範囲外の時、ステージ内に戻す
	if (curRange > RADIUS)
	{
		const float REFLECT_POW = 10.0f;
		transform_.pos = VAdd(transform_.pos,
			VScale(VNorm(VSub(UtilityMath::VECTOR_ZERO, limitPos)),
				REFLECT_POW));
	}
}

void Player::DrawShotOrbit(void)
{
	constexpr float ORBIT_RADIUS = 1.0f;
	constexpr float ORBIT_RADIUS_UP = 0.65f;
	constexpr int SPHERE_DIV = 12;
	constexpr int ORBIT_MAX = 50;

	// 軌道の色
	constexpr COLOR_F ORBIT_COLOR = { 150, 150,150, 0.25f };
	unsigned int color = GetColor(ORBIT_COLOR.r, ORBIT_COLOR.g, ORBIT_COLOR.b);


	// 軌道の変化量
	static constexpr float ORBIT_DELTA = 0.1f;

	// 軌道の間隔
	static constexpr float ORBIT_STEP_SCALE = 10.0f;

	// 発射速度
	constexpr std::array<float, static_cast<int>(SHOT_TYPE::MAX)>
		SHOT_POW_XZ
			= { PBulletBomb::SHOT_SPEED_BOMB_XZ, PBulletBomb::SHOT_SPEED_BOMB_XZ 
				, PBulletBig::SHOT_SPEED_BIG_XZ
				, SHOT_SPEED_XZ_RAPID, SHOT_SPEED_XZ_RAPID
				, PBulletRecovery::SHOT_SPEED_RECOVERY_XZ, PBulletRecovery::SHOT_SPEED_RECOVERY_XZ
	};

	constexpr std::array<float, static_cast<int>(SHOT_TYPE::MAX)>
		SHOT_POW_Y
			= { PBulletBomb::SHOT_SPEED_BOMB_Y, PBulletBomb::SHOT_SPEED_BOMB_Y
				, PBulletBig::SHOT_SPEED_BIG_Y
				, SHOT_SPEED_Y_RAPID, SHOT_SPEED_Y_RAPID
				, PBulletRecovery::SHOT_SPEED_RECOVERY_Y, PBulletRecovery::SHOT_SPEED_RECOVERY_Y, 
	};

	float powXZ, powY = 0.0f;
	powXZ = SHOT_POW_XZ[static_cast<int>(shotType_)];
	powY = SHOT_POW_Y[static_cast<int>(shotType_)];

	
	VECTOR shotDir = UtilityMath::VECTOR_ZERO;
	shotDir = UtilityMath::VNormalize(CalcShotDir());

	// 経過時間
	float radius = ORBIT_RADIUS;

	const VECTOR SHOT_LOCAL_POS = { 0.0f, 25.0f, 0.0f};
	VECTOR viewPos = VAdd(transform_.pos, SHOT_LOCAL_POS);

	VECTOR shotPow = UtilityMath::VECTOR_ZERO;
	shotPow.x = (shotDir.x * powXZ * ORBIT_STEP_SCALE);
	shotPow.z = (shotDir.z * powXZ * ORBIT_STEP_SCALE);
	shotPow.y = (shotDir.y * powY * ORBIT_STEP_SCALE);

	for (int i = 1; i < ORBIT_MAX; ++i)
	{
		// このステップの移動量[発射速度 - 重力加算値]
		VECTOR pos = shotPow;

		if (shotType_ != SHOT_TYPE::RAPID_FIRE && shotType_ != SHOT_TYPE::CLUSTER)
		{
			pos.y -= (Application::GetInstance().GetGravityPow() * (ORBIT_DELTA * ORBIT_STEP_SCALE * i));
		}


		// 発射位置からの移動量を積算する
		viewPos = VAdd(viewPos, pos);

		if (viewPos.y < 0.0f) { break; }

		
		DrawSphere3D(viewPos, radius, SPHERE_DIV, color, color, true);

		radius += ORBIT_RADIUS_UP;
	}
}

void Player::ProcessJump(void)
{
	if (!actionController_->IsActiveAction() &&
		actionController_->GetActionState() == PActionController::PACTION_STATE::NONE)
	{
		bool isHitTrg = InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE)
					 || InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RB_BOTTOM);
		// ジャンプ
		if (isHitTrg && !isJump_)
		{
			PlayAnimation(ANIM_TYPE::JUMP, false);
			actionController_->Active(3);
		}
	}

	// Y軸制限
	const float LIMIT_POS_Y = -1500.0f;
	if (transform_.pos.y < LIMIT_POS_Y)
	{
		transform_.pos.y = -(LIMIT_POS_Y);
	}
}
void Player::Jump(void)
{
	isJump_ = true;
	jumpPow_ = JUMP_POW;
}

void Player::ProcessDodge(void)
{
	if (timeInvDodge_ > 0.0f)
	{
		timeInvDodge_ -= TimeManager::GetInstance().GetDeltaTime();
	}
	else
	{
		dodgePowXZ_ = UtilityMath::VECTOR2F_ZERO;
	}


	if (curTimeWaitDodge_ <= 0.0f
		&& !UtilityMath::EqualsVZero(moveDir_))
	{
		if (!actionController_->IsEndActionActive()
			&& actionController_->GetActionState() != PActionController::PACTION_STATE::NONE
			|| shotIndex_ != -1)
		{
			return;
		}

		if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_LSHIFT)
			|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RB_LEFT))
		{
			PlayAnimation(ANIM_TYPE::DODGE, false);
			actionController_->Active(4);
		}
	}
	else
	{
		curTimeWaitDodge_ -= TimeManager::GetInstance().GetDeltaTime();
	}
}
void Player::Dodge(void)
{
	dodgePowXZ_ = Vector2F(moveDir_.x, moveDir_.z);
	dodgePowXZ_ *= DODGE_POW;

	timeInvDodge_ = TIME_DODGE;
	curTimeWaitDodge_ = TIME_WAIT_DODGE;
}

void Player::ProcessKnock(void)
{
	/* 吹っ飛ばしの重力加算 */
	if (!UtilityMath::EqualsVZero(knockPowXZ_)
		&& jumpPow_ < 0.0f)
	{
		knockPowXZ_ = UtilityMath::VECTOR2F_ZERO;
	}

	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::HIT_WAVE))
	{
		VECTOR hitPos = CollisionController::GetInstance().IsActorHitPosWithTag(this, ColliderBase::TAG::HIT_WAVE);
		VECTOR knockDir = VSub(transform_.pos, hitPos);
		knockDir.y = KNOCK_POW_Y;
		knockDir = UtilityMath::VNormalize(knockDir);

		SetKnock(knockDir, 10.0f, false);
	}
}

void Player::ProcessAttack(void)
{
	// 行動の更新
	actionController_->Update();

	if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		if (shotTerm_ <= 0.0f)
		{
			if (InputManager::GetInstance().IsClickMouseLeft()
				|| InputManager::GetInstance().IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER))
			{
				ProcShotNormal();
				shotTerm_ = SHOT_RAPID_TERM;
			}

			if (InputManager::GetInstance().IsClickMouseLeft()
				|| InputManager::GetInstance().IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER))
			{
				ProcShotSpecial();
				shotTerm_ = SHOT_RAPID_TERM;
			}
		}
		else
		{
			shotTerm_ -= TimeManager::GetInstance().GetDeltaTime();
		}
	}
	else
	{
		if (InputManager::GetInstance().IsTrgMouseLeft()
			|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER))
		{
			ProcShotNormal();
		}

		if (InputManager::GetInstance().IsTrgMouseRight()
			|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER))
		{
			ProcShotSpecial();
		}
	}
}
void Player::ProcShotNormal(void)
{
	// コンボをするか否か
	const bool IS_COMBO = (attackNumMax_ != 0);

	// コンボができて、入力可能か否か
	const bool canCombo = (actionController_->IsActiveAction()
		&& actionController_->IsActiveInput() && IS_COMBO);

	// 攻撃可能か否か
	const bool canAttack = !actionController_->IsActiveAction();

	if (canCombo || canAttack)
	{
		// 行動回数が最大値を超えた場合、０に戻す
		if (curAttackNum_ >= attackNumMax_
			&& IS_COMBO)
		{
			curAttackNum_ = 0;
		}

		
		shotType_ = SHOT_TYPE_NORMAL[static_cast<int>(jobType_)];

		CreateBullet();

		// コンボ時、登録した攻撃コンボアクションを呼び出す
		int actionNum = ((IS_COMBO) ? curAttackNum_ : 0);
		actionController_->Active(actionNum);

		curAttackNum_++;

		// 攻撃時に左右交互に弾を投げるアニメーション
		ANIM_TYPE type = ((curAttackNum_ % 2 == 0) ? ANIM_TYPE::THROW_LEFT : ANIM_TYPE::THROW_RIGHT);
		PlayAnimation(type, false);
	}
}
void Player::ProcShotSpecial(void)
{
}

void Player::UpdateBullets(void)
{
	// 発射時の手のフレームに生成した弾を追従させる
	constexpr int FRAME_FINGER_LEFT = 23;
	constexpr int FRAME_FINGER_RIGHT = 39;
	const int FRAME_FINGER = ((curAttackNum_ % 2 == 0) ? FRAME_FINGER_LEFT : FRAME_FINGER_RIGHT);

	const int FRAME_HAND_PALM = (FRAME_FINGER - 1);
	VECTOR posFinger = MV1GetFramePosition(transform_.modelId, FRAME_FINGER);
	VECTOR posHandPalm = MV1GetFramePosition(transform_.modelId, FRAME_HAND_PALM);

	throwDir_ = UtilityMath::VNormalize(VSub(posFinger, posHandPalm));
	throwPos_ = posFinger;

	for (auto& bullet : bullets_)
	{
		bullet->Update();
	}

	if (bullets_.empty()) { return; }
	

	if (shotIndex_ != -1)
	{
		if (animation_->IsStop())
		{
			bullets_.at(shotIndex_)->PreActiveProcess();
		}

		// 弾を手に追従
		bullets_.at(shotIndex_)->SetFollow(throwPos_, throwDir_);
	}
}

void Player::DelayRotate(void)
{
	Quaternion goalRot = Quaternion::Identity();

	// ロックオン有効時、カメラ方向に回転
	isCameraRotActive_ = SceneManager::GetInstance().GetCamera()->GetIsLockOn();

	if (isCameraRotActive_)
	{
		// カメラのY軸回転を回転に変換する
		goalRot = SceneManager::GetInstance().GetCamera()->GetQuaRotY();
		goalRot.x = 0.0f;
		moveDir_ = Quaternion::PosAxis(goalRot, UtilityMath::DIR_FORWARD);
	}
	else if (!UtilityMath::EqualsVZero(moveDir_))
	{
		// 移動方向から回転に変換する
		goalRot = Quaternion::LookRotation(moveDir_);
	}


	constexpr float ROT_TERM = 0.2f;

	// 回転の補間
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, goalRot, ROT_TERM);
}



void Player::CreateBullet(void)
{
	std::unique_ptr<PBulletBase> bullet;

	shotIndex_ = 0;

	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive())
		{
			bullet->Init();

			bullet->Create(transform_.pos, throwDir_, curAttackNum_, (curAttackNum_ >= (attackNumMax_ - 1)));
			return;
		}
		shotIndex_++;
	}

	switch (shotType_)
	{
		case SHOT_TYPE::BIG:
			bullet = std::make_unique<PBulletBig>();
		break;

		case SHOT_TYPE::BOMB:
			bullet = std::make_unique<PBulletBomb>();
		break;

		case SHOT_TYPE::RECOVERY:
			bullet = std::make_unique<PBulletRecovery>();
		break;

		case SHOT_TYPE::RAPID_FIRE:
		{
			bullet = std::make_unique<PBulletNormal>
						(SCALE_RAPID, RADIUS_RAPID, POWER_RAPID
						, SHOT_SPEED_XZ_RAPID, SHOT_SPEED_Y_RAPID, ALIVE_TIME_RAPID
						, false);
		}
		break;

		default:
		break;

	}

	bullet->Load();
	bullet->Init();
	bullet->Create(throwPos_, throwDir_, curAttackNum_, (curAttackNum_ >= (attackNumMax_ - 1)));

	bullets_.emplace_back(std::move(bullet));
}

void Player::ShotBullet(void)
{
	// 発射処理の有効化
	bullets_[shotIndex_]->Shot(CalcShotDir());
	shotIndex_ = -1;
}
VECTOR Player::CalcShotDir(void)
{
	VECTOR shotDir = transform_.GetForward();

	if (shotType_ != SHOT_TYPE::RAPID_FIRE)
	{
		// 放物線状に投げる
		shotDir = VAdd(shotDir, UtilityMath::DIR_UP);
		shotDir = UtilityMath::VNormalize(shotDir);
	}
	else if (SceneManager::GetInstance().GetCamera()->GetIsLockOn())
	{
		VECTOR throwDir = UtilityMath::VNormalize(
			VSub(SceneManager::GetInstance().GetCamera()->GetLockOnPos(),
				throwPos_));

		shotDir = UtilityMath::VNormalize(VAdd(shotDir, throwDir));
	}

	return shotDir;
}

void Player::DrawShadowRound(void)
{
	/* 丸影 */
	/*
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
		// チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
		if (j == 0)
		{
			ModelHandle = stg.ModelHandle;
		}
		else
		{
			ModelHandle = stg.CollObjModelHandle[j - 1];
		}

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(col->GetFollow()->modelId, -1, transform_.pos,
										 VAdd(transform_.pos, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)),
										 PLAYER_SHADOW_SIZE);

		// 頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		// 球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (int i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			// ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			// ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			// ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = static_cast<BYTE>(128 * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT));

			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = static_cast<BYTE>(128 * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT));

			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = static_cast<BYTE>(128 * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT));

			// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			// 影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, shadowHandle_, TRUE);
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);
	*/
}

void Player::PlayAnimation(ANIM_TYPE _type, bool _isLoop, float _animSpeed)
{
	// コンボ時のみ
	if (_type != ANIM_TYPE::THROW_LEFT
		&& _type != ANIM_TYPE::THROW_RIGHT
		&& (attackNumMax_ != 0))
	{
		if (animType_ == ANIM_TYPE::THROW_LEFT
			|| animType_ == ANIM_TYPE::THROW_RIGHT)
		{
			curAttackNum_ = 0;
		}
	}

	animType_ = _type;

	animation_->Play(static_cast<int>(_type), _isLoop, _animSpeed);
}

