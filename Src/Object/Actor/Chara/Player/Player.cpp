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
#include "../Weapon/Bullet/Player/PBulletPoison.h"
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
	constexpr float TIME_WAIT_DODGE = 1.5f;

	constexpr float BODY_POS_OFFSET_Y = 25.0f;

	constexpr float MOVE_SPEED = 8.5f;
	constexpr float MOVE_SPEED_SHOT = (MOVE_SPEED * 0.3f);

	static constexpr float SHOT_RAPID_TERM = 0.025f;
};


Player::Player(int _playerNo, JOB_TYPE _jobType, SKIN_TYPE _skinType, const VECTOR& _startPos)
	: PlayerBase::PlayerBase(_playerNo, _jobType, _startPos, _skinType)
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
	, shotTerm_(0.0f), curTimeDefeat_(0.0f)
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
	const std::map<SKIN_TYPE, ResourceManager::SRC>
		SKIN_SRC = { { SKIN_TYPE::HYMAN, ResourceManager::SRC::MODEL_PLAYER_HUMAN}
					, { SKIN_TYPE::MONKEY, ResourceManager::SRC::MODEL_PLAYER_MONKEY}
					, { SKIN_TYPE::BIRD, ResourceManager::SRC::MODEL_PLAYER_BIRD}
					, { SKIN_TYPE::DOG, ResourceManager::SRC::MODEL_PLAYER_DOG} };

	transform_.modelId = ResourceManager::GetInstance()
		.LoadModelDuplicate(SKIN_SRC.at(playerType_));
}
void Player::SetPlayerType(SKIN_TYPE _type)
{
	using SRC = ResourceManager::SRC;

	const std::array<SRC, static_cast<int>(SKIN_TYPE::MAX)> MODEL_RESOURCES
		= { SRC::MODEL_PLAYER_HUMAN, SRC::MODEL_PLAYER_DOG, SRC::MODEL_PLAYER_MONKEY, SRC::MODEL_PLAYER_BIRD };

	SKIN_TYPE pType = _type;
	if (_type == SKIN_TYPE::MAX)
	{
		int rand = GetRand(static_cast<int>(SKIN_TYPE::MAX));
		pType = static_cast<SKIN_TYPE>(rand);
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
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_IDLE), SPEED_IDLE);

	constexpr float SPEED_RUN = 32.5f;
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::RUN)
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_RUN), SPEED_RUN);

	float throwSpeed = 0.0f;

	if (jobType_ == JOB_TYPE::CANNON)
	{
		constexpr float THROW_SPEED_BIG = 20.0f;
		throwSpeed = THROW_SPEED_BIG;
	}
	else if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		constexpr float THROW_SPEED_RAPID = 200.0f;
		 throwSpeed = THROW_SPEED_RAPID;
		//animSpeedRapid_ = throwSpeed = THROW_SPEED_RAPID_START;
	}
	else
	{
		constexpr float THROW_SPEED_BOMB = 35.0f;
		throwSpeed = THROW_SPEED_BOMB;
	}

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_LEFT)
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_LEFT), throwSpeed);

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RIGHT)
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RIGHT), throwSpeed);

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RUN)
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RUN), 20.0f);


	// ジャンプ
	constexpr float SPEED_JUMP = 50.0f;
	constexpr VECTOR LOCAL_POS_JUMP = { 0.0f, 50.0f, 0.0f };
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::JUMP)
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_JUMP)
		, LOCAL_POS_JUMP, SPEED_JUMP);

	// 回避
	constexpr float SPEED_DODGE = 50.0f;
	constexpr VECTOR LOCAL_POS_DODGE = { 0.0f, 25.0f, 0.0f };
	constexpr VECTOR LOCAL_POS_DODGE_END = { 0.0f, 50.0f, 0.0f };
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::DODGE)
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_DODGE)
		, LOCAL_POS_DODGE, LOCAL_POS_DODGE_END, SPEED_DODGE);

	// 撃墜
	constexpr float SPEED_DEFEAT = 30.0f;
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::DEFEAT)
		, resMng.LoadHandleId(ResourceManager::SRC::ANIM_DEFEAT));


	PlayAnimation(ANIM_TYPE::IDLE);
}
void Player::InitTransform(void)
{
	constexpr float MODEL_SCALE = 0.625f;
	constexpr float LOCAL_POS_Y = 5.25f;
	constexpr float LOCAL_ROT_Y = 180.0f;

	transform_.InitTransform(MODEL_SCALE
		, Quaternion::Identity()
		, Quaternion::AngleAxis(UtilityMath::Deg2RadF(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
		, UtilityMath::VECTOR_ZERO, VGet(0.0f, LOCAL_POS_Y, 0.0f));

	transform_.pos = START_POS;

	transform_.Update();
}
void Player::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::PLAYER, &transform_
								, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::PLAYER)].push_back(colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCap = new ColliderCapsule(ColliderBase::TAG::PLAYER
									, &transform_, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);

	ownColliders_[static_cast<int>(ColliderBase::TAG::PLAYER)].push_back(colCap);

	colCap->SetTriger(false);

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
	int actionNum = 0;

	// ジャンプ
	actionNum = static_cast<int>(ACTION_TYPE::JUMP);
	timeActive = 0.2f;
	timeEnd = 0.0f;
	timeActionActive = 0.1f;
	timeEnd = SHOT_TIME_END;
	timeStop = 0.035f;
	timeStopActive = 0.175f;
	actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
		, std::bind(&Player::Jump, this)
		, timeStop, timeStopActive);

	// 回避
	actionNum = static_cast<int>(ACTION_TYPE::DODGE);
	timeActive = 0.5f;
	timeActionActive = 0.1f;
	timeEnd = 0.0f;
	timeStop = 0.025f;
	timeStopActive = 0.15f;
	actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
		, std::bind(&Player::Dodge, this)
		, timeStop, timeStopActive);

	// 撃破
	actionNum = static_cast<int>(ACTION_TYPE::DEFEAT);
	timeActive = 1.0f;
	timeActionActive = 0.01f;
	timeEnd = 5.0f;
	timeStop = 2.5f;
	timeStopActive = 0.0f;
	actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
		, nullptr, timeStop, timeStopActive);

	// 攻撃処理
	timeInput = SHOT_TIME_ACTIVE_INPUT;
	timeEnd = SHOT_TIME_END;

	curAttackNum_ = 0;

	if (jobType_ == JOB_TYPE::CANNON)
	{
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		constexpr float SHOT_TIME_INCREMENT = 0.3f; // 行動間隔上昇値
		constexpr float SHOT_TIME_INC_INPUT = 0.2f; // 行動間隔上昇値

		constexpr float SHOT_TIME_ACTIVE = 2.5f; // 有効時間
		constexpr float SHOT_TIME_ACTION_ACTIVE = 1.25f; // 行動有効時間

		constexpr float SHOT_TIME_STOP = 1.85f; // 停止時間
		constexpr float SHOT_TIME_STOP_ACTIVE = 1.15f; // 停止有効化時間

		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this)
			, timeStop, timeStopActive, timeInput);

		actionNum++;
		timeActive += SHOT_TIME_INCREMENT;
		timeInput += SHOT_TIME_INC_INPUT;
		timeEnd += SHOT_TIME_INCREMENT;
		timeStop = SHOT_TIME_STOP;
		timeStopActive = SHOT_TIME_STOP_ACTIVE;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this)
			, timeStop, timeStopActive, timeInput);


		actionNum++;
		timeActive += (SHOT_TIME_INCREMENT * 2);
		timeInput = 0.0f;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this)
			, timeStop, timeStopActive, timeInput);


		actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		timeActive += (SHOT_TIME_INCREMENT * 2);
		timeInput = 0.0f;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this)
			, timeStop, timeStopActive, timeInput);
	}
	else if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		constexpr float SHOT_TIME_ACTIVE = 0.4f; // 有効時間
		constexpr float SHOT_TIME_ACTION_ACTIVE = 0.325f;
		timeEnd = 0.0f;
		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotCluster, this));


		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		timeEnd = 0.0f;
		timeActive = 0.5f;
		timeActionActive = 0.1f;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this));
	}
	else if (jobType_ == JOB_TYPE::SUPPORT)
	{
		constexpr float SHOT_TIME_ACTIVE = 1.0f; // 有効時間
		constexpr float SHOT_TIME_ACTION_ACTIVE = 0.75f; // 行動有効時間

		constexpr float SHOT_TIME_STOP = 0.5f; // 停止時間
		constexpr float SHOT_TIME_STOP_ACTIVE = 0.25f; // 停止有効化時間

		actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this));


		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this));
	}
	else
	{
		constexpr float SHOT_TIME_ACTIVE = 1.0f; // 有効時間
		constexpr float SHOT_TIME_ACTION_ACTIVE = 0.75f; // 行動有効時間

		constexpr float SHOT_TIME_STOP = 0.5f; // 停止時間
		constexpr float SHOT_TIME_STOP_ACTIVE = 0.25f; // 停止有効化時間

		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		timeActive = SHOT_TIME_ACTIVE;
		timeActionActive = SHOT_TIME_ACTION_ACTIVE;

		actionController_->SetAction(actionNum, timeActive, timeActionActive, timeEnd
			, std::bind(&Player::ShotBullet, this));
	}

	isJump_ = false;
}


