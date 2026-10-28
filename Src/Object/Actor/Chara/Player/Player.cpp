#include "Player.h"
#include <memory>
#include <cassert>
#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../Common/AnimationController.h"

#include "../../../../Utility/UtilityMath.h"
#include "../../../../Manager/Generic/KeyConfInputManager.h"
#include "../../../../Manager/Generic/SceneManager.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../../Manager/Decoration/SoundManager.h"
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
#include "../../../../Net/NetStructures.h"
#include "../../../../Manager/System/NetManager.h"

namespace
{
	// 衝突判定用線分位置
	constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, 0.0f, 0.0f };

	// 衝突判定用カプセル位置
	constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
	constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 18.0f, 0.0f };

	// 衝突判定用カプセル球体半径
	constexpr float COL_CAPSULE_RADIUS = 10.0f;

	// プレイヤースキンとモデルの対応表
	const std::map<Player::SKIN_TYPE, ResourceManager::SRC>
		SKIN_SRC = { { Player::SKIN_TYPE::HUMAN, ResourceManager::SRC::MODEL_PLAYER_HUMAN}
					, { Player::SKIN_TYPE::MONKEY, ResourceManager::SRC::MODEL_PLAYER_MONKEY}
					, { Player::SKIN_TYPE::BIRD, ResourceManager::SRC::MODEL_PLAYER_BIRD}
					, { Player::SKIN_TYPE::DOG, ResourceManager::SRC::MODEL_PLAYER_DOG} };

	// 回転の間隔
	constexpr float ROT_DELAY_TERM = 0.2f;
};

Player::Player(int _playerNo, JOB_TYPE _jobType, SKIN_TYPE _skinType, const VECTOR& _startPos)
	: PlayerBase::PlayerBase(_playerNo, _jobType, _startPos, _skinType)
	, animationType_(ANIMATION_TYPE::IDLE)
	, actionController_(nullptr)
	, isCameraRotActive_(false)
	, curTimeWaitDodge_(0.0f)
	, knockPowXZ_(UtilityMath::VECTOR2F_ZERO)
	, dodgePowXZ_(UtilityMath::VECTOR2F_ZERO)
	, shotTerm_(0.0f)
	, curTimeDefeat_(0.0f)
	, isHostControl_(false)
	, netKey_(0)
	, isNetAttack_(false)
	, isAttackSend_(false)
	, shotPointVertex_{}
{
	moveSpeed_ = MOVE_SPEED;

	// 武器クラス生成
	weaponController_ = std::make_unique<PlayerWeaponController>(static_cast<int>(jobType_), transform_);

	// 弾の衝突の範囲
	const VECTOR INIT_NORM = UtilityMath::DIR_UP;
	const COLOR_U8 INIT_DIFUSECOLOR = GetColorU8(255, 255, 255, 255);

	for (auto& vertex : shotPointVertex_)
	{
		vertex.norm = INIT_NORM;
		vertex.dif = INIT_DIFUSECOLOR;
	}

	// UV座標の割り当て
	shotPointVertex_[LEFT_BACK].u     = 0.0f; shotPointVertex_[LEFT_BACK].v     = 1.0f;
	shotPointVertex_[LEFT_FORWARD].u  = 0.0f; shotPointVertex_[LEFT_FORWARD].v  = 0.0f;
	shotPointVertex_[RIGHT_BACK].u    = 1.0f; shotPointVertex_[RIGHT_BACK].v    = 1.0f;
	shotPointVertex_[RIGHT_FORWARD].u = 1.0f; shotPointVertex_[RIGHT_FORWARD].v = 0.0f;
}

Player::~Player(void)
{
	// メモリ解放
	Release();
}

void Player::Load(void)
{
	transform_.modelId = ResourceManager::GetInstance()
		.LoadModelDuplicate(SKIN_SRC.at(skinType_));

	// 回復SE
	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_RECOVERY_PLAYER
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_PLAYER_RECOVERY));

	// 武器コントローラのリソース読み込み
	weaponController_->Load();
}

