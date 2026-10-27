#include "PBulletBase.h"
#include <DxLib.h>
#include <algorithm>
#include <array>
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../../Manager/System/TimeManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"
#include "../../../../../../Application.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collision/CollisionController.h"

namespace
{
	// 弾が着弾(爆発)する対象タグ。毎フレーム生成しないよう静的に保持
	constexpr std::array<ColliderBase::TAG, 10> HIT_TARGET_TAGS =
	{
		ColliderBase::TAG::BOSS, ColliderBase::TAG::ENEMY, ColliderBase::TAG::ENEMYROBO,
		ColliderBase::TAG::WEAPON_CANNON_L, ColliderBase::TAG::WEAPON_CANNON_R,
		ColliderBase::TAG::WEAPON_MG_L, ColliderBase::TAG::WEAPON_MG_R,
		ColliderBase::TAG::WEAPON_MP_L, ColliderBase::TAG::WEAPON_MP_R,
		ColliderBase::TAG::WEAPON_RG
	};

	// 描画色
	constexpr unsigned int COLOR_BULLET = 0xffffff;
	constexpr unsigned int COLOR_SPECULAR = 0xffffff;

#ifdef _DEBUG
	constexpr unsigned int COLOR_BLAST_DEBUG = 0xff0000;
#endif
}

PBulletBase::PBulletBase(int _shotType, bool _isGravity)
	: ActorBase::ActorBase()
	, shotType_(_shotType)
	, bulletState_(BULLET_STATE::INACTIVE)
	, radiusBullet_(0.0f)
	, radiusBlast_(0.0f)
	, shotSpeedXZ_(0.0f)
	, shotSpeedY_(0.0f)
	, throwPow_(UtilityMath::VECTOR_ZERO)
	, throwDir_(UtilityMath::VECTOR_ZERO)
	, curGravityPow_(0.0f)
	, aliveTime_(0.0f)
	, shotCnt_(0)
	, isVisible_(false)
	, power_(0)
	, activePowerBullet_(0)
	, activePowerBlast_(0)
	, isActiveDestroy_(false)
	, IS_GRAVITY(_isGravity)
{
}

PBulletBase::~PBulletBase(void)
{
	// メモリ解放
	Release();
}

void PBulletBase::InitCollider(void)
{
	// 再初期化時、処理を終了
	if (!ownColliders_.empty()) { return; }

	// 衝突判定マネージャに登録
	ColliderSphere* bullet = new ColliderSphere(ColliderBase::TAG::PLAYER_BULLET, &transform_
									, UtilityMath::VECTOR_ZERO, radiusBullet_);
	ownColliders_[static_cast<int>(COLLISION_TYPE::BULLET)]
		.emplace_back(bullet);

	ColliderSphere* blast = new ColliderSphere(ColliderBase::TAG::PLAYER_BLAST, &transform_
									, UtilityMath::VECTOR_ZERO, radiusBullet_);
	ownColliders_[static_cast<int>(COLLISION_TYPE::BLAST)]
		.emplace_back(blast);

	// 当たり判定の割り当て
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, true);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
}

void PBulletBase::InitPost(void)
{
	// 可視化
	isVisible_ = true;

	// 無効状態化
	bulletState_ = BULLET_STATE::INACTIVE;

	// 攻撃力無効化
	activePowerBullet_ = 0;
	activePowerBlast_  = 0;

	// 消滅状態化
	isActiveDestroy_ = false;

	// 各弾の初期化
	SetParam();

	// 爆発の半径割り当て・無効化
	radiusBlast_ = 0.0f;
	ownColliders_.at(static_cast<int>(COLLISION_TYPE::BLAST))
		.at(0)->SetRadius(radiusBlast_);
}

void PBulletBase::Update(void)
{
	if (bulletState_ == BULLET_STATE::SHOT)
	{
		UpdateShot();
	}

	// 弾別の更新処理
	UpdatePost();


	if (bulletState_ == BULLET_STATE::INACTIVE
		|| bulletState_ == BULLET_STATE::BLAST) {
		return;
	}

	// 発射中以外は着弾判定を行わない
	if (bulletState_ != BULLET_STATE::SHOT) { return; }

	if (IsHitTarget() || IsHitTerrain())
	{
		// 衝突時、着弾処理
		BlastAction();
	}
}

