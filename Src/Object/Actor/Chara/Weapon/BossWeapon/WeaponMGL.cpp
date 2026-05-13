#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponMGL.h"

WeaponMGL::WeaponMGL(int _modelId, int _jointNo)
{
	modelId_ = _modelId;
	jointNo_ = _jointNo;
}

void WeaponMGL::Release(void)
{
}

void WeaponMGL::InitLoad(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_MG_L));
}

void WeaponMGL::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal=
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos= MV1GetFramePosition(modelId_, jointNo_);
	transform_.Update();
}

void WeaponMGL::InitCollider(void)
{
}

void WeaponMGL::InitAnimation(void)
{
}

void WeaponMGL::InitPost(void)
{
}

void WeaponMGL::UpdateProcess(void)
{
}

void WeaponMGL::UpdateProcessPost(void)
{
}

float WeaponMGL::Damage(void)
{
	return 0.0f;
}

void WeaponMGL::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
}