void Player::InitAnimation(void)
{
	animation_ = std::make_unique<AnimationController>(transform_.modelId);
	
	// 待機アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::IDLE)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_IDLE), ANIMATION_SPEED_IDLE);

	// 移動アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::WALK)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_WALK), ANIMATION_SPEED_WALK);

	float throwSpeed = 0.0f;

	if (jobType_ == JOB_TYPE::CANNON)
	{
		throwSpeed = ANIMATION_SPEED_THROW_BIG;
	}
	else if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		throwSpeed = ANIMATION_SPEED_THROW_RAPID;
	}
	else
	{
		throwSpeed = ANIMATION_SPEED_THROW_BOMB;
	}

	// 左投擲
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::THROW_LEFT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_THROW_LEFT), throwSpeed);

	// 右投擲
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::THROW_RIGHT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_THROW_RIGHT), throwSpeed);

	// ジャンプ
	constexpr VECTOR LOCAL_POS_JUMP = { 0.0f, 50.0f, 0.0f };
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::JUMP)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_JUMP)
		, LOCAL_POS_JUMP, ANIMATION_SPEED_JUMP);

	// 回避
	constexpr VECTOR LOCAL_POS_DODGE = { 0.0f, 25.0f, 0.0f };
	constexpr VECTOR LOCAL_POS_DODGE_END = { 0.0f, 50.0f, 0.0f };
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DODGE)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_DODGE)
		, LOCAL_POS_DODGE, LOCAL_POS_DODGE_END, ANIMATION_SPEED_DODGE);

	// 撃墜
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DEFEAT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_DEFEAT));

	// アニメーション再生
	PlayAnimation(ANIMATION_TYPE::IDLE);
}

