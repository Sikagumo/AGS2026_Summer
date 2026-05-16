#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../Common/Transform.h"
#include "../Weapon/WeaponBase.h"
#include "../Weapon/BossWeapon/WeaponMGL.h"
#include "../Weapon/BossWeapon/WeaponMGR.h"
#include "../Weapon/BossWeapon/WeaponMP.h"
#include "../Weapon/BossWeapon/WeaponRG.h"
#include "../Weapon/BossWeapon/WeaponCannon.h"
#include "Boss.h"

Boss::Boss(void):
	transformFeet_(),
	transformBody_(),
	hp_(),
	attackDelay_(),

	CharaBase()
{
}

Boss::~Boss(void)
{
}

void Boss::Release(void)
{
}

Boss::Bone Boss::GetBone(BONE_NAME _boneName)
{
	return boneId_[static_cast<int>(_boneName)];
}

void Boss::InitLoad(void)
{
	transformFeet_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_FEET));
	transformBody_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_BODY));
	transformFeet_.scl = BOSS_SIZE;
	transformBody_.scl = BOSS_SIZE;
	transformFeet_.quaRot= Quaternion::Identity();
	transformFeet_.quaRotLocal =
		Quaternion::Mult(transformFeet_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformBody_.quaRot = Quaternion::Identity();
	transformBody_.quaRotLocal =
		Quaternion::Mult(transformBody_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));

	transformFeet_.pos= BOSS_INIT_POS;
	transformFeet_.Update();
	transformBody_.pos = MV1GetFramePosition(transformFeet_.modelId,JOINT_NO);
	
	transformBody_.Update();

}

void Boss::InitTransform(void)
{
}

void Boss::InitCollider(void)
{
}

void Boss::InitAnimation(void)
{
}

void Boss::InitPost(void)
{
	//make_uniqueÇ≈èâä˙âª
	weaponMGL_=std::make_unique<WeaponMGL>(transformBody_.modelId,4);
	weaponMGR_=std::make_unique<WeaponMGR>(transformBody_.modelId,10);
	weaponMPL_=std::make_unique<WeaponMP>(transformBody_.modelId,8);
	weaponMPR_=std::make_unique<WeaponMP>(transformBody_.modelId,14);
	weaponRG_=std::make_unique<WeaponRG>(transformBody_.modelId,16);
	weaponCannonL_=std::make_unique<WeaponCannon>(transformBody_.modelId,6);
	weaponCannonR_=std::make_unique<WeaponCannon>(transformBody_.modelId,12);

	weaponMGL_->Init();
	weaponMGR_->Init();
	weaponMPL_->Init();
	weaponMPR_->Init();
	weaponRG_->Init();
	weaponCannonL_->Init();
	weaponCannonR_->Init();
}

void Boss::UpdateProcess(void)
{
	weaponMGL_->Update();
	weaponMGR_->Update();
	weaponMPL_->Update();
	weaponMPR_->Update();
	weaponRG_->Update();
	weaponCannonL_->Update();
	weaponCannonR_->Update();

}

void Boss::UpdateProcessPost(void)
{
}

void Boss::DrawPre(void)
{
	MV1DrawModel(transformFeet_.modelId);
	MV1DrawModel(transformBody_.modelId);
	weaponMGL_->Draw();
	weaponMGR_->Draw();
	weaponMPL_->Draw();
	weaponMPR_->Draw();
	weaponRG_->Draw();
	weaponCannonL_->Draw();
	weaponCannonR_->Draw();
}
