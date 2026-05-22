#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderCapsule.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "WeaponRG.h"

WeaponRG::WeaponRG(void)
{
		
}

void WeaponRG::Release(void)
{
}

void WeaponRG::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag)
{
	bone_.id = _id;
	bone_.transform = _trans;
	tag_ = _tag;
}


VECTOR WeaponRG::GetPos(void) const
{
	return transform_.pos;
}

void WeaponRG::Load(void)
{
	transform_.SetModel(resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_RG));
}

void WeaponRG::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(90.0f), UtilityMath::AXIS_X));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponRG::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(tag_, &transform_, { 0.0f,0.0f,0.0f }, { 0.0f,-50.0f,0.0f });
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, {0.0f,-100.0f,-60.0f }, { 0.0f,80.0f,-60.0f }, 30.0f);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
	colCapsule->SetTriger(false);

	CollisionManager::GetInstance().RegisterActor(this);
}

void WeaponRG::InitAnimation(void)
{
}

void WeaponRG::InitPost(void)
{
}

void WeaponRG::UpdateProcess(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponRG::UpdateProcessPost(void)
{
}

float WeaponRG::Damage(void)
{
	return 0.0f;
}

void WeaponRG::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);

	for (auto& col : ownColliders_)
	{
		col.second->Draw();
	}
}
