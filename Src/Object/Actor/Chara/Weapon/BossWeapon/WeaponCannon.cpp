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

VECTOR WeaponCannon::GetPos(void) const
{
	return transform_.pos;
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
	ColliderLine* colLine = new ColliderLine(tag_, &transform_, {0.0f,0.0f,0.0f}, { 0.0f,-5.0f,0.0f });
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);
	

	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, {0.0f,50.0f,160.0f}, { 0.0f,50.0f,-40.0f }, 20.0f);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
	colCapsule->SetTriger(false);
}

void WeaponCannon::InitAnimation(void)
{
}

void WeaponCannon::InitPost(void)
{
}

void WeaponCannon::UpdateProcess(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponCannon::UpdateProcessPost(void)
{
}

float WeaponCannon::Damage(void)
{
	return 0.0f;
}

void WeaponCannon::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);

	for (auto& col : ownColliders_)
	{
		col.second->Draw();
	}
}
