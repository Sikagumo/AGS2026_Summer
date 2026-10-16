#include <DxLib.h>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collider/ColliderCapsule.h"
#include "../../../../../Collision/CollisionController.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"
#include "../../../../ActorBase.h"
#include "BBulletMissile.h"

BBulletMissile::BBulletMissile(void)
	: fallingHandle_(-1)
	, isUp_(true)
	, isAttack_(false)
	, playerPos_({ 0.0f, 0.0f, 0.0f })
	, attackCount_(0)
	, maxPos_(0.0f)
{
	const VECTOR INIT_NORM = VGet(0.0f, 1.0f, 0.0f);
	const COLOR_U8 INIT_DIFUSECOLOR = GetColorU8(255, 255, 255, 255);

	// 警告エリア用ポリゴンの4頂点に対して初期法線とディフューズ色を設定する
	for (int i = 0; i < 4; ++i)
	{
		imageVertex_[i].norm = INIT_NORM;
		imageVertex_[i].dif = INIT_DIFUSECOLOR;
	}

	// UV座標の割り当て
	imageVertex_[LEFT_BACK].u = 0.0f;    imageVertex_[LEFT_BACK].v = 1.0f;
	imageVertex_[LEFT_FORWARD].u = 0.0f; imageVertex_[LEFT_FORWARD].v = 0.0f;
	imageVertex_[RIGHT_BACK].u = 1.0f;   imageVertex_[RIGHT_BACK].v = 1.0f;
	imageVertex_[RIGHT_FORWARD].u = 1.0f; imageVertex_[RIGHT_FORWARD].v = 0.0f;
}

BBulletMissile::~BBulletMissile(void)
{
}

void BBulletMissile::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_MISSILE));
	fallingHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_FALLING_POINT);
}

void BBulletMissile::ReleasePost(void)
{
}

void BBulletMissile::InitTransform(void)
{
	transform_.scl = MISSILE_SCALE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y);
	transform_.quaRotLocal = Quaternion::Mult(transform_.quaRotLocal, Quaternion::AngleAxis(UtilityMath::Deg2RadF(-90.0f), UtilityMath::AXIS_X));
	transform_.Update();
}

void BBulletMissile::InitCollider(void)
{
	ColliderSphere* colHitSphere = new ColliderSphere(
		ColliderBase::TAG::MISSILE_ATTACK, &transform_, COLLIDER_OFFSET, radius_ * ATTACK_COLLIDER_RADIUS_RATE);
	ownColliders_[static_cast<int>(ColliderBase::TAG::MISSILE_ATTACK)].push_back(colHitSphere);

	ColliderSphere* colPushSphere = new ColliderSphere(
		ColliderBase::TAG::MISSILE_PUSH, &transform_, COLLIDER_OFFSET, radius_ * PUSH_COLLIDER_RADIUS_RATE);
	ownColliders_[static_cast<int>(ColliderBase::TAG::MISSILE_PUSH)].push_back(colPushSphere);

	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MISSILE_ATTACK, false);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MISSILE_PUSH, false);
}

void BBulletMissile::InitAnimation(void)
{
}

void BBulletMissile::InitPost(void)
{
	isUp_ = true;
	isAlive_ = true;
	attackCount_ = 0;
}

void BBulletMissile::UpdateProcess(void)
{
	// 弾が死亡（非非生存）状態の場合は更新処理を行わずスキップする
	if (!isAlive_) return;

	// 上昇中の場合は上昇移動処理を実行する
	if (isUp_)
	{
		MoveUp();
	}
	// 降下中の場合は降下移動処理を実行する
	else
	{
		MoveDown();
	}

	// 着地後の攻撃フラグが有効な場合は攻撃持続処理を実行する
	if (isAttack_)
	{
		Attack();
	}
}

void BBulletMissile::UpdateProcessPost(void)
{
}

void BBulletMissile::DrawPre(void)
{
	// 生存している場合のみミサイルの3Dモデルを描画する
	if (isAlive_)
	{
		MV1DrawModel(transform_.modelId);
	}

	// 降下フェーズ（isUp_がfalse）に入っている場合は地面に着弾警告描画を行う
	if (!isUp_)
	{
		DrawAreaAlert();
	}
}