void Player::UpdateProcess(void)
{
	float delta = TimeManager::GetInstance().GetDeltaTime();

	// 無敵時間導入
	timeInv_ = ((timeInv_ > 0.0f) ? (timeInv_ - delta) : 0.0f);

	// 行動の更新
	actionController_->Update();

	if (hp_ <= 0)
	{
		ProcessDefeat();
		return;
	}

	ProcessJump();

	// 移動操作
	ProcessMove();

	UpdateBullets();

	ProcessAttack();

	// 回避処理
	ProcessDodge();

	// 吹っ飛ばし処理
	ProcessKnock();
	

	// 胴体位置更新
	bodyPos_ = transform_.pos;
	bodyPos_.y += BODY_POS_OFFSET_Y;


	UpdateSound();

	// 回復処理
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

#ifdef _DEBUG
	DrawFormatString(10, 140, 0xffffff, "Playerの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
#endif
	ActorBase::Draw();

	CharaBase::DrawShadowRound(30.0f);
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
	InitPost();
	hp_ = HP_MAX;
	transform_.pos = START_POS;

	timeInv_ = TIME_INVINCIBLE;
	PlayAnimation(ANIM_TYPE::IDLE, true, false);
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

	for (auto& [id, colliderVector] : ownColliders_)
	{
		for (auto* collider : colliderVector)
		{
			if (collider == nullptr)
			{
				continue;
			}

			collider->Draw();
		}
	}
#endif
}

VECTOR Player::CalcAddPosition(void)
{
	VECTOR ret = UtilityMath::VECTOR_ZERO;

	if (hp_ <= 0)
	{
		movePow_ = UtilityMath::VECTOR_ZERO;
		dodgePowXZ_ = UtilityMath::VECTOR2F_ZERO;
		knockPowXZ_ = UtilityMath::VECTOR2F_ZERO;
		return ret;
	}

	// 吹っ飛ばし量を加算
	const VECTOR knockVec = VGet(knockPowXZ_.x, 0.0f, knockPowXZ_.y);
	ret = VAdd(ret, knockVec);

	// 回避移動量を加算
	VECTOR dodgeVec;
	if (CollisionController::GetInstance()
		.IsTagCollidingWithTag(ColliderBase::TAG::PLAYER, ColliderBase::TAG::WALL))
	{
		dodgeVec = VGet(-dodgePowXZ_.x, 0.0f, -dodgePowXZ_.y);
		ret = VAdd(ret, dodgeVec);
		dodgePowXZ_ = UtilityMath::VECTOR2F_ZERO;
	}
	else
	{
	
		dodgeVec = VGet(dodgePowXZ_.x, 0.0f, dodgePowXZ_.y);
		ret = VAdd(ret, dodgeVec);
	}

	return ret;
}

void Player::SetSoundData(VECTOR _pos, float _radius, bool _isLanging,bool _isMGFire, bool _isRoad)
{
	PlayerBase::SetSoundData(_pos, _radius, _isLanging, _isMGFire, _isRoad);
}

bool Player::GetIsRespawn(void) const
{
	return (actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::NONE)
			&& actionController_->GetPreActionNum() == static_cast<int>(ACTION_TYPE::DEFEAT));
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
		&& actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL)
		||!actionController_->IsEndActionActive()
		&& actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::ATTACK)
		|| hp_ <= 0)
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

	const VECTOR SHOT_LOCAL_POS = VGet(0.0f, 25.0f, 0.0f);
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
			actionController_->Active(static_cast<int>(ACTION_TYPE::JUMP));
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
			PlayAnimation(ANIM_TYPE::DODGE, false, false);
			actionController_->Active(static_cast<int>(ACTION_TYPE::DODGE));
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

