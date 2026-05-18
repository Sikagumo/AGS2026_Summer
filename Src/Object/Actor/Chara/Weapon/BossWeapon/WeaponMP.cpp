#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponMP.h"

WeaponMP::WeaponMP()
{
	
}

void WeaponMP::Release(void)
{
}

void WeaponMP::SetBone(int _id, Transform _trans)
{
	bone_.id = _id;
	bone_.transform = _trans;
}

int WeaponMP::GetDamage(void) const
{
	return 0;
}

void WeaponMP::InitLoad(void)
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
}