void Player::InitTransform(void)
{
	transform_.InitTransform(MODEL_SCALE
		, Quaternion::Identity()
		, Quaternion::AngleAxis(UtilityMath::Deg2RadF(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
		, START_POS, VGet(0.0f, LOCAL_POS_Y, 0.0f));

	transform_.Update();
}

void Player::InitCollider(void)
{
	// 地面当たり判定
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
	isJump_ = false;

	// 行動クラス生成
	const bool IS_RAPID_FIRE = (jobType_ == JOB_TYPE::RAPID_FIRE);
	actionController_ = std::make_unique<PActionController>(animation_, IS_RAPID_FIRE);

	// 行動初期化
	InitActions();
}

void Player::InitActions(void)
{
	int actionNum = 0;

	// ジャンプ
	actionNum = static_cast<int>(ACTION_TYPE::JUMP);
	actionController_->SetAction(actionNum, JUMP_TOTAL_ACTIVE, JUMP_TIMING_ACTION, JUMP_TOTAL_END
		, std::bind(&Player::Jump, this), JUMP_TOTAL_STOP, JUMP_TIMING_STOP);

	// 回避
	actionNum = static_cast<int>(ACTION_TYPE::DODGE);
	actionController_->SetAction(actionNum, DODGE_TOTAL_ACTIVE, DODGE_TIMING_ACTION, DODGE_TOTAL_END
		, std::bind(&Player::Dodge, this), DODGE_TOTAL_STOP, DODGE_TIMING_STOP);

	// 撃破
	actionNum = static_cast<int>(ACTION_TYPE::DEFEAT);
	actionController_->SetAction(actionNum, DEFEAT_TOTAL_ACTIVE, DEFEAT_TIMING_ACTION, DEFEAT_TOTAL_END
		, nullptr, DEFEAT_TOTAL_STOP, DEFEAT_TIMING_STOP);

	if (jobType_ == JOB_TYPE::CANNON)
	{
		float timeTotalActive = 0.0f, timeTimingAction = 0.0f, timeTotalStop = 0.0f, timeTimingStop = 0.0f;
		float timeInput = SHOT_CANNON_TOTAL_INPUT;
		float timeTotalEnd = SHOT_CANNON_TOTAL_END;

		// 特殊弾
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		actionController_->SetAction(actionNum, SHOT_CANNON_TOTAL_ACTIVE, timeTimingAction, timeTotalEnd
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get())
			, timeTotalStop, timeTimingStop);

		// 通常弾 (1弾目)
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		timeTotalActive = SHOT_CANNON_TOTAL_ACTIVE;
		timeTimingAction = SHOT_CANNON_TIMING_ACTION;

		actionController_->SetAction(actionNum, timeTotalActive, timeTimingAction, timeTotalEnd
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get())
			, timeTotalStop, timeTimingStop, timeInput);

		// 通常弾 (2弾目)
		actionNum++;
		timeTotalActive += SHOT_CANNON_TOTAL_ACTIVE_INCREMENT;
		timeInput += SHOT_CANNON_TOTAL_INPUT;
		timeTotalEnd += SHOT_CANNON_TOTAL_END_INCREMENT;
		timeTotalStop = SHOT_CANNON_TOTAL_STOP;
		timeTimingStop = SHOT_CANNON_TIMING_STOP;

		actionController_->SetAction(actionNum, timeTotalActive, timeTimingAction, timeTotalEnd
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get())
			, timeTotalStop, timeTimingStop, timeInput);

		// 通常弾 (3弾目)
		actionNum++;
		timeTotalActive += (SHOT_CANNON_TOTAL_ACTIVE_INCREMENT * 2);

		actionController_->SetAction(actionNum, timeTotalActive, timeTimingAction, timeTotalEnd
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get())
			, timeTotalStop, timeTimingStop);
	}
	else if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		// 特殊弾(連射)
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		actionController_->SetAction(actionNum, SHOT_RAPID_TOTAL_ACTIVE, SHOT_RAPID_TIMING_ACTIVE, SHOT_RAPID_TOTAL_END
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get()));

		// 通常弾(拡散)
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		actionController_->SetAction(actionNum, SHOT_CLUSTER_TOTAL_ACTIVE, SHOT_CLUSTER_TIMING_ACTIVE, SHOT_CLUSTER_TOTAL_END
			, std::bind(&PlayerWeaponController::ShotCluster, weaponController_.get()));
	}
	else if (jobType_ == JOB_TYPE::SUPPORT)
	{
		// 特殊弾(回復)
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		actionController_->SetAction(actionNum, SHOT_SUPPORT_TOTAL_ACTIVE, SHOT_SUPPORT_TIMING_ACTIVE, SHOT_SUPPORT_TOTAL_END
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get()));

		// 通常弾(毒)
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		actionController_->SetAction(actionNum, SHOT_SUPPORT_TOTAL_ACTIVE, SHOT_SUPPORT_TIMING_ACTIVE, SHOT_SUPPORT_TOTAL_END
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get()));
	}
	else if (jobType_ == JOB_TYPE::BOMB)
	{
		// 通常弾
		actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		actionController_->SetAction(actionNum, SHOT_BOMB_TOTAL_ACTIVE, SHOT_BOMB_TIMING_ACTION, SHOT_BOMB_TOTAL_END
			, std::bind(&PlayerWeaponController::ShotBullet, weaponController_.get()));
	}
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
		// 撃墜処理
		ProcessDefeat();
		return;
	}

	// 操作中のプレイヤーで動作させる
	if (isHostControl_)
	{
		ProcessJump();
		ProcessMove();
		ProcessAttack();
		ProcessDodge();
		ProcessRecovery();
	}

	// 弾更新処理
	weaponController_->Update(static_cast<int>(skinType_), animation_->IsStop());

	// 吹っ飛ばし処理
	ProcessKnockback();

	// 胴体位置更新
	bodyPos_ = transform_.pos;

	UpdateSound();

	// マルチプレイのため追加
	if (isHostControl_)
	{
		SendMyActionToNetManager();
	}
}

void Player::DrawPre(void)
{
	COLOR_F material = COLOR_F();

	// 発射方向描画
	weaponController_->DrawShotOrbit();

	weaponController_->Draw();

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
}

void Player::DrawLate(void)
{
	// 影描画
	CharaBase::DrawShadowRound(SHADOW_SCALE);

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
	PlayAnimation(ANIMATION_TYPE::IDLE, true, false);
}

VECTOR Player::CalcAddPosition(void)
{
	/* 座標にベクトルを加算 */

	VECTOR ret = UtilityMath::VECTOR_ZERO;

	if (hp_ <= 0
		|| CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::WALL))
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
	VECTOR dodgeVec = UtilityMath::VECTOR_ZERO;

	if (CollisionController::GetInstance()
		.IsTagCollidingWithTag(ColliderBase::TAG::PLAYER, ColliderBase::TAG::WALL))
	{
		// 壁に衝突時の回避量加算
		dodgeVec = VGet(-dodgePowXZ_.x, 0.0f, -dodgePowXZ_.y);
		ret = VAdd(ret, dodgeVec);
		dodgePowXZ_ = UtilityMath::VECTOR2F_ZERO;
	}
	else
	{
		// 回避量加算
		dodgeVec = VGet(dodgePowXZ_.x, 0.0f, dodgePowXZ_.y);
		ret = VAdd(ret, dodgeVec);
	}

	return ret;
}

