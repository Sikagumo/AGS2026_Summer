#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponRG.h"

WeaponRG::WeaponRG()
{
		
}

void WeaponRG::Release(void)
{
}

void WeaponRG::SetBone(int _id, Transform _trans)
{
	bone_.id = _id;
	bone_.transform = _trans;
}

int WeaponRG::GetDamage(void) const
{
	return 0;
}

void WeaponRG::InitLoad(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_RG));
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
}
