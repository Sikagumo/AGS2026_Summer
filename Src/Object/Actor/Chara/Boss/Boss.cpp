#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../Common/Transform.h"
#include "../../../Collider/ColliderBase.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderLine.h"
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
	transformFeetCar_(),
	transformWheelBack_(),
	transformWheelFront_(),
	hp_(),
	attackDelay_(),
	boneName_(),


	CharaBase()
{
}

Boss::~Boss(void)
{
}

void Boss::Release(void)
{
}



void Boss::BoneParam(void)
{
	boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)]={JOINT_WAEAPON_MG_L, transformBody_ };
	boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)]={ JOINT_WAEAPON_MG_R, transformBody_ };
	boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)]={JOINT_WAEAPON_CANNON_L, transformBody_ };
	boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)]={ JOINT_WAEAPON_CANNON_R, transformBody_ };
	boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)]={JOINT_WAEAPON_MP_L, transformBody_ };
	boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)]={ JOINT_WAEAPON_MP_R, transformBody_ };
	boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)]={JOINT_WAEAPON_RG, transformBody_ };

}

void Boss::BossTransformUpdate(void)
{
	transformFeet_.Update();
	transformFeetCar_.Update();
	BoneParam();
	transformBody_.Update();

	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform);
	weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform);
	weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform);
	weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform);
	weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform);
	weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform);

	weaponMGL_->Update();
	weaponMGR_->Update();
	weaponMPL_->Update();
	weaponMPR_->Update();
	weaponRG_->Update();
	weaponCannonL_->Update();
	weaponCannonR_->Update();
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
	transformBody_.pos = MV1GetFramePosition(transformFeet_.modelId,JOINT_FEET_BODY);
	
	transformBody_.Update();
	BoneParam();
}

void Boss::InitTransform(void)
{
}

void Boss::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::BOSS,&transformFeet_,{0.0f,100.0f,0.0f },{0.0f,-10.0f,0.0f});
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::BOSS, &transformFeet_, { 0.0f,130.0f,0.0f }, { 0.0f,80.0f,0.0f },80.0f);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Boss::InitAnimation(void)
{
}

void Boss::InitPost(void)
{
	//make_uniqueÇ≈èâä˙âª

	weaponMGL_=std::make_unique<WeaponMGL>();
	weaponMGR_=std::make_unique<WeaponMGR>();
	weaponMPL_=std::make_unique<WeaponMP>();
	weaponMPR_=std::make_unique<WeaponMP>();
	weaponRG_=std::make_unique<WeaponRG>();
	weaponCannonL_=std::make_unique<WeaponCannon>();
	weaponCannonR_=std::make_unique<WeaponCannon>();

	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform);
	weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform);
	weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform);
	weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform);
	weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform);
	weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform);

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
	

	
	
	BossTransformUpdate();
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
	for (auto& col : ownColliders_)
	{
		col.second->Draw();
	}
}