bool Player::IsRespawn(void) const
{
	// 撃破アニメーション終了の瞬間、有効
	return (actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::DEFEAT)
			&& animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::DEFEAT)
			&& animation_->IsEnd());
}

void Player::ProcessMove(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();

	// 左スティックの入力方向を取得
	VECTOR direction = keyConfInputManager.GetLeftStickDirection();

	// スティック入力がない場合はキーボード入力を適用
	if (VSquareSize(direction) < 0.01f)
	{
		// 完全にゼロにリセットする
		direction = UtilityMath::VECTOR_ZERO;

		if (keyConfInputManager.isPressed("UP"))
		{
			direction.z++;
		}
		if (keyConfInputManager.isPressed("DOWN"))
		{
			direction.z--;
		}
		if (keyConfInputManager.isPressed("LEFT"))
		{
			direction.x--;
		}
		if (keyConfInputManager.isPressed("RIGHT"))
		{
			direction.x++;
		}
	}

	if (IsActionAttack()|| hp_ <= 0)
	{
		// ジャンプ行動有効時に行動前の時、処理終了
		movePow_ = UtilityMath::VECTOR_ZERO;
		return;
	}

	// 無敵中、移動無効
	else if (timeInvDodge_ > 0.0f) { return; }

	// カメラの方向で進行
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotY();

	if (!UtilityMath::EqualsVZero(direction))
	{
		// 攻撃中は移動速度を低下
		moveSpeed_ = ((weaponController_->GetShotIndex() != -1)
			? MOVE_SPEED_SHOT : MOVE_SPEED);

		direction = UtilityMath::VNormalize(direction);
		movePow_ = UtilityMath::VECTOR_ZERO;

		// 移動方向を取得
		moveDir_ = Quaternion::PosAxis(cameraRot, direction);
		moveDir_.y = 0.0f;

		// 加速度に割り当て
		movePow_ = VScale(UtilityMath::VNormalize(moveDir_), moveSpeed_);
	}
	else
	{
		// 移動量無効化
		movePow_ = UtilityMath::VECTOR_ZERO;
	}

	if (!isJump_
		&& actionController_->GetActionState() == PActionController::PACTION_STATE::NONE)
	{
		// ジャンプ行動無効時、移動or待機アニメーション
		if (!UtilityMath::EqualsVZero(direction))
		{
			PlayAnimation(ANIMATION_TYPE::WALK);
		}
		else
		{
			PlayAnimation(ANIMATION_TYPE::IDLE);
		}
	}
}

bool Player::IsActionAttack(void) const
{
	// 攻撃行動中か否か
	return (!actionController_->IsEndActionActive()
		&& actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL)
		|| !actionController_->IsEndActionActive()
		&& actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::ATTACK));
}

void Player::ProcessJump(void)
{
	if (!actionController_->IsActiveAction() &&
		actionController_->GetActionState() == PActionController::PACTION_STATE::NONE)
	{
		bool isJumpTriggered = KeyConfInputManager::GetInstance().isTrigerDown("JUMP");
		
		// ジャンプ
		if (isJumpTriggered && !isJump_)
		{
			PlayAnimation(ANIMATION_TYPE::JUMP, false);
			actionController_->Active(static_cast<int>(ACTION_TYPE::JUMP));
		}
	}

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
			|| weaponController_->GetShotIndex() != -1)
		{
			return;
		}

		bool isDodgeTriggered = KeyConfInputManager::GetInstance().isTrigerDown("DODGE");

		if (isDodgeTriggered)
		{
			PlayAnimation(ANIMATION_TYPE::DODGE, false, false);
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
		&& animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::DEFEAT))
	{
		// 撃破アニメーションが終了時にリスポーン
		SetRespawn();
	}
	else if (actionController_->GetCurActionNum() != static_cast<int>(ACTION_TYPE::DEFEAT))
	{
		actionController_->Active(static_cast<int>(ACTION_TYPE::DEFEAT));
		PlayAnimation(Player::ANIMATION_TYPE::DEFEAT, false);
	}
}

