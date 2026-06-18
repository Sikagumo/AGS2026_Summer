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
#include "../Weapon/Bullet/Player/PBulletRapidFire.h"
#include "../../../../Application.h"


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
static constexpr float DODGE_POW = 15.0f;
constexpr float TIME_DODGE = 0.65f;
constexpr float TIME_WAIT_DODGE = 1.75f;

constexpr float BODY_POS_OFFSET_Y = 25.0f;


Player::Player(int _playerNo, BULLET_TYPE _bulletType, const VECTOR& _startPos)
	: PlayerBase::PlayerBase(_playerNo, _bulletType, _startPos)
	, shadowHandle_(-1)
	, animType_(ANIM_TYPE::IDLE)	
	, curAttackNum_(0)
	, throwPos_(UtilityMath::VECTOR_ZERO), throwDir_(UtilityMath::VECTOR_ZERO)
	, shotIndex_(-1)
	, isCameraRotActive_(false)
	, curTimeWaitDodge_(0.0f), timeActiveDodge_(0.0f)
	, knockPowXZ_(UtilityMath::VECTOR2F_ZERO)
	, dodgePowXZ_(UtilityMath::VECTOR2F_ZERO)
{
	constexpr int BULLET_MAX = 3;
	attackNumMax_ = BULLET_MAX;

	constexpr float MOVE_SPEED = 6.5f;
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

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::IDLE)
		, 30.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_IDLE));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::RUN)
		, 40.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_RUN));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_LEFT)
		, 20.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_LEFT));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RIGHT)
		, 20.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RIGHT));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RUN)
		, 20.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RUN));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::JUMP)
		, 50.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_JUMP)
		, true, VGet(0.0f, 50.0f, 0.0f));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::DODGE)
		, 40.0f, resMng.LoadHandleId(ResourceManager::SRC::ANIM_DODGE)
		, true, VGet(0.0f, 22.5f, 0.0f));


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

	curAttackNum_ = 0;

	constexpr float SHOT_TIME_INCREMENT = 0.3f; // 行動間隔上昇値
	constexpr float SHOT_TIME_INC_INPUT = 0.2f; // 行動間隔上昇値

	constexpr float SHOT_TIME_ACTIVE = 2.5f; // 有効時間
	constexpr float SHOT_TIME_ACTION_ACTIVE = 1.25f; // 行動有効時間
	constexpr float SHOT_TIME_ACTIVE_INPUT = 1.725f; // 入力可能時間
	constexpr float SHOT_TIME_END = 0.25f; // 終了時間

	constexpr float SHOT_TIME_STOP = 0.85f; // 停止時間
	constexpr float SHOT_TIME_STOP_ACTIVE = 1.15f; // 停止有効化時間



	float timeActive, timeActionActive, timeInput, timeEnd;
	timeActive = SHOT_TIME_ACTIVE;
	timeActionActive = SHOT_TIME_ACTION_ACTIVE;
	timeInput = SHOT_TIME_ACTIVE_INPUT;
	timeEnd = SHOT_TIME_END;

	actionController_->SetAction(0, 50, timeActive, timeActionActive, timeEnd
								 , std::bind(&Player::ShotBullet, this)
								 , 0.0f, 0.0f, timeInput);

	timeActive += SHOT_TIME_INCREMENT;
	timeInput += SHOT_TIME_INC_INPUT;
	timeEnd += SHOT_TIME_INCREMENT;

	actionController_->SetAction(1, 75, timeActive, timeActionActive, timeEnd
								 , std::bind(&Player::ShotBullet, this)
								 , SHOT_TIME_STOP, SHOT_TIME_STOP_ACTIVE, timeInput);

	timeActive += (SHOT_TIME_INCREMENT * 2);
	actionController_->SetAction(2, 150, timeActive, timeActionActive, timeEnd
								 , std::bind(&Player::ShotBullet, this)
								 , SHOT_TIME_STOP, SHOT_TIME_STOP_ACTIVE, 0.0f);


	// ジャンプ
	actionController_->SetAction(3, 0, 0.2f, 0.1f, 0.0f
								 , std::bind(&Player::Jump, this)
								 , 0.05f, 0.2f);

	// 回避
	actionController_->SetAction(4, 0, 0.5f, 0.1f, 0.0f
								 , std::bind(&Player::Dodge, this)
								 , 0.075f, 0.15f);
}

