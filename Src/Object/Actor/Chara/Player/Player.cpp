#include "Player.h"
#include <memory>
#include <cassert>
#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../Common/AnimationController.h"

#include "../../../../Utility/UtilityMath.h"
#include "../../../../Manager/Generic/InputManager.h"
#include "../../../../Manager/Generic/SceneManager.h"
#include "../../../../Camera/Camera.h"
#include "../../../../Common/Quaternion.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Collider/ColliderBase.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderLine.h"
#include "../Weapon/Bullet/Player/PBulletBig.h"
#include "../Weapon/Bullet/Player/PBulletRapidFire.h"


// 衝突判定用線分位置
static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, 0.0f, 0.0f };

// 衝突判定用カプセル位置
static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 50.0f, 0.0f };
static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 18.0f, 0.0f };

// 衝突判定用カプセル球体半径
static constexpr float COL_CAPSULE_RADIUS = 10.0f;


Player::Player(int _playerNo, BULLET_TYPE _playerType)
	: PlayerBase::PlayerBase(_playerNo, _playerType)
	, shadowHandle_(-1)
	, inputManager_(InputManager::GetInstance())
	, animType_(ANIM_TYPE::IDLE)	
	,  curAttackNum_(0)
	, throwPos_(UtilityMath::VECTOR_ZERO), throwDir_(UtilityMath::VECTOR_ZERO)
{
	constexpr int BULLET_MAX = 3;
	attackNumMax_ = BULLET_MAX;

	constexpr float MOVE_SPEED = 5.0f;
	moveSpeed_ = MOVE_SPEED;
}


void Player::Load(void)
{
	transform_.modelId = resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_HUMAN);
}
void Player::DrawDebug(void)
{
	CharaBase::DrawDebug();


}
void Player::InitAnimation(void)
{
	animation_ = std::make_unique<AnimationController>(transform_.modelId);
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::IDLE)
		, 30.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_IDLE));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::RUN)
		, 30.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_RUN));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_LEFT)
		, 17.5f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_THROW_LEFT));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RIGHT)
		, 17.5f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RIGHT));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RUN)
		, 20.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RUN));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::JUMP)
		, 20.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_JUMP));

	PlayAnim(ANIM_TYPE::IDLE);
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

	ownColliders_.emplace(0, new ColliderCapsule(ColliderBase::TAG::PLAYER
										, &transform_, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS));

	ownColliders_.at(0)->SetTriger(false);

	// 衝突判定マネージャに登録
	CollisionManager::GetInstance().RegisterActor(this);
}
void Player::InitPost(void)
{
	constexpr bool IS_RAPID_FIRE = false;
	actionController_ = std::make_unique<PActionController>(animation_, IS_RAPID_FIRE);

	curAttackNum_ = 0;

	constexpr float SHOT_TIME_INCREMENT = 0.5f; // 行動間隔上昇値
	constexpr float SHOT_TIME_INC_ACTION = 0.1875f; // 行動間隔上昇値

	constexpr float SHOT_TIME_ACTIVE = 2.0f; // 有効時間
	constexpr float SHOT_TIME_ACTION_ACTIVE = 1.25f; // 有効時間
	constexpr float SHOT_TIME_ACTIVE_INPUT = 1.725f; // 入力可能時間
	constexpr float SHOT_TIME_END = 1.0f; // 終了時間

	constexpr float SHOT_TIME_STOP = 0.85f; // 停止時間
	constexpr float SHOT_TIME_STOP_ACTIVE = 1.15f; // 停止有効化時間



	float timeActive, timeActionActive, timeInput;
	timeActive = SHOT_TIME_ACTIVE;
	timeActionActive = SHOT_TIME_ACTION_ACTIVE;
	timeInput = SHOT_TIME_ACTIVE_INPUT;

	actionController_->SetAction(0, timeActive, SHOT_TIME_END, timeActionActive
								, std::bind(&Player::ShotBullet, this)
								, 0.0f, 0.0f, timeInput);

	timeActive += SHOT_TIME_INCREMENT;
	//timeActionActive += SHOT_TIME_INC_ACTION;
	actionController_->SetAction(1, timeActive, SHOT_TIME_END, timeActionActive
								, std::bind(&Player::ShotBullet, this)
								, SHOT_TIME_STOP, SHOT_TIME_STOP_ACTIVE, timeInput);

	timeActive += SHOT_TIME_INCREMENT * 2;
	//timeActionActive += SHOT_TIME_INC_ACTION;
	timeInput += (SHOT_TIME_INCREMENT / 2);
	actionController_->SetAction(2, timeActive, SHOT_TIME_END, timeActionActive
								, std::bind(&Player::ShotBullet, this)
								, SHOT_TIME_STOP, SHOT_TIME_STOP_ACTIVE, timeInput);
}


void Player::UpdateProcess(void)
{
	ProcessAttack();

	ProcessJump();

	// 移動操作
	ProcessMove();

	UpdateBullets();

	// ロックオン有効時、カメラ方向に回転
	isDirRotActive_ = !sceneManager_.GetCamera()->GetIsLockOn();
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

	UtilityMath::DrawLineXYZ(transform_.pos, transform_.quaRot);

	DrawFormatString(10, 160, 0xffffff, "player:(%.f,%.f,%.f)(%.2f°,%.2f°,%.2f°)(%.2f°,%.2f°,%.2f°)"
		, transform_.pos.x, transform_.pos.y, transform_.pos.z
		, UtilityMath::Rad2DegF(transform_.quaRot.x), UtilityMath::Rad2DegF(transform_.quaRot.y), UtilityMath::Rad2DegF(transform_.quaRot.z)
		, UtilityMath::Rad2DegF(transform_.quaRotLocal.x), UtilityMath::Rad2DegF(transform_.quaRotLocal.y), UtilityMath::Rad2DegF(transform_.quaRotLocal.z));

	actionController_->DrawDebug();

	animation_->DrawDebug();

	for (auto& collider : ownColliders_)
	{
		collider.second->Draw();
	}
#endif
}

