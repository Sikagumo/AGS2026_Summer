#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Manager/Generic/InputManager.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../Common/Transform.h"
#include "../../../Collider/ColliderBase.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderSphere.h"
#include "../../../Manager/CollisionManager.h"
#include "../Weapon/WeaponBase.h"
#include "../Weapon/BossWeapon/WeaponMGL.h"
#include "../Weapon/BossWeapon/WeaponMGR.h"
#include "../Weapon/BossWeapon/WeaponMP.h"
#include "../Weapon/BossWeapon/WeaponRG.h"
#include "../Weapon/BossWeapon/WeaponCannon.h"
#include "../Weapon/Bullet/Boss/BBulletWave.h"
#include "Boss.h"

Boss::Boss(void) :
	transformFeet_(),
	transformBody_(),
	transformFeetCar_(),
	transformWheelBack_(),
	transformWheelFront_(),
	hp_(1000),
	boneName_(),


	CharaBase()
{
	weaponMGL_ = std::make_unique<WeaponMGL>();
	weaponMGR_ = std::make_unique<WeaponMGR>();
	weaponMPL_ = std::make_unique<WeaponMP>();
	weaponMPR_ = std::make_unique<WeaponMP>();
	weaponRG_ = std::make_unique<WeaponRG>();
	weaponCannonL_ = std::make_unique<WeaponCannon>();
	weaponCannonR_ = std::make_unique<WeaponCannon>();
	
}

Boss::~Boss(void)
{
}

void Boss::ReleasePost(void)
{
}

const VECTOR& Boss::GetBossPos(void) const
{
	return transformBody_.pos;
}


//各武器のダメージ受け取り用＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Boss::SetWeaponMGLDamage(int _damage)
{

	weaponMGL_->SetDamage(_damage);
}

void Boss::SetWeaponMGRDamage(int _damage)
{
	weaponMGR_->SetDamage(_damage);
}

void Boss::SetWeaponMPLDamage(int _damage)
{
	weaponMPL_->SetDamage(_damage);
}

void Boss::SetWeaponMPRDamage(int _damage)
{
	weaponMPR_->SetDamage(_damage);
	
}

void Boss::SetWeaponCannonLDamage(int _damage)
{
	weaponCannonL_->SetDamage(_damage);
}

void Boss::SetWeaponCannonRDamage(int _damage)
{
	weaponCannonR_->SetDamage(_damage);
}

void Boss::SetWeaponRGDamage(int _damage)
{
	weaponRG_->SetDamage(_damage);
}

void Boss::SetBossDamage(int _damage)
{
	hp_ -= _damage;
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝


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






void Boss::Load(void)
{
	transform_.modelId = resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_FEET);
	transformBody_.modelId = resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_BODY);

	WeaponLoad();
	
}

void Boss::InitTransform(void)
{
	transform_.scl = BOSS_SIZE;
	transformBody_.scl = BOSS_SIZE;
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));

	transformBody_.quaRot = Quaternion::Mult(transformBody_.quaRot,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));

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
	wave_ = std::make_unique< BBulletWave>(transform_);


	WeaponSet();

	WeaponInit();

	
	wave_->Init();

	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&Boss::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&Boss::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::JUMP), std::bind(&Boss::ChangeStateJump, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&Boss::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);


	hp_ = 1000;
}

void Boss::ChangeState(STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	Boss::ChangeState(state);
}

void Boss::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();

}
//各ステイトのチェンジ関数＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝－
void Boss::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateIdle, this);
	attackCount_ = 0;
}

void Boss::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateAttack, this);
	attackCount_ = 0;
	
}

void Boss::ChangeStateJump(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateJump, this);
	// ジャンプ量の計算
	float jumpSpeed = POW_JUMP_INIT * TimeManager::GetInstance().GetDeltaTime();
	jumpPow_ = VScale(UtilityMath::DIR_UP, jumpSpeed);
	isJump_ = true;
}

