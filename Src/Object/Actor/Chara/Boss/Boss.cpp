#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Manager/Generic/InputManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../Common/Transform.h"
#include "../../../Collider/ColliderBase.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Manager/CollisionManager.h"
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
	hp_(1000),
	attackDelay_(20),
	boneName_(),


	CharaBase()
{
}

Boss::~Boss(void)
{
}

void Boss::ReleasePost(void)
{
}

VECTOR Boss::GetBossPos(void) const
{
	return transformBody_.pos;
}

void Boss::SetWeponDamege(int Damege)
{
}


void Boss::SetBossDamege(int Damege)
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
	transform_.Update();
	transformFeetCar_.Update();
	BoneParam();
	transformBody_.Update();

	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform, ColliderBase::TAG::WEAPON_MG_L);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform, ColliderBase::TAG::WEAPON_MG_R);
	weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform, ColliderBase::TAG::WEAPON_MP_L);
	weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform, ColliderBase::TAG::WEAPON_MP_R);
	weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform, ColliderBase::TAG::WEAPON_RG);
	weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform, ColliderBase::TAG::WEAPON_CANNON_L);
	weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform, ColliderBase::TAG::WEAPON_CANNON_R);

	weaponMGL_->Update();
	weaponMGR_->Update();
	weaponMPL_->Update();
	weaponMPR_->Update();
	weaponRG_->Update();
	weaponCannonL_->Update();
	weaponCannonR_->Update();
}

void Boss::UpdateCollision(void)
{

}

void Boss::Load(void)
{
	transform_.modelId = resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_FEET);
	transformBody_.modelId = resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_BODY);
	
}

void Boss::InitTransform(void)
{
	transform_.scl = BOSS_SIZE;
	transformBody_.scl = BOSS_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformBody_.quaRot = Quaternion::Identity();
	transformBody_.quaRotLocal =
		Quaternion::Mult(transformBody_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));

	transform_.pos = BOSS_INIT_POS;
	transform_.Update();
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);

	transformBody_.Update();
	BoneParam();
}

void Boss::InitCollider(void)
{

	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::BOSS, &transform_, COL_LINE_START_POS, COL_LINE_END_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::BOSS, &transform_, COL_CAPSULE_START_POS, COL_CAPSULE_END_POS, COL_CAPSULE_END_RADIUS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
	colCapsule->SetTriger(false);

	CollisionManager::GetInstance().RegisterActor(this);
}

void Boss::InitAnimation(void)
{
}

void Boss::InitPost(void)
{
	//make_uniqueで初期化

	weaponMGL_=std::make_unique<WeaponMGL>();
	weaponMGR_=std::make_unique<WeaponMGR>();
	weaponMPL_=std::make_unique<WeaponMP>();
	weaponMPR_=std::make_unique<WeaponMP>();
	weaponRG_=std::make_unique<WeaponRG>();
	weaponCannonL_=std::make_unique<WeaponCannon>();
	weaponCannonR_=std::make_unique<WeaponCannon>();

	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform, ColliderBase::TAG::WEAPON_MG_L);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform, ColliderBase::TAG::WEAPON_MG_R);
	weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform, ColliderBase::TAG::WEAPON_MP_L);
	weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform, ColliderBase::TAG::WEAPON_MP_R);
	weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform, ColliderBase::TAG::WEAPON_RG);
	weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform, ColliderBase::TAG::WEAPON_CANNON_L);
	weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform, ColliderBase::TAG::WEAPON_CANNON_R);

	weaponMGL_->Load();
	weaponMGR_->Load();
	weaponMPL_->Load();
	weaponMPR_->Load();
	weaponRG_->Load();
	weaponCannonL_->Load();
	weaponCannonR_->Load();

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
	
	if (inputManager_.IsTrgDown(KEY_INPUT_U))
	{
		transform_.pos.y += 10.0f;
	}

	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);

	
	
	BossTransformUpdate();

	// 各武器にボーン情報を設定（ここはそのまま）
	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform, ColliderBase::TAG::WEAPON_MG_L);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform, ColliderBase::TAG::WEAPON_MG_R);
	weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform, ColliderBase::TAG::WEAPON_MP_L);
	weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform, ColliderBase::TAG::WEAPON_MP_R);
	weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform, ColliderBase::TAG::WEAPON_RG);
	weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform, ColliderBase::TAG::WEAPON_CANNON_L);
	weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform, ColliderBase::TAG::WEAPON_CANNON_R);

	// 武器の更新
	weaponMGL_->Update();
	weaponMGR_->Update();
	weaponMPL_->Update();
	weaponMPR_->Update();
	weaponRG_->Update();
	weaponCannonL_->Update();
	weaponCannonR_->Update();

	bool a = CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::BOSS
		, ColliderBase::TAG::PLAYER);

	
}

void Boss::UpdateProcessPost(void)
{
}

void Boss::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
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
	DrawFormatString(10, 100, 0xffffff, "bossの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);

}
