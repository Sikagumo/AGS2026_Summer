#include "WeaponCannon.h"
#include <DxLib.h>
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderCapsule.h"
#include "../../../../../Collider/ColliderLine.h"
#include "../../../../../Collision/CollisionController.h"
#include "../../Bullet/Boss/BBulletCannon.h"


WeaponCannon::WeaponCannon(void)
	: isAttack_(false)
	, bulletDir_(VGet(0.0f, 0.0f, 0.0f))
	, look_(0)
	, count_(0)
{
}

void WeaponCannon::ReleasePost(void)
{
}

void WeaponCannon::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_CN));
}

void WeaponCannon::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::GetIdentity();
	transform_.quaRotLocal =
		Quaternion::Multiply(transform_.quaRotLocal,
			Quaternion::CreateAngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponCannon::InitCollider(void)
{
	ColliderLine* lineCollider = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::STAGE)].push_back(lineCollider);

	ColliderCapsule* capsuleCollider = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	capsuleCollider->SetTriger(false);
	ownColliders_[static_cast<int>(tag_)].push_back(capsuleCollider);

	CollisionController::GetInstance().RegisterActor(this);
}

void WeaponCannon::InitAnimation(void)
{
}

void WeaponCannon::InitPost(void)
{
	
	isAlive_ = true;
	localPos_ = LINE_START_POS;
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&WeaponCannon::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&WeaponCannon::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&WeaponCannon::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);
}

void WeaponCannon::UpdateProcess(void)
{
	// HPがなくなったら死亡処理
	if (hp_ <= 0 && isAlive_)
	{
		ChangeState(static_cast<int>(STATE::END));
	}

	for (const std::shared_ptr<BBulletBase>& bullet : bullets_)
	{
		bullet->Update();
	}

	if (stateUpdate_)
	{
		stateUpdate_();
	}
}

void WeaponCannon::UpdateProcessPost(void)
{
}

void WeaponCannon::DrawPre(void)
{
	for (const std::shared_ptr<BBulletBase>& bullet : bullets_)
	{
		if (bullet->GetIsAlive())
		{
			bullet->Draw();
		}
	}
}

void WeaponCannon::LookPlayer(void)
{
	// 照準調整
	VECTOR targetPos = bone_.playerPos;
	targetPos.y += DOWN_LOCK;

	VECTOR moveDir = VSub(targetPos, transform_.pos);
	moveDir = VNorm(moveDir);

	// Y軸計算
	float targetAngleYRad = atan2(moveDir.x, moveDir.z);
	Quaternion rotY = Quaternion::CreateAngleAxis(targetAngleYRad, UtilityMath::AXIS_Y);

	// X軸計算と制限
	float horizontalDistance = sqrtf(moveDir.z * moveDir.z + moveDir.x * moveDir.x);
	float targetAngleXRad = atan2(moveDir.y, horizontalDistance);

	float targetAngleXDeg = UtilityMath::Rad2DegF(targetAngleXRad);
	targetAngleXDeg = std::clamp(targetAngleXDeg, LIMIT_MIN_ANGLE, LIMIT_MAX_ANGLE);

	float clampedAngleXRad = UtilityMath::Deg2RadF(targetAngleXDeg);
	Quaternion rotX = Quaternion::CreateAngleAxis(-clampedAngleXRad, UtilityMath::AXIS_X);

	// 合成
	Quaternion localRot = Quaternion::Multiply(rotY, rotX);
	transform_.quaRot = localRot;

	Quaternion bulletRot = transform_.quaRot;

	// 発射方向の計算
	VECTOR forward = VGet(0.0f, 0.0f, 1.0f);
	MATRIX rotationMatrix = Quaternion::ConvertToMatrix(bulletRot);
	VECTOR bulletDir = VTransformSR(forward, rotationMatrix);
	bulletDir_ = VNorm(bulletDir);
}

void WeaponCannon::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&WeaponCannon::UpdateIdle, this);
}

void WeaponCannon::ChangeStateAttackWindup(void)
{
}

void WeaponCannon::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&WeaponCannon::UpdateAttack, this);
	isAttack_ = true;
}

void WeaponCannon::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&WeaponCannon::UpdateEnd, this);
	isAlive_ = false;
	CollisionController::GetInstance().SetCollisionActive(this, tag_, false);
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
	jumpPow_ = JUMP_POW;
	isJump_ = true;
	moveDir_ = VSub(transform_.pos, bone_.transform.pos);
	moveDir_.y = 0.0f;
	moveDir_ = VNorm(moveDir_);
}

void WeaponCannon::UpdateAttackWindup(void)
{
}

void WeaponCannon::UpdateAttack(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	LookPlayer();

	CreateBullets();
	count_++;

	isAttack_ = false;
	ChangeState(STATE::IDLE);
}

void WeaponCannon::UpdateIdle(void)
{
	LookPlayer();
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
}

void WeaponCannon::CreateBullets(void)
{
	std::shared_ptr<BBulletBase> bullet = GetValidBullet();

	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.RotatePosition(CAPSULE_START_POS);

	// 位置を加算して最終的なワールド座標にする
	VECTOR bulletPos = VAdd(transform_.pos, localRotPos);

	bullet->CreateBullets(bulletPos, bulletDir_, BULLET_HIT_SIZE);
	bullet->Init();
	bullet->SetTransform(transform_);
}

std::shared_ptr<BBulletBase> WeaponCannon::GetValidBullet(void)
{
	for (auto& bullet : bullets_)
	{
		if (!bullet->GetIsAlive()) return bullet;
	}
	std::shared_ptr<BBulletBase> bullet = std::make_shared<BBulletCannon>();
	bullets_.emplace_back(bullet);
	bullet->Load();
	return bullet;
}