void PBulletBase::UpdateShot(void)
{
	const float delta = timeManager_.GetDeltaTime();

	VECTOR pos = throwPow_;

	if (IS_GRAVITY)
	{
		// 重力加算処理
		curGravityPow_ += (Application::GetInstance().GetGravityPow() * delta);
		pos.y -= curGravityPow_;
	}

	transform_.Translate(pos);

	if (aliveTime_ > 0.0f)
	{
		// 生存時間減少
		aliveTime_ -= delta;
	}
	else
	{
		// 生存時間無効時、爆破処理
		BlastAction();
	}
}

bool PBulletBase::IsHitTarget(void)
{
	auto& collision = CollisionController::GetInstance();

	// いずれかの衝突判定に衝突時、true
	return std::ranges::any_of(HIT_TARGET_TAGS
		, [this, &collision](ColliderBase::TAG _tag)
		{
			return collision.IsActorCollidingWithTag(this, _tag);
		});
}

bool PBulletBase::IsHitTerrain(void)
{
	// 下向きの即着弾は重力のある弾だけ。重力なし(連射・拡散)はステージ衝突か生存時間切れで着弾する
	return (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::STAGE)
		|| (IS_GRAVITY && throwPow_.y < 0.0f));
}

void PBulletBase::Draw(void)
{
	if (isVisible_)
	{
		DrawSphere3D(transform_.pos, radiusBullet_, SPHERE_DIV
			, COLOR_BULLET, COLOR_SPECULAR, true);
	}

#ifdef _DEBUG
	if (bulletState_ == BULLET_STATE::BLAST)
	{
		DrawSphere3D(transform_.pos, radiusBlast_, SPHERE_DIV
			, COLOR_BLAST_DEBUG, COLOR_SPECULAR, false);
	}
#endif
}

void PBulletBase::Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt)
{
	// 現在の発射数
	shotCnt_ = _shotCnt;

	// 弾再初期化
	SetParam();

	// 無効状態化
	bulletState_ = BULLET_STATE::INACTIVE;

	// 重力初期化
	curGravityPow_ = 0.0f;

	// 発射方向割り当て
	throwDir_ = _throwDir;
	transform_.pos = VAdd(_pos, VScale(_throwDir, radiusBullet_));
	transform_.Update();

	// 弾可視化
	isVisible_ = true;
}

void PBulletBase::Shot(const VECTOR& _shotDir)
{
	// 発射方向(未指定時は投げる方向)
	const VECTOR shotDir = UtilityMath::VNormalize(
		UtilityMath::EqualsVZero(_shotDir) ? throwDir_ : _shotDir);

	// 全方向に平面速度を適用
	throwPow_ = VScale(shotDir, shotSpeedXZ_);

	if (IS_GRAVITY)
	{
		// 重力有効時、縦方向だけ縦速度を使用
		throwPow_.y = (shotDir.y * shotSpeedY_);
	}

	bulletState_ = BULLET_STATE::SHOT;

	curGravityPow_ = 0.0f;

	transform_.Update();

	// 当たり判定登録
	ownColliders_.at(static_cast<int>(COLLISION_TYPE::BULLET)).at(0)->SetRadius(radiusBullet_);
	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BULLET, true);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::PLAYER_BLAST, false);
}

bool PBulletBase::IsAlive(void) const
{
	return(bulletState_ != BULLET_STATE::INACTIVE && !isActiveDestroy_);
}

void PBulletBase::SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir)
{
	// 追従位置割り当て
	transform_.pos = VAdd(_pos, VScale(_offsetDir, radiusBullet_));
}

void PBulletBase::SetColliderActive(ColliderBase::TAG _tag, bool _isActive)
{
	CollisionController::GetInstance().SetCollisionActive(this, _tag, _isActive);
}

void PBulletBase::SetColliderRadius(COLLISION_TYPE _type, float _radius)
{
	ownColliders_.at(static_cast<int>(_type)).at(0)->SetRadius(_radius);
}

void PBulletBase::BeginBlast(void)
{
	bulletState_ = BULLET_STATE::BLAST;
	isVisible_ = false;
	SetColliderActive(ColliderBase::TAG::PLAYER_BULLET, false);
}

void PBulletBase::ActivateBlastCollider(float _radius)
{
	radiusBlast_ = _radius;
	SetColliderRadius(COLLISION_TYPE::BLAST, radiusBlast_);
	SetColliderActive(ColliderBase::TAG::PLAYER_BLAST, true);
}
