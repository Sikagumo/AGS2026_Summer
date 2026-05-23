#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderSphere.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "WeaponMP.h"

WeaponMP::WeaponMP(void)
{
	
}

void WeaponMP::Release(void)
{
}

void WeaponMP::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag)
{
	bone_.id = _id;
	bone_.transform = _trans;
	tag_ = _tag;
}

int WeaponMP::GetDamage(void)
{
	return 0;
}

VECTOR WeaponMP::GetPos(void) const
{
	return transform_.pos;
}

void WeaponMP::Load(void)
{
	transform_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_RK));
}


void WeaponMP::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(0), UtilityMath::AXIS_Y));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMP::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(tag_, &transform_, { 0.0f,0.0f,0.0f }, { 0.0f,-50.0f,0.0f });
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);


	ColliderSphere* colSphere = new ColliderSphere(
		tag_, &transform_, { 0.0f,0.0f,-40.0f },40.0f);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colSphere);
	colSphere->SetTriger(false);
}

void WeaponMP::InitAnimation(void)
{
}

void WeaponMP::InitPost(void)
{
}

void WeaponMP::UpdateProcess(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMP::UpdateProcessPost(void)
{
}

float WeaponMP::Damage(void)
{
	return 0.0f;
}

void WeaponMP::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
	for (auto& col : ownColliders_)
	{
		col.second->Draw();
	}
}
