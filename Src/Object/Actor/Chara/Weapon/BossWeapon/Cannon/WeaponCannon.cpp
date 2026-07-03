#include <DxLib.h>
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderCapsule.h"
#include "../../../../../Collider/ColliderLine.h"
#include "../../../../../Collision/CollisionController.h"
#include "../../Bullet/Boss/BBulletCannon.h"
#include "WeaponCannon.h"

WeaponCannon::WeaponCannon():count_(0)
{
	
}

void WeaponCannon::ReleasePost(void)
{
}

void WeaponCannon::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos)
{
	bone_.id = _id;
	bone_.transform = _trans;
	bone_.playerPos = _playerPos;
	tag_ = _tag;
}

const VECTOR WeaponCannon::GetPos(void) const
{
	
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(localPos_);

	// 位置を加算して最終的なワールド座標にする
	return VAdd(transform_.pos, localRotPos);
}

void WeaponCannon::Load(void)
{
	//transform_.SetModel(ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_CN));
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_CN));
}


void WeaponCannon::InitTransform(void)
{
	
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponCannon::InitCollider(void)
{
	ColliderLine* lineCollider = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::TAG::STAGE), std::vector<ColliderBase*>{ lineCollider });


	ColliderCapsule* capsuleCollider = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	capsuleCollider->SetTriger(false);
	ownColliders_.emplace(static_cast<int>(tag_), std::vector<ColliderBase*>{ capsuleCollider });


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
	// HPがなくなったら死亡処理（左右共通）
	if (hp_ <= 0 && isAlive_)
	{
		ChangeState(static_cast<int>(STATE::END));
	}

	for (std::shared_ptr<BBulletBase> bullet : bullets_)
	{
		bullet->Update();

	}

	stateUpdate_();

}

void WeaponCannon::UpdateProcessPost(void)
{
}



void WeaponCannon::DrawPre(void)
{
	for (std::shared_ptr<BBulletBase> bullet : bullets_)
	{
		if (bullet->GetIsAlive() == true)
		{
			bullet->Draw();
		}
	}
#ifdef _DEBUG
<<<<<<< HEAD

	DrawFormatString(10, 420, 0xffffff, "count:%d", count_);

	for (auto& col : ownColliders_)
=======
	for (auto& [id, colliderVector] : ownColliders_)
>>>>>>> Scene
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


void WeaponCannon::LookPlayer(void)
{
	//照準調整
	VECTOR targetPos = bone_.playerPos;
	targetPos.y += DOUN_ROCK;

	VECTOR moveDir = VSub(targetPos, transform_.pos);
	moveDir = VNorm(moveDir);
	//Y軸計算
	float targetAngleYRad = atan2(moveDir.x, moveDir.z);
	Quaternion rotY = Quaternion::AngleAxis(targetAngleYRad, UtilityMath::AXIS_Y);
	//X軸計算と制限
	float horizontalDistance = sqrtf(moveDir.z * moveDir.z + moveDir.x * moveDir.x);
	float targetAngleXRad = atan2(moveDir.y, horizontalDistance);
	
	float targetAngleXDeg = UtilityMath::Rad2DegF(targetAngleXRad);
	targetAngleXDeg = std::clamp(targetAngleXDeg, LIMIT_MIN_ANGLE, LIMIT_MAX_ANGLE);

	float clampedAngleXRad = UtilityMath::Deg2RadF(targetAngleXDeg);
	Quaternion rotX = Quaternion::AngleAxis(-clampedAngleXRad, UtilityMath::AXIS_X);
	//合成
	Quaternion localRot = Quaternion::Mult(rotY, rotX); 
	transform_.quaRot = localRot;

	Quaternion bulletRot = transform_.quaRot;

	//発射方向の計算
	VECTOR forward = VGet(0.0f, 0.0f, 1.0f);
	MATRIX rotationMatrix = Quaternion::ToMatrix(bulletRot);
	VECTOR bulletDir = VTransformSR(forward, rotationMatrix);
	bulletDir_ = VNorm(bulletDir);
}


void WeaponCannon::ChangeState(STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	ChangeState(state);
}

void WeaponCannon::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void WeaponCannon::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&WeaponCannon::UpdateIdle, this);
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
}

void WeaponCannon::UpdateAttack(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	LookPlayer();
	/*SoundManager::GetInstance().Set3DPosition(SoundManager::SOUND::SE_MG_FIRE, transform_.pos);*/

	CreateBullets();
	count_++;

	isAttack_ = false;
	/*SoundManager::GetInstance().Stop(SoundManager::SOUND::SE_MG_FIRE);*/
	ChangeState(STATE::IDLE);

}

void WeaponCannon::UpdateIdle(void)
{
	LookPlayer();
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
}

void WeaponCannon::UpdateEnd(void)
{
}

void WeaponCannon::CreateBullets(void)
{
	std::shared_ptr<BBulletBase> bullet = GetValidBullet();
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(CAPSULE_START_POS);

	// 位置を加算して最終的なワールド座標にする
	VECTOR bulletpos = VAdd(transform_.pos, localRotPos);

	bullet->CreateBullets(bulletpos, bulletDir_, 12.0f);
	bullet->Init();
	bullet->SetTransform(transform_);
}

std::shared_ptr<BBulletBase> WeaponCannon::GetValidBullet(void)
{
	for (auto& bullet : bullets_) {
		if (!bullet->GetIsAlive()) return bullet;
	}
	std::shared_ptr<BBulletBase> bullet = std::make_shared<BBulletCannon>();
	bullets_.emplace_back(bullet);
	bullet->Load();
	return bullet;
}
