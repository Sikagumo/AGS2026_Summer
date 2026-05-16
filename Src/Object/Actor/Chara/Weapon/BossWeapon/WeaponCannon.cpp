#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponCannon.h"

WeaponCannon::WeaponCannon(int _modelId,int _jointNo)
{
	modelId_ = _modelId;
	jointNo_ = _jointNo;
}

void WeaponCannon::Release(void)
{
}

void WeaponCannon::InitLoad(void)
{
	transform_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_CN));
}

void WeaponCannon::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos = MV1GetFramePosition(modelId_, jointNo_);
	transform_.Update();
}

void WeaponCannon::InitCollider(void)
{
}

void WeaponCannon::InitAnimation(void)
{
}

void WeaponCannon::InitPost(void)
{
}

void WeaponCannon::UpdateProcess(void)
{
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
}
