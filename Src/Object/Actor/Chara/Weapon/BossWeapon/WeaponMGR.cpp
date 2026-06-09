#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderCapsule.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "../Bullet/Boss/BBulletMG.h"
#include "WeaponMGR.h"

WeaponMGR::WeaponMGR()
	:bulletDir_{ 0.0f,0.0f,0.0f }
	, bulletCount_(MAX_BULLET_COUNT)
	, muzzlePos_{ 0.0f,0.0f,0.0f }
	, muzzleCount_(0)
{
	
}



void WeaponMGR::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos)
{
	bone_.id = _id;
	bone_.transform = _trans;
	bone_.playerPos = _playerPos;
	tag_ = _tag;
}

const VECTOR WeaponMGR::GetPos(void) const
{
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(localPos_);

	// 位置を加算して最終的なワールド座標にする
	return VAdd(transform_.pos, localRotPos);
}

void WeaponMGR::Load(void)
{
	transform_.SetModel(resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_MG_R));
}



void WeaponMGR::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMGR::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);


	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
	colCapsule->SetTriger(false);

	CollisionManager::GetInstance().RegisterActor(this);
}

void WeaponMGR::InitAnimation(void)
{
}

void WeaponMGR::InitPost(void)
{
	isAlive_ = true;
	localPos_ = LINE_START_POS;
	for (int i = 0; i < MUZZLE_MAX_COUNT; ++i)
	{
		muzzlePos_[i] = MUZZLE_POS[i];
	}
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&WeaponMGR::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&WeaponMGR::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&WeaponMGR::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);
}

void WeaponMGR::UpdateProcess(void)
{
	stateUpdate_();

	for (std::shared_ptr<BBulletBase> shot : bullets_)
	{
		shot->Update();
	}

	if (hp_ <= 0)
	{
		ChangeState(STATE::END);
	}
}

void WeaponMGR::UpdateProcessPost(void)
{
	
}



void WeaponMGR::DrawPre(void)
{


	for (std::shared_ptr<BBulletBase> shot : bullets_)
	{
		shot->Draw();
	}

#ifdef _DEBUG
	if (isAlive_)
	{


		for (auto& col : ownColliders_)
		{
			col.second->Draw();
		}

		
	}

	DrawFormatString(10, 300, 0xffffff, "MGR_HP:%d", hp_);

	

#endif
}

void WeaponMGR::LookPlayer(void)
{
	VECTOR moveDir;

	// プレイヤーの位置に向かう方向を計算
	moveDir = VSub(bone_.playerPos, transform_.pos);

	moveDir = VNorm(moveDir);

	float horizontalDistance = sqrtf(moveDir.z * moveDir.z + moveDir.x * moveDir.x);

	float targetAngle = atan2(moveDir.y, horizontalDistance);

	Quaternion weaponPitch = Quaternion::AngleAxis(-targetAngle, UtilityMath::AXIS_X);

	transform_.quaRot = Quaternion::Mult(bone_.transform.quaRot, weaponPitch);

	Quaternion bulletRot = transform_.quaRot;

	//発射向きのランダム化
	float randDirX = UtilityMath::RandRangeF(-5, 5);
	bulletRot = Quaternion::Mult(bulletRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(randDirX), UtilityMath::AXIS_X));
	float randDirY = UtilityMath::RandRangeF(-5, 5);
	bulletRot = Quaternion::Mult(bulletRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(randDirY), UtilityMath::AXIS_Y));


	//発射方向の計算
	VECTOR forward = VGet(0.0f, 0.0f, 1.0f);
	MATRIX rotationMatrix = Quaternion::ToMatrix(bulletRot);
	VECTOR bulletDir = VTransformSR(forward, rotationMatrix);
	bulletDir_ = VNorm(bulletDir);
}
void WeaponMGR::ChangeState(STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	WeaponMGR::ChangeState(state);
}
void WeaponMGR::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void WeaponMGR::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&WeaponMGR::UpdateIdle, this);
}

void WeaponMGR::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&WeaponMGR::UpdateAttack, this);
	bulletCount_ = MAX_BULLET_COUNT;
}

void WeaponMGR::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&WeaponMGR::UpdateEnd, this);
	isAlive_ = false;
	CollisionManager::GetInstance().SetCollisionActive(this, tag_, false);
}

void WeaponMGR::UpdateAttack(void)
{
	bulletCount_--;
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	LookPlayer();

	if (bulletCount_ >= 0)
	{
		CreateBullets();
	}
	if (bulletCount_ <= 0)
	{
		ChangeState(STATE::IDLE);
	}
	
}

void WeaponMGR::UpdateIdle(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	LookPlayer();
}

void WeaponMGR::UpdateEnd(void)
{
}

std::shared_ptr<BBulletBase> WeaponMGR::GetValidBullet(void)
{
	size_t bulletCount = bullets_.size();
	for (size_t i = 0; i < bulletCount; i++)
	{
		if (!bullets_[i]->GetIsAlive())
		{
			return bullets_[i];
		}
	}


	std::shared_ptr<BBulletBase> bullet = std::make_shared<BBulletMG>(transform_);
	// 可変長配列に追加
	bullets_.push_back(bullet);


	return bullet;
}

void WeaponMGR::CreateBullets(void)
{
	muzzleCount_++;
	if (muzzleCount_ > MUZZLE_MAX_COUNT - 1)
	{
		muzzleCount_ = 0;
	}

	std::shared_ptr<BBulletBase> bullet = GetValidBullet();



	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(muzzlePos_[muzzleCount_]);

	// 位置を加算して最終的なワールド座標にする
	VECTOR bulletpos = VAdd(transform_.pos, localRotPos);

	bullet->CreateBullets(bulletpos, bulletDir_, 2.0f);
	bullet->Init();

}
