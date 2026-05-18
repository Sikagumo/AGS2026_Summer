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
#include "../../../Collider/ColliderBase.h"
#include "../Weapon/Bullet/Player/PBulletBig.h"
#include "../Weapon/Bullet/Player/PBulletRapidFire.h"


Player::Player(int _playerNo, BULLET_TYPE _playerType)
	: PlayerBase::PlayerBase(_playerNo, _playerType)
	, shadowHandle_(-1)
	, inputManager_(InputManager::GetInstance())
	
{
	constexpr float MOVE_SPEED = 5.0f;
	moveSpeed_ = MOVE_SPEED;
}


void Player::InitLoad(void)
{
	transform_.modelId = resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_PLAYER_HUMAN);
}
void Player::InitAnimation(void)
{
	animation_ = std::make_unique<AnimationController>(transform_.modelId);
	animation_->AddExternal(0, 30.0f, resourceManager_.LoadHandleId(ResourceManager::SRC::ANIM_IDLE));
	animation_->Play(0, true);
}
void Player::InitTransform(void)
{
	constexpr float MODEL_SCALE = 0.75f;

	transform_.InitTransform(MODEL_SCALE
		, Quaternion::Identity()
		, Quaternion::Identity()
		, UtilityMath::VECTOR_ZERO);
}
void Player::InitCollider(void)
{

}
void Player::InitPost(void)
{
}


void Player::UpdateProcess(void)
{
	ProcessAttack();

	ProcessJump();

	// 移動操作
	ProcessMove();

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
#endif
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
		dir = UtilityMath::VNormalize(dir);
		//movePow_ = UtilityMath::VECTOR_ZERO;

		if (!isJump_)
		{
			//PlayAnim(ANIM_TYPE::RUN);
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
		//PlayAnim(ANIM_TYPE::JUMP, false);
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
	if (inputManager_.IsTrgMouseLeft())
	{
		std::unique_ptr<PBulletBase> bullet;

		switch (bulletType_)
		{
			case BULLET_TYPE::BIG:
				bullet = std::make_unique<PBulletBig>();
			break;

			default:
			break;

		}
		bullet->Init();

		bullet->CreateShot(transform_.pos, transform_.GetForward(), Quaternion::Identity());

		bullets_.emplace_back(std::move(bullet));
	}

	for (auto& bullet : bullets_)
	{
		bullet->Update();
	}
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