void Player::Draw(void)
{
	COLOR_F material = COLOR_F();

	if (timeInv_ > 0.0f)
	{
		material = COLOR_F(1.0f, 0.0f, 0.0f, 1.0f);
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
	hp_ = MAX_HP;
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
	MoveLimit();

	// 胴体位置更新
	bodyPos_ = transform_.pos;
	bodyPos_.y += BODY_POS_OFFSET_Y;


	UpdaetaSound();
	
	/*
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_LEFT)
		|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::CROSS_LEFT))
	{
		int type = static_cast<int>(playerType_) - 1;
		if (type < 0) { type = static_cast<int>(PLAYER_TYPE::MAX) - 1; }

		SetPlayerType(static_cast<PLAYER_TYPE>(type));
		animation_->SetModelId(transform_.modelId);
	}
	

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RIGHT)
		|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::CROSS_RIGHT))
	{
		int type = static_cast<int>(playerType_) + 1;
		if (type >= static_cast<int>(PLAYER_TYPE::MAX)) { type = 0; }

		SetPlayerType(static_cast<PLAYER_TYPE>(type));
		animation_->SetModelId(transform_.modelId);
	}
	*/
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
	DrawFormatString(10, Application::SCREEN_HALF_Y + (16 * 5), 0xff0000, "プレイヤーHP：%d"
		, hp_);

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

void Player::SetSoundDate(VECTOR _pos, float _radius, bool _isLanging,bool _isMGFire, bool _isRoad)
{
	PlayerBase::SetSoundDate(_pos, _radius, _isLanging, _isMGFire, _isRoad);
}

void Player::ReleasePost(void)
{
}

int Player::GetPower(void)
{
	/*
	int power = 0;
	for (auto& bullet : bullets_)
	{
		if (!bullet->GetIsVisible()) { continue; }

		power += bullet->GetPower();

		if (bullet->GetPower() > 0)
		{
			bullet->BlastAction();
		}
	}

	return power;*/

	return actionController_->GetActionAttackPower(curAttackNum_ - 1);
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

	// カメラの方向で進行
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotY();

	if (!UtilityMath::EqualsVZero(dir))
	{
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
		&& timeActiveDodge_ <= 0.0f
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
	if (curTimeWaitDodge_ <= 0.0f && !UtilityMath::EqualsVZero(moveDir_))
	{
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

	if (timeActiveDodge_ > 0.0f)
	{
		timeActiveDodge_ -= TimeManager::GetInstance().GetDeltaTime();
	}
	else
	{
		dodgePowXZ_ = UtilityMath::VECTOR2F_ZERO;
	}
}
void Player::Dodge(void)
{
	dodgePowXZ_ = Vector2F(moveDir_.x, moveDir_.z);
	dodgePowXZ_ *= DODGE_POW;

	timeActiveDodge_ = TIME_DODGE;

	timeInv_ = TIME_DODGE;
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

	if (InputManager::GetInstance().IsTrgMouseLeft()
		|| InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER))
	{
		const bool canCombo = (actionController_->IsActiveAction() && actionController_->IsActiveInput());
		const bool canAttack = !actionController_->IsActiveAction();

		if (canCombo || canAttack)
		{
			// 行動回数が最大値を超えた場合、０に戻す
			if (curAttackNum_ >= attackNumMax_)
			{
				curAttackNum_ = 0;
			}

			CreateBullet();

			actionController_->Active(curAttackNum_++);

			ANIM_TYPE type = ((curAttackNum_ % 2 == 0) ? ANIM_TYPE::THROW_LEFT : ANIM_TYPE::THROW_RIGHT);
			PlayAnimation(type, false);
		}
	}
}

void Player::UpdateBullets(void)
{
	// 発射時の手のフレームに生成した弾を追従させる
	constexpr int FRAME_FINGER_LEFT = 23;
	constexpr int FRAME_FINGER_RIGHT = 47;
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
		if (bulletType_ == BULLET_TYPE::BIG && animation_->isStop())
		{
			bullets_[shotIndex_]->PreActiveProcess();
		}

		bullets_[shotIndex_]->SetFollow(throwPos_, throwDir_);
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
		moveDir_ = Quaternion::PosAxis(goalRot, UtilityMath::DIR_F);
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

	switch (bulletType_)
	{
		case BULLET_TYPE::BIG:
			bullet = std::make_unique<PBulletBig>();
			bullet->Load();
		break;

		default:
		break;

	}

	bullet->Init();
	bullet->Create(throwPos_, throwDir_, curAttackNum_, (curAttackNum_ >= (attackNumMax_ - 1)));

	bullets_.emplace_back(std::move(bullet));
}

void Player::ShotBullet(void)
{
	VECTOR shotDir = transform_.GetForward();

	if (bulletType_ != BULLET_TYPE::RAPID_FIRE)
	{
		// 放物線状に投げる
		const VECTOR UP_VEC = VScale(UtilityMath::DIR_UP, 1.0f);

		shotDir = VAdd(shotDir, UP_VEC);
		shotDir = UtilityMath::VNormalize(shotDir);
	}

	bullets_[shotIndex_]->Shot(shotDir);
	shotIndex_ = -1;
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

void Player::PlayAnimation(ANIM_TYPE _type, bool _isLoop)
{
	if (_type != ANIM_TYPE::THROW_LEFT
		&& _type != ANIM_TYPE::THROW_RIGHT)
	{
		if (animType_ == ANIM_TYPE::THROW_LEFT
			|| animType_ == ANIM_TYPE::THROW_RIGHT)
		{
			curAttackNum_ = 0;
		}
	}

	animType_ = _type;

	animation_->Play(static_cast<int>(_type), _isLoop);
}

