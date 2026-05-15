#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponRG.h"

WeaponRG::WeaponRG(int _modelId,int _jointNo)
{
		modelId_ = _modelId;
		jointNo_ = _jointNo;
}

void WeaponRG::Release(void)
{
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

	transform_.pos = MV1GetFramePosition(modelId_, jointNo_);
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
