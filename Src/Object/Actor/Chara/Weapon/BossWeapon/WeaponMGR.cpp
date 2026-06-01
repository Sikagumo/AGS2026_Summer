#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderCapsule.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "WeaponMGR.h"

WeaponMGR::WeaponMGR()
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
}

void WeaponMGR::UpdateProcess(void)
{
	if (isAlive_)
	{
		transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
		LookPlayer();
	}
	if (hp_ <= 0)
	{
		isAlive_ = false;
		CollisionManager::GetInstance().SetCollisionActive(this, tag_, false);
	}
}

void WeaponMGR::UpdateProcessPost(void)
{
	
}



void WeaponMGR::DrawPre(void)
{
	if (isAlive_)
	{

		
		for (auto& col : ownColliders_)
		{
			col.second->Draw();
		}
	}
	DrawFormatString(10, 300, 0xffffff, "MGR_HP:%d", hp_);
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
}
