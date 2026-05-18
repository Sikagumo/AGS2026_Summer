#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponMGR.h"

WeaponMGR::WeaponMGR()
{
	
}

void WeaponMGR::Release(void)
{
}


void WeaponMGR::SetBone(int _id, Transform _trans)
{
	bone_.id = _id;
	bone_.transform = _trans;
}

int WeaponMGR::GetDamage(void) const
{
	return 0;
}

VECTOR WeaponMGR::GetPos(void) const
{
	return transform_.pos;
}

void WeaponMGR::Load(void)
{
	transform_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_MG_R));
}

void WeaponMGR::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMGR::InitCollider(void)
{
}

void WeaponMGR::InitAnimation(void)
{
}

void WeaponMGR::InitPost(void)
{
}

void WeaponMGR::UpdateProcess(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMGR::UpdateProcessPost(void)
{
}

float WeaponMGR::Damage(void)
{
	return 0.0f;
}

void WeaponMGR::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
}