void Player::ProcessDefeat(void)
{
	
	if (actionController_->IsEndActionActive()
		&& animation_->IsEnd()
		&& animation_->GetPlayType() == static_cast<int>(ANIM_TYPE::DEFEAT))
	{
		// 撃破アニメーションが終了時にリスポーン
		SetRespawn();
	}
	else if (actionController_->GetCurActionNum() != static_cast<int>(ACTION_TYPE::DEFEAT))
	{
		actionController_->Active(static_cast<int>(ACTION_TYPE::DEFEAT));
		PlayAnimation(Player::ANIM_TYPE::DEFEAT, false);
	}

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

	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::MISSILE_PUSH))
	{
		VECTOR hitPos = CollisionController::GetInstance().IsActorHitPosWithTag(this, ColliderBase::TAG::MISSILE_PUSH);
		VECTOR knockDir = VSub(transform_.pos, hitPos);
		knockDir.y = KNOCK_POW_Y;
		knockDir = UtilityMath::VNormalize(knockDir);

		SetKnock(knockDir, 10.0f, false);
	}
}

void Player::ProcessAttack(void)
{
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
		int actionNum = static_cast<int>(ACTION_TYPE::ATTACK) + ((IS_COMBO) ? curAttackNum_ : 0);
		actionController_->Active(actionNum);

		curAttackNum_++;

		// 攻撃時に左右交互に弾を投げるアニメーション
		ANIM_TYPE type = ((curAttackNum_ % 2 == 0) ? ANIM_TYPE::THROW_LEFT : ANIM_TYPE::THROW_RIGHT);
		PlayAnimation(type, false);
	}
}
void Player::ProcShotSpecial(void)
{
	// 攻撃可能か否か
	const bool canAttack = !actionController_->IsActiveAction();

	if (canAttack)
	{
		shotType_ = SHOT_TYPE_SPECIAL[static_cast<int>(jobType_)];

		CreateBullet();

		// 登録した攻撃アクションを呼び出す
		int actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		actionController_->Active(actionNum);

		curAttackNum_++;

		// 攻撃時に左右交互に弾を投げるアニメーション
		ANIM_TYPE type = ((curAttackNum_ % 2 == 0) ? ANIM_TYPE::THROW_LEFT : ANIM_TYPE::THROW_RIGHT);
		PlayAnimation(type, false);
	}
}

