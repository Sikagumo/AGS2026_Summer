#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponMGR.h"

WeaponMGR::WeaponMGR(int _modelId, int _jointNo)
{
	modelId_ = _modelId;
	jointNo_ = _jointNo;
}

void WeaponMGR::Release(void)
{
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

	transform_.pos = MV1GetFramePosition(modelId_, jointNo_);
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