void BBulletMissile::MoveUp(void)
{
	transform_.pos.y += MOVE_UP_SPEED;

	// 上昇限界高度を超えた場合にプレイヤーの頭上へワープして降下準備を行う
	if (transform_.pos.y > maxPos_)
	{
		isUp_ = false;
		// プレイヤーの頭上へワープ
		transform_.pos = playerPos_;
		transform_.pos.y = SPAWN_HEIGHT_Y;
		transform_.quaRotLocal = Quaternion::Mult(transform_.quaRotLocal, Quaternion::AngleAxis(UtilityMath::Deg2RadF(180.0f), UtilityMath::AXIS_X));
	}
}

void BBulletMissile::MoveDown(void)
{
	transform_.pos.y -= MOVE_DOWN_SPEED;

	// 地面に到達した場合に位置を固定して攻撃判定とエフェクトを発生させる
	if (transform_.pos.y <= GROUND_POS_Y)
	{
		transform_.pos.y = GROUND_POS_Y; // 地上に位置固定

		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MISSILE_ATTACK, true);
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MISSILE_PUSH, true);
		EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_MISSILE, transform_.pos, EFFECT_ZERO_ROT, MISSILE_EFFECT_SCALE, MISSILE_EFFECT_SPEED, this);
		isAttack_ = true;
	}
}

void BBulletMissile::Attack(void)
{
	attackCount_++;
	// 攻撃持続フレーム数が上限に達した場合に各種判定とエフェクトを停止しアクター登録を解除する
	if (attackCount_ >= MAX_ATTACK_COUNT)
	{
		attackCount_ = 0;
		isAttack_ = false;
		isAlive_ = false;
		isUp_ = true;

		EffectManager::GetInstance().Stop(EffectManager::EFFECT::EFFECT_MISSILE, this);

		// 当たり判定の無効化とアクター登録解除（アクター自体の解除は1回のみ行う）
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MISSILE_ATTACK, false);
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::MISSILE_PUSH, false);
		CollisionController::GetInstance().UnregisterActor(this);
	}
}

void BBulletMissile::DrawAreaAlert(void)
{
	const float SHADOW_SIZE = radius_;   // 影の基本サイズ（半径）
	float shadowY = SHADOW_POS_Y;

	// キャラクターの現在位置に合わせて頂点座標を更新
	imageVertex_[LEFT_BACK].pos = VGet(transform_.pos.x - SHADOW_SIZE, shadowY, transform_.pos.z - SHADOW_SIZE);
	imageVertex_[LEFT_FORWARD].pos = VGet(transform_.pos.x - SHADOW_SIZE, shadowY, transform_.pos.z + SHADOW_SIZE);
	imageVertex_[RIGHT_BACK].pos = VGet(transform_.pos.x + SHADOW_SIZE, shadowY, transform_.pos.z - SHADOW_SIZE);
	imageVertex_[RIGHT_FORWARD].pos = VGet(transform_.pos.x + SHADOW_SIZE, shadowY, transform_.pos.z + SHADOW_SIZE);

	// 警告表示ポリゴンの4つの頂点に透明度（アルファ値）を設定する
	for (int i = 0; i < 4; ++i)
	{
		imageVertex_[i].dif.a = ALERT_ALPHA;
	}

	// 描画環境のセットアップ
	SetUseLighting(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(FALSE);
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALERT_ALPHA);

	// インデックス配列の定義
	const int POINT_CNT = 6;
	const int TRIANGLE_CNT = 2;
	WORD index[POINT_CNT];

	index[0] = LEFT_BACK;    index[1] = LEFT_FORWARD; index[2] = RIGHT_BACK;
	index[3] = RIGHT_FORWARD; index[4] = RIGHT_BACK;   index[5] = LEFT_FORWARD;

	// 描画
	DrawPolygonIndexed3D(imageVertex_, 4, index, TRIANGLE_CNT, fallingHandle_, TRUE);

	// グラフィック設定の復元
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
}