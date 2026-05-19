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
#include "../Weapon/Bullet/Player/PBulletBig.h"
#include "../Weapon/Bullet/Player/PBulletRapidFire.h"


Player::Player(int _playerNo, BULLET_TYPE _playerType)
	: PlayerBase::PlayerBase(_playerNo, _playerType)
	, shadowHandle_(-1)
	, inputManager_(InputManager::GetInstance())
	, animType_(ANIM_TYPE::IDLE)	
	,  curAttackNum_(0)
{
	constexpr int BULLET_MAX = 3;
	attackNumMax_ = BULLET_MAX;

	constexpr float MOVE_SPEED = 5.0f;
	moveSpeed_ = MOVE_SPEED;
}


void Player::Load(void)
{
	transform_.modelId = resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_PLAYER_HUMAN);
}
void Player::InitAnimation(void)
{
	animation_ = std::make_unique<AnimationController>(transform_.modelId);
	animation_->AddExternal(static_cast<int>(ANIM_TYPE::IDLE)
		, 30.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_IDLE));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::RUN)
		, 30.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_RUN));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_LEFT)
		, 40.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_THROW_LEFT));

	animation_->AddExternal(static_cast<int>(ANIM_TYPE::THROW_RIGHT)
		, 40.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_THROW_RIGHT));

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
}
void Player::InitCollider(void)
{
	const VECTOR COL_CAPSULE_TOP = VScale(COL_CAPSULE_TOP_LOCAL_POS, transform_.scl.y);
	const VECTOR COL_CAPSULE_DOWN = VScale(COL_CAPSULE_DOWN_LOCAL_POS, transform_.scl.y);
	const float CAPSULE_RADIUS = (COL_CAPSULE_RADIUS * transform_.scl.y);

	ownColliders_.emplace(0
		, new ColliderCapsule(ColliderBase::TAG::PLAYER, &transform_, COL_CAPSULE_TOP, COL_CAPSULE_DOWN, CAPSULE_RADIUS));

	ownColliders_.at(0)->SetTriger(false);

	// 衝突判定マネージャに登録
	CollisionManager::GetInstance().RegisterActor(this);
}
void Player::InitPost(void)
{
	constexpr bool IS_RAPID_FIRE = false;
	actionController_ = std::make_unique<PActionController>(IS_RAPID_FIRE);

	curAttackNum_ = 0;

	constexpr float SHOT_TIME_INCREMENT = 0.5f; // 行動間隔上昇値
	constexpr float SHOT_TIME_ACTIVE = 1.5f; // 有効時間
	constexpr float SHOT_TIME_ACTION_ACTIVE = 0.1f; // 有効時間
	constexpr float SHOT_TIME_END = 0.5f; // 終了時間
	constexpr float SHOT_TIME_ACTIVE_INPUT = 0.35f; // 入力可能時間

	float timeActive, timeEnd, timeActionActive, timeInput;
	timeActive = SHOT_TIME_ACTIVE;
	timeEnd = SHOT_TIME_END;
	timeActionActive = SHOT_TIME_ACTION_ACTIVE;
	timeInput = SHOT_TIME_ACTIVE_INPUT;

	actionController_->SetAction(0, timeActive, timeEnd, timeActionActive
		, std::bind(&Player::ShotBullet, this), timeInput);

	timeActive += SHOT_TIME_INCREMENT;
	timeEnd += SHOT_TIME_INCREMENT;
	timeActionActive += SHOT_TIME_INCREMENT;
	timeInput += SHOT_TIME_INCREMENT;
	actionController_->SetAction(1, timeActive, timeEnd, timeActionActive
		, std::bind(&Player::ShotBullet, this), timeInput);

	timeActive += SHOT_TIME_INCREMENT * 2;
	timeEnd += SHOT_TIME_INCREMENT * 2;
	timeActionActive += SHOT_TIME_INCREMENT * 2;
	timeInput += SHOT_TIME_INCREMENT * 2;
	actionController_->SetAction(2, timeActive, timeEnd, timeActionActive
		, std::bind(&Player::ShotBullet, this), timeInput);
}


void Player::UpdateProcess(void)
{
	ProcessAttack();

	ProcessJump();

	// 移動操作
	ProcessMove();

	for (auto& bullet : bullets_)
	{
		bullet->Update();
	}

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

	DrawFormatString(0, 0, 0xffffff, "player:(%.f,%.f,%.f)(%.2f°,%.2f°,%.2f°)(%.2f°,%.2f°,%.2f°)"
		, transform_.pos.x, transform_.pos.y, transform_.pos.z
		, UtilityMath::Rad2DegF(transform_.quaRot.x), UtilityMath::Rad2DegF(transform_.quaRot.y), UtilityMath::Rad2DegF(transform_.quaRot.z)
		, UtilityMath::Rad2DegF(transform_.quaRotLocal.x), UtilityMath::Rad2DegF(transform_.quaRotLocal.y), UtilityMath::Rad2DegF(transform_.quaRotLocal.z));

	actionController_->DrawDebug();

	for (auto& collider : ownColliders_)
	{
		collider.second->Draw();
	}
#endif
}

void Player::Release(void)
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

		if (!isJump_)
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

	bool isHitKeyNew = input.IsNew(KEY_INPUT_SPACE)
		|| input.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::RB_BOTTOM);
	if (isHitKeyNew)
	{
		// ジャンプの入力受付時間を減少
		stepJump_ += sceneManager_.GetDeltaTime();
		if (stepJump_ <= TIME_JUMP_INPUT)
		{
			// ジャンプ量の計算
			float jumpSpeed = POW_JUMP_KEEP * sceneManager_.GetDeltaTime();
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
		float jumpSpeed = (POW_JUMP_INIT * sceneManager_.GetDeltaTime());
		jumpPow_ = VScale(UtilityMath::DIR_UP, jumpSpeed);

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

void Player::ProcessAttack(void)
{
	if (animation_->IsEnd()
		&& actionController_->GetActionState() == PActionController::PACTION_STATE::NONE)
	{
		if (animType_ != ANIM_TYPE::THROW_LEFT &&
			animType_ != ANIM_TYPE::THROW_RIGHT)
		{
			PlayAnim(ANIM_TYPE::IDLE);
		}
	}

	actionController_->Update();

	if (actionController_->GetActionState() != PActionController::PACTION_STATE::NONE) { return; }

	if (inputManager_.IsTrgMouseLeft())
	{
		if (curAttackNum_ >= attackNumMax_)
		{
			if (actionController_->GetActionState() == PActionController::PACTION_STATE::NONE)
			{
				curAttackNum_ = 0;
				PlayAnim(ANIM_TYPE::IDLE);
			}
			return;
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

			bullet->Create(transform_.pos, curAttackNum_);
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

	bullets_.emplace_back(std::move(bullet));
}

void Player::ShotBullet(void)
{
	bullets_[shotIndex_]->Shot(transform_.GetForward(), transform_.quaRot);
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