void Boss::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateEnd, this);
}
//===========================================================================================================================================================================================================================================================

void Boss::BossTransformUpdate(void)
{
	transform_.Update();
	transformFeetCar_.Update();
	BoneParam();
	transformBody_.Update();

	WeaponSet();

	WeaponUpdate();

}

void Boss::UpdateProcess(void)
{
	
	if (inputManager_.IsTrgDown(KEY_INPUT_U))
	{
		transform_.pos.y += 10.0f;
	}

	
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);

	
	
	BossTransformUpdate();

	WeaponSet();

	// 武器の更新
	WeaponUpdate();

	wave_->SetPos(transform_.pos);
	wave_->Update();

	bool a = CollisionManager::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::BOSS
		, ColliderBase::TAG::PLAYER);

	stateUpdate_();

}

void Boss::UpdateProcessPost(void)
{
}

void Boss::UpdateCollision(void)
{

}

//各ステイトのアップデート関数＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝－
void Boss::UpdateIdle(void)
{
	attackCount_++;
	if (attackCount_ >= 600)
	{
		ChangeState(STATE::ATTACK);
	}
}

void Boss::UpdateAttack(void)
{
	ChangeState(STATE::JUMP);
}

void Boss::UpdateJump(void)
{
	if (!isJump_)
	{
		wave_->SetIsAttac(true);
		ChangeState(STATE::IDLE);
	}
	
	if (transform_.pos.y >= 3500)
	{
		jumpPow_ = VScale(UtilityMath::DIR_UP, -50.0f);
	}
}

void Boss::UpdateEnd(void)
{
	
}
//===========================================================================================================================================================================================================================================================



void Boss::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
	MV1DrawModel(transformBody_.modelId);
	
	WeaponDraw();

	wave_->Draw();
	for (auto& col : ownColliders_)
	{
		col.second->Draw();
	}
	DrawFormatString(10, 100, 0xffffff, "bossの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
	DrawFormatString(10, 400, 0xffffff, "hp:%d", hp_);
}

//ウェポンの呼び出し纏めよう＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Boss::WeaponSet(void)
{
	// 各武器にボーン情報を設定（ここはそのまま）
	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform, ColliderBase::TAG::WEAPON_MG_L);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform, ColliderBase::TAG::WEAPON_MG_R);
	//weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform, ColliderBase::TAG::WEAPON_MP_L);
	//weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform, ColliderBase::TAG::WEAPON_MP_R);
	//weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform, ColliderBase::TAG::WEAPON_RG);
	//weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform, ColliderBase::TAG::WEAPON_CANNON_L);
	//weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform, ColliderBase::TAG::WEAPON_CANNON_R);
}

void Boss::WeaponLoad(void)
{
	weaponMGL_->Load();
	weaponMGR_->Load();
	//weaponMPL_->Load();
	//weaponMPR_->Load();
	//weaponRG_->Load();
	//weaponCannonL_->Load();
	//weaponCannonR_->Load();
}

void Boss::WeaponInit(void)
{
	weaponMGL_->Init();
	weaponMGR_->Init();
	//weaponMPL_->Init();
	//weaponMPR_->Init();
	//weaponRG_->Init();
	//weaponCannonL_->Init();
	//weaponCannonR_->Init();
}

void Boss::WeaponUpdate(void)
{
	weaponMGL_->Update();
	weaponMGR_->Update();
	//weaponMPL_->Update();
	//weaponMPR_->Update();
	//weaponRG_->Update();
	//weaponCannonL_->Update();
	//weaponCannonR_->Update();
}

void Boss::WeaponDraw(void)
{
	weaponMGL_->Draw();
	weaponMGR_->Draw();
	//weaponMPL_->Draw();
	//weaponMPR_->Draw();
	//weaponRG_->Draw();
	//weaponCannonL_->Draw();
	//weaponCannonR_->Draw();
}
