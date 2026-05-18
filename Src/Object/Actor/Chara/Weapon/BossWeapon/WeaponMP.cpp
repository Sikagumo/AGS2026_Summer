#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponMP.h"

WeaponMP::WeaponMP(int _modelId,int _jointNo)
{
	modelId_ = _modelId;
	jointNo_ = _jointNo;
}

void WeaponMP::Release(void)
{
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

	transform_.pos = MV1GetFramePosition(modelId_, jointNo_);
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
