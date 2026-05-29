#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderCapsule.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "WeaponCannon.h"

WeaponCannon::WeaponCannon()
{
	
}

void WeaponCannon::ReleasePost(void)
{
}

void WeaponCannon::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag)
{
	bone_.id = _id;
	bone_.transform = _trans;
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
	//transform_.SetModel(resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_CN));
	transform_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_CN));
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
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);
	

	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
	colCapsule->SetTriger(false);

	CollisionManager::GetInstance().RegisterActor(this);
}

void WeaponCannon::InitAnimation(void)
{
}

void WeaponCannon::InitPost(void)
{
	isAlive_ = true;
	localPos_ = LINE_START_POS;
}

void WeaponCannon::UpdateProcess(void)
{
	if (isAlive_)
	{
		transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	}

	if (hp_ <= 0)
	{
		isAlive_ = false;
		//CollisionManager::GetInstance().SetCollisionActive(this, tag_, false);
	}

}

void WeaponCannon::UpdateProcessPost(void)
{
}



void WeaponCannon::DrawPre(void)
{


	for (auto& col : ownColliders_)
	{
		col.second->Draw();
	}

}
