#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "WeaponMGL.h"

WeaponMGL::WeaponMGL()
{
	
}

void WeaponMGL::Release(void)
{
}

void WeaponMGL::SetBone(int _id, Transform _trans)
{
	bone_.id = _id;
	bone_.transform = _trans;
}
int WeaponMGL::GetDamage(void) const
{
	return 0;
}

void WeaponMGL::InitLoad(void)
{
	transform_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_MG_L));
}

void WeaponMGL::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal=
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos= MV1GetFramePosition(bone_.transform.modelId, bone_.id);
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
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
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
