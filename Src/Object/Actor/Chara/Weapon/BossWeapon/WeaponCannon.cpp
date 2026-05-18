#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderCapsule.h"
#include "WeaponCannon.h"

WeaponCannon::WeaponCannon()
{
	
}

void WeaponCannon::Release(void)
{
}

void WeaponCannon::SetBone(int _id, Transform _trans)
{
	bone_.id = _id;
	bone_.transform = _trans;
}
int WeaponCannon::GetDamage(void) const
{
	return 0;
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

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
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
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
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