void Player::UpdateBullets(void)
{
	// 発射時の手のフレームに生成した弾を追従させる
	const int FRAME_FINGER_LEFT = FRAME_NUM_FINGER_LEFT.at(playerType_);
	const int FRAME_FINGER_RIGHT = FRAME_NUM_FINGER_RIGHT.at(playerType_);
	const int FRAME_FINGER = ((curAttackNum_ % 2 == 0) ? FRAME_FINGER_LEFT : FRAME_FINGER_RIGHT);

	const int FRAME_HAND_PALM = (FRAME_FINGER - 1);

	VECTOR posFinger = UtilityMath::VECTOR_ZERO, posHandPalm = UtilityMath::VECTOR_ZERO;
	posFinger = MV1GetFramePosition(transform_.modelId, FRAME_FINGER);
	posHandPalm = MV1GetFramePosition(transform_.modelId, FRAME_HAND_PALM);

	throwDir_ = UtilityMath::VNormalize(VSub(posFinger, posHandPalm));
	throwPos_ = posFinger;

	for (auto& bullet : bullets_)
	{
		bullet->Update();
	}

	if (bullets_.empty()) { return; }
	

	if (shotIndex_ != -1)
	{
		int temp = MV1GetFrameNum(transform_.modelId);
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

	// 弾の再利用処理
	for (auto& bullet : bullets_)
	{
		// 拡散弾では再利用生成を行わない
		if (shotType_ == SHOT_TYPE::CLUSTER) { break; }

		if (!bullet->IsAlive()
			&& static_cast<int>(shotType_) == bullet->GetShotType())
		{
			bullet->Init();
			bullet->Create(transform_.pos, throwDir_, curAttackNum_, (curAttackNum_ >= (attackNumMax_ - 1)));
			return;
		}
		shotIndex_++;
	}

	int type = static_cast<int>(shotType_);
	switch (shotType_)
	{
		case SHOT_TYPE::BIG:
			bullet = std::make_unique<PBulletBig>(type);
		break;

		case SHOT_TYPE::BOMB:
			bullet = std::make_unique<PBulletBomb>(type);
		break;

		case SHOT_TYPE::BOMB_FINISH:
			bullet = std::make_unique<PBulletBomb>(type);
		break;

		case SHOT_TYPE::RECOVERY:
			bullet = std::make_unique<PBulletRecovery>(type);
		break;

		case SHOT_TYPE::POISON:
			bullet = std::make_unique<PBulletPoison>(type);
		break;

		// 連射
		case SHOT_TYPE::RAPID_FIRE:
		{
			bullet = std::make_unique<PBulletNormal>
						(SCALE_RAPID, RADIUS_RAPID, POWER_RAPID
						, SHOT_SPEED_XZ_RAPID, SHOT_SPEED_Y_RAPID, ALIVE_TIME_RAPID
						, type, false);
		}
		break;

		// 拡散弾
		case SHOT_TYPE::CLUSTER:
		{
			CreateCluster();
			return;
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

void Player::CreateCluster(void)
{
	// 円の分割数
	constexpr float ANGLE = (360.0f / CLUSTER_SPLIT);

	const float RANGE = 50;

	float angle = 0;
	int cnt = 0, listCnt = -1;
	Quaternion rot = Quaternion::Identity();

	VECTOR createPos = throwPos_;

	// 弾を生成
	std::unique_ptr<PBulletNormal> bullet;

	// 中心の弾を生成
	clusterBullets_.at(cnt) = _CreateClusterBullet(throwDir_);

	createPos.y += 1.0f;

	const int MAX = (CLUSTER_NUM_MAX / CLUSTER_SPLIT) + 1;
	int spawnMax;

	for (int i = 1; i < MAX; i++)
	{
		rot = Quaternion::Identity();

		// 生成最大数
		spawnMax = (CLUSTER_SPLIT * i);

		for (int circle = 0; circle < (spawnMax - 1); circle++)
		{
			// 円状に一定の範囲間隔で生成位置を設定
			angle = ((ANGLE / i) * (circle + 1));
			rot = rot.Mult(Quaternion::AngleAxis(UtilityMath::Deg2RadF(angle), UtilityMath::AXIS_Y));

			//VECTOR shotDir = rot.GetForward();
			VECTOR shotDir = UtilityMath::VNormalize(VAdd(transform_.GetForward(), rot.GetForward()));
			createPos = VScale(shotDir, RANGE * i);

			// 円状に敵生成処理
			// 中心の弾を生成
			std::unique_ptr bullet = _CreateClusterBullet(throwDir_);

			bullet->Load();
			bullet->Init();
			bullet->Create(createPos, shotDir);
			clusterBullets_.at(++cnt) = std::move(bullet);

			// 生成数が一定を超えたら終了
			if (cnt > CLUSTER_NUM_MAX) { break; }
		}
	}

}
void Player::ShotCluster(void)
{
	// 発射処理の有効化
	for (auto& bullet : clusterBullets_)
	{
		bullet->Shot();
	}

	shotIndex_ = -1;
}
std::unique_ptr<PBulletNormal> Player::_CreateClusterBullet(const VECTOR& _throwDir)
{
	constexpr float SCALE = 0.25f;
	constexpr float RADIUS = 10.0f;
	constexpr int POWER = 2;
	constexpr float SHOT_SPEED = 7.5f;
	constexpr float ALIVE_TIME = 1.25f;
	
	return std::make_unique<PBulletNormal>(SCALE, RADIUS, POWER, SHOT_SPEED, 0.0f, ALIVE_TIME
		,static_cast<int>(SHOT_TYPE::CLUSTER) , false);
}

void Player::ShotBullet(void)
{

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

void Player::PlayAnimation(ANIM_TYPE _type, bool _isLoop, bool _isAnimBlend, float _animSpeed)
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

	// ブレンド無効時、ブレンド時間を減少
	float blendTime = ((!_isAnimBlend) ? 0.001f : AnimationController::BLEND_TIME_DEFAULT);

	animType_ = _type;
	
	animation_->Play(static_cast<int>(animType_), _isLoop, _animSpeed, blendTime);
}