void Player::ProcessKnockback(void)
{
	/* 吹っ飛ばしの重力加算 */

	constexpr float WAVE_KNOCK = 3.25f;
	constexpr float MISSILE_KNOCK = 2.5f;

	// ボスの衝撃波による吹っ飛ばし
	if (CollisionController::GetInstance().IsActorCollidingWithTag(
		this, ColliderBase::TAG::HIT_WAVE))
	{
		VECTOR hitPos =
			CollisionController::GetInstance().IsActorHitPosWithTag(
				this, ColliderBase::TAG::HIT_WAVE);

		VECTOR knockDir = VSub(transform_.pos, hitPos);

		knockDir.y = KNOCK_POW_Y;
		knockDir = UtilityMath::VNormalize(knockDir);

		SetKnock(knockDir, WAVE_KNOCK, false);
	}

	// ボスのミサイルによる吹っ飛ばし
	if (CollisionController::GetInstance().IsActorCollidingWithTag(
		this, ColliderBase::TAG::MISSILE_PUSH))
	{
		VECTOR hitPos =
			CollisionController::GetInstance().IsActorHitPosWithTag(
				this, ColliderBase::TAG::MISSILE_PUSH);

		VECTOR knockDir = VSub(transform_.pos, hitPos);

		knockDir.y = (KNOCK_POW_Y / 2);
		knockDir = UtilityMath::VNormalize(knockDir);

		SetKnock(knockDir, MISSILE_KNOCK, false);
	}


	// 壁に衝突・落下中に吹っ飛ばしが残っている場合は停止を停止
	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::WALL)
		|| !UtilityMath::EqualsVZero(knockPowXZ_) && jumpPow_ < 0.0f)
	{
		knockPowXZ_ = UtilityMath::VECTOR2F_ZERO;
	}
}