void Player::ReleasePost(void)
{
}

void Player::ProcessMove(void)
{
	VECTOR dir = UtilityMath::VECTOR_ZERO;

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState = inputManager_.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	dir = inputManager_.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	if (inputManager_.IsNew(KEY_INPUT_W)) { dir.z += 1.0f; }
	if (inputManager_.IsNew(KEY_INPUT_S)) { dir.z += -1.0f; }
	if (inputManager_.IsNew(KEY_INPUT_A)) { dir.x += -1.0f; }
	if (inputManager_.IsNew(KEY_INPUT_D)) { dir.x += 1.0f; }

	if (actionController_->IsActiveAction())
	{
		movePow_ = UtilityMath::VECTOR_ZERO;
		return;
	}

	if (!UtilityMath::EqualsVZero(dir))
	{
		dir = UtilityMath::VNormalize(dir);
		movePow_ = UtilityMath::VECTOR_ZERO;

		if (!isJump_)
		{
			if (animType_ != ANIM_TYPE::THROW_LEFT
				&& animType_ != ANIM_TYPE::THROW_RIGHT)
			{
				PlayAnim(ANIM_TYPE::RUN);
			}
		}

		// カメラの方向で進行
		Quaternion cameraRot = sceneManager_.GetCamera()->GetQuaRotY();

		// 移動方向を取得
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);
		moveDir_.y = 0.0f;

		// 加速度に割り当て
		movePow_ = VScale(UtilityMath::VNormalize(moveDir_), moveSpeed_);
	}
	else
	{
		movePow_ = UtilityMath::VECTOR_ZERO;

		if (!isJump_ && animType_ != ANIM_TYPE::IDLE)
		{
			if (animType_ != ANIM_TYPE::THROW_LEFT &&
				animType_ != ANIM_TYPE::THROW_RIGHT)
			{
				PlayAnim(ANIM_TYPE::IDLE);
			}
		}
	}
}

void Player::ProcessJump(void)
{
	auto& input = InputManager::GetInstance();
	
	if (!actionController_->IsActiveAction())
	{
		bool isHitKeyNew = input.IsNew(KEY_INPUT_SPACE)
			|| input.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
				InputManager::JOYPAD_BTN::RB_BOTTOM);

		bool isHitTrg = input.IsTrgDown(KEY_INPUT_SPACE)
			|| input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
				InputManager::JOYPAD_BTN::RB_BOTTOM);

		if (isHitKeyNew && !isJump_)
		{
			if (isHitTrg)
			{
				// ジャンプ量の計算
				float jumpSpeed = (POW_JUMP_INIT * sceneManager_.GetDeltaTime());
				jumpPow_ = VScale(UtilityMath::DIR_UP, jumpSpeed);
			}

			// ジャンプの入力受付時間を減少
			stepJump_ += sceneManager_.GetDeltaTime();
			if (stepJump_ <= TIME_JUMP_INPUT)
			{
				// ジャンプ量の計算
				float jumpSpeed = POW_JUMP_KEEP * sceneManager_.GetDeltaTime();
				jumpPow_ = VAdd(jumpPow_, VScale(UtilityMath::DIR_UP, jumpSpeed));
			}
		}

		// ジャンプ
		if (isHitTrg && !isJump_)
		{
			isJump_ = true;
		}
	}

	// Y軸制限
	const float LIMIT_POS_Y = -1500.0f;
	if (transform_.pos.y < LIMIT_POS_Y)
	{
		transform_.pos.y = -(LIMIT_POS_Y);
	}
}

void Player::ProcessAttack(void)
{
	// 投げモーション終了時、行動が終了時
	if (animation_->IsEnd() && !actionController_->IsActiveAction())
	{
		if (animType_ == ANIM_TYPE::THROW_LEFT
			|| animType_ == ANIM_TYPE::THROW_RIGHT)
		{
			PlayAnim(ANIM_TYPE::IDLE);
		}
	}

	// 行動の更新
	actionController_->Update();

	// 行動中は処理終了
	if (actionController_->IsActiveAction()) { return; }


	if (inputManager_.IsTrgMouseLeft())
	{
		// 行動回数が最大値を超えた場合、０に戻す
		if (curAttackNum_ >= attackNumMax_)
		{
			curAttackNum_ = 0;
		}

		if (!actionController_->IsActiveInput())
		{
			CreateBullet();

			actionController_->Active(curAttackNum_);

			curAttackNum_++;

			ANIM_TYPE type = ((curAttackNum_ % 2 == 0) ? ANIM_TYPE::THROW_LEFT : ANIM_TYPE::THROW_RIGHT);
			PlayAnim(type, false);
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

void Player::CreateBullet(void)
{
	std::unique_ptr<PBulletBase> bullet;

	shotIndex_ = 0;

	for (auto& bullet : bullets_)
	{
		if (!bullet->IsAlive())
		{
			bullet->Release();
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

void Player::PlayAnim(ANIM_TYPE _type, bool _isLoop)
{
	animType_ = _type;

	animation_->Play(static_cast<int>(_type), _isLoop);
}