void Player::ProcessAttack(void)
{
	// 連射弾型
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();

	if (jobType_ == JOB_TYPE::RAPID_FIRE)
	{
		if (shotTerm_ <= 0.0f)
		{
			// 押しっぱなしでの連射判定
			if (keyConfInputManager.isPressed("ATTACK_NORMAL"))
			{
				ProcShotNormal();
				shotTerm_ = SHOT_RAPID_TERM;
			}

			if (keyConfInputManager.isPressed("ATTACK_SPECIAL"))
			{
				// 弾が生存中は拡散弾を有効にしない
				if (weaponController_->IsClusterAlive()) { return; }

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
		// 押した瞬間の単発判定
		if (keyConfInputManager.isTrigerDown("ATTACK_NORMAL"))
		{
			ProcShotNormal();
		}

		if (keyConfInputManager.isTrigerDown("ATTACK_SPECIAL"))
		{
			ProcShotSpecial();
		}
	}
}

void Player::ProcessRecovery(void)
{
	// 回復処理
	if (CollisionController::GetInstance()
		.IsTagCollidingWithTag(ColliderBase::TAG::PLAYER, ColliderBase::TAG::PLAYER_RECOVERY)
		&& hp_ <= HP_MAX)
	{
		float recovery = (static_cast<float>(HP_MAX) * PBulletRecovery::RECOVERY_RATE);
		hp_ += static_cast<int>(recovery);
		hp_ = ((hp_ > HP_MAX) ? HP_MAX : hp_);

		// SE再生
		SoundManager::GetInstance().Play(SoundManager::SOUND::SE_RECOVERY_PLAYER);
	}
}

void Player::ProcShotNormal(void)
{	
	// コンボをするか否か
	const bool IS_COMBO = (jobType_ == JOB_TYPE::CANNON);
	
	// コンボができて、入力可能か否か
	const bool canCombo = (actionController_->IsActiveAction()
		&& actionController_->IsActiveInput() && IS_COMBO);

	// 攻撃可能か否か
	const bool canAttack = !actionController_->IsActiveAction();

	if (canCombo || canAttack)
	{
		// 弾丸生成
		weaponController_->CreateBullet(false);

		// 発射の種類が無効時、処理終了
		if (weaponController_->GetShotType() == PlayerWeaponController::SHOT_TYPE::NONE) { return; }

		// 巨大弾の場合、登録した攻撃コンボアクションを呼び出す
		int actionNum = static_cast<int>(ACTION_TYPE::ATTACK);
		actionNum  += ((jobType_ == JOB_TYPE::CANNON) ? weaponController_->GetAttackCount() : 0);
		actionController_->Active(actionNum);

		// 攻撃有効をホスト側に送信
		isAttackSend_ = true;

		// 攻撃時に左右交互に弾を投げるアニメーション
		ANIMATION_TYPE type = ((weaponController_->GetAttackCount() % 2 == 0) ? ANIMATION_TYPE::THROW_LEFT : ANIMATION_TYPE::THROW_RIGHT);
		PlayAnimation(type, false);
	}
}
void Player::ProcShotSpecial(void)
{
	// 攻撃可能か否か
	const bool canAttack = !actionController_->IsActiveAction();

	if (canAttack)
	{
		// 弾丸生成処理
		weaponController_->CreateBullet(true);

		// 発射の種類が無効時、処理終了
		if (weaponController_->GetShotType() == PlayerWeaponController::SHOT_TYPE::NONE) { return; }

		// 登録した攻撃アクションを呼び出す
		int actionNum = static_cast<int>(ACTION_TYPE::ATTACK_SPECIAL);
		actionController_->Active(actionNum);

		// 攻撃有効をホスト側に送信
		isAttackSend_ = true;

		// 攻撃時に左右交互に弾を投げるアニメーション
		ANIMATION_TYPE type = ((weaponController_->GetAttackCount() % 2 == 0) ? ANIMATION_TYPE::THROW_LEFT : ANIMATION_TYPE::THROW_RIGHT);
		PlayAnimation(type, false);
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

	// 回転の補間
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, goalRot, ROT_DELAY_TERM);
}

void Player::PlayAnimation(ANIMATION_TYPE _type, bool _isLoop, bool _isAnimBlend, float _animSpeed)
{
	if (IsResetAttackCount(_type))
	{
		// 攻撃回数のリセット
		weaponController_->ResetAttackCount();
	}

	// ブレンド無効時、ブレンド時間を減少
	float blendTime = ((!_isAnimBlend) ? 0.001f : AnimationController::BLEND_TIME_DEFAULT);

	// アニメーション登録
	animationType_ = _type;
	animation_->Play(static_cast<int>(animationType_), _isLoop, _animSpeed, blendTime);
}

bool Player::IsResetAttackCount(ANIMATION_TYPE _type) const
{
	if (jobType_ != JOB_TYPE::CANNON)
	{
		// 巨大弾以外のジョブの場合、false
		return false;
	}

	if (_type == ANIMATION_TYPE::THROW_LEFT
		|| _type == ANIMATION_TYPE::THROW_RIGHT)
	{
		// 再生するアニメーションが投擲アニメーションの場合、false
		return false;
	}
	if (animationType_ != ANIMATION_TYPE::THROW_LEFT
		&& animationType_ != ANIMATION_TYPE::THROW_RIGHT)
	{
		// 再生中のアニメーションが投擲アニメーション以外の場合、false
		return false;
	}

	return true;
}

void Player::SetNetworkAction(const VECTOR& _pos, const Quaternion& _rot, int _animId, bool _isAttack, int _currentHp)
{
	transform_.pos = _pos;
	transform_.quaRot = _rot;

	if (static_cast<int>(animationType_) != _animId)
	{
		PlayAnimation(static_cast<ANIMATION_TYPE>(_animId));
	}

	if (_isAttack && !isNetAttack_)
	{
		ProcShotNormal();
	}

	isNetAttack_ = _isAttack;

	hp_ = _currentHp;
}

void Player::SendMyActionToNetManager(void)
{
	// 操作プレイヤー以外の時、処理終了
	if (!isHostControl_) { return; }

	// クライアント側のプレイヤー情報を送信
	NET_ACTION myAction;
	myAction.key = NetManager::GetInstance().GetMyKey();
	myAction.frameNo = 0;
	myAction.pos = transform_.pos;
	myAction.rot = transform_.quaRot;
	myAction.animId = static_cast<int>(animationType_);
	myAction.currentHp = hp_;
	myAction.actionBits = 0;
	myAction.isAttack = isAttackSend_;
	NetManager::GetInstance().AddSelfAction(myAction);

	// 攻撃状況を無効化
	isAttackSend_ = false;
}

