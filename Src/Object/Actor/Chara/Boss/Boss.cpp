#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Manager/Generic/InputManager.h"
#include "../../../../Manager/Generic/SceneManager.h"
#include "../../../../Manager/Decoration/SoundManager.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../../Manager/Decoration/EffectManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../../Camera/Camera.h"
#include "../../../Common/Transform.h"
#include "../../../Common/AnimationController.h"
#include "../../../Collider/ColliderBase.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderSphere.h"
#include "../../../Collision/CollisionController.h"
#include "../Weapon/WeaponBase.h"
#include "../Weapon/BossWeapon/MG/WeaponMGL.h"
#include "../Weapon/BossWeapon/MG/WeaponMGR.h"
#include "../Weapon/BossWeapon/MP/WeaponMP.h"
#include "../Weapon/BossWeapon/RG/WeaponRG.h"
#include "../Weapon/BossWeapon/Cannon/WeaponCannon.h"
#include "../Weapon/Bullet/Boss/BBulletWave.h"
#include "Boss.h"

Boss::Boss(void) :
	               
	hp_(MAX_HP),
	boneName_(BONE_NAME::WEAPON_JOINT_MGL_L), 
	jumpDir_({ 0.0f, 0.0f, 0.0f }),          
	speed_(MOVE_SPEED_INIT),
	jumpCount_(0),                           
	attackCount_(0),                             
	state_(STATE::IDLE),
	stateBase_(-1),
	roadCount_(0),
	roadDir_({0.0f,0.0f,0.0f}),
	roadIsAttack_(false),
	roadAttackTime_(-1),
	roadLockTime_(-1),
	attackInterval_(MAX_ATTACK_INTERVAL),
	soundRadius_(SOUND_RADIUS),
	isLanging_(false),
	isMGFire_(false),
	isRoadFire_(false),
	laserShotHp_(MAX_HP),
	laserAttackRot_(0.0f),
	laserRotSpeed_(LASER_ROT_SPEED),
	lastAttackType_ (ATTACK_TYPE::MAX),
	wallStopPos_({0,0,0}),
	mainPos_({0,0,0}),
	mpPos_({0,0,0}),
	CannonPos_({0,0,0}),
	isHostControl_(false),
	nextChangeMainTime_(INTERVAL_SEC),
	nextChangeMpTime_(INTERVAL_SEC_MP),
	nextChangeCannonTime_(INTERVAL_SEC_CANNON),
	


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
	if (_damage > EFFECT_PLAEY_DAMEGE)
	{

		PlayEffect();
	}


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
	transformFeet_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_BOSS_FEET);
	transformBody_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_BOSS_BODY);
	transformFeetCar_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_BOSS_CAR);
	transformWheelBackL_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WHEEL_BACK);
	transformWheelBackFrontL_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WHEEL_BACK);
	transformWheelFrontL_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WHEEL_FRONT);
	transformWheelBackR_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WHEEL_BACK);
	transformWheelBackFrontR_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WHEEL_BACK);
	transformWheelFrontR_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WHEEL_FRONT);

	WeaponLoad();
	
	//SE
	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_BOSS_LANDING, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_BOSS_LANDING));

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_BOSS_ROAD, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_BOSS_ROAD));

	


}

void Boss::PlayEffect(void)
{
	
	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_BOSS_HIT, { 0,0,0 }, EFFECT_ROT, EFFECT_SCL, EFFECT_PLAEY_SPEED, this, EFFECT_NO_ONE);
	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_BOSS_HIT, { 0,0,0 }, { 0,0,0 }, EFFECT_SCL, EFFECT_PLAEY_SPEED, this, EFFECT_NO_TWO);
	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_BOSS_HIT, { 0,0,0 }, { 0,0,0 }, EFFECT_SCL, EFFECT_PLAEY_SPEED, this, EFFECT_NO_THREE);
}

void Boss::UpdateEffect(void)
{
	VECTOR hitEffectPos1= MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	EffectManager::GetInstance().UpdatePos(EffectManager::EFFECT::EFFECT_BOSS_HIT, this, hitEffectPos1, EFFECT_NO_ONE);
	VECTOR hitEffectPos2 = MV1GetFramePosition(transformBody_.modelId, JOINT_WAEAPON_MP_L);
	EffectManager::GetInstance().UpdatePos(EffectManager::EFFECT::EFFECT_BOSS_HIT, this, hitEffectPos2, EFFECT_NO_TWO);
	VECTOR hitEffectPos3 = MV1GetFramePosition(transformBody_.modelId, JOINT_WAEAPON_MP_R);
	EffectManager::GetInstance().UpdatePos(EffectManager::EFFECT::EFFECT_BOSS_HIT, this, hitEffectPos3, EFFECT_NO_THREE);

	VECTOR hitEffectRot1 = Quaternion::Mult(transformBody_.quaRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(90.0f), UtilityMath::AXIS_X)).ToEuler();
	hitEffectRot1.x = UtilityMath::Rad2DegF(hitEffectRot1.x);
	hitEffectRot1.y = UtilityMath::Rad2DegF(hitEffectRot1.y);
	hitEffectRot1.z = UtilityMath::Rad2DegF(hitEffectRot1.z);
	EffectManager::GetInstance().UpdateRot(EffectManager::EFFECT::EFFECT_BOSS_HIT, this, hitEffectRot1, EFFECT_NO_ONE);
	VECTOR hitEffectRot2 = Quaternion::Mult(transformBody_.quaRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(180.0f), UtilityMath::AXIS_Y)).ToEuler();
	hitEffectRot2.x = UtilityMath::Rad2DegF(hitEffectRot2.x);
	hitEffectRot2.y = UtilityMath::Rad2DegF(hitEffectRot2.y);
	hitEffectRot2.z = UtilityMath::Rad2DegF(hitEffectRot2.z);
	EffectManager::GetInstance().UpdateRot(EffectManager::EFFECT::EFFECT_BOSS_HIT, this, hitEffectRot2, EFFECT_NO_TWO);
	VECTOR hitEffectRot3 = Quaternion::Mult(transformBody_.quaRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(0.0f), UtilityMath::AXIS_X)).ToEuler();
	hitEffectRot3.x = UtilityMath::Rad2DegF(hitEffectRot3.x);
	hitEffectRot3.y = UtilityMath::Rad2DegF(hitEffectRot3.y);
	hitEffectRot3.z = UtilityMath::Rad2DegF(hitEffectRot3.z);
	EffectManager::GetInstance().UpdateRot(EffectManager::EFFECT::EFFECT_BOSS_HIT, this, hitEffectRot3, EFFECT_NO_THREE);


}

void Boss::InitTransform(void)
{
	//ベース
	transform_.modelId = transformFeet_.modelId;
	transform_.scl = BOSS_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transform_.pos = BOSS_INIT_POS;
	transform_.Update();

	//足

	transformFeet_.scl = BOSS_SIZE;
	transformFeet_.Update();

	//車体
	transformFeetCar_.scl = BOSS_CAR_SIZE;
	transformFeetCar_.quaRot = Quaternion::Identity();
	transformFeetCar_.quaRotLocal =
		Quaternion::Mult(transformFeetCar_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformFeetCar_.pos = transform_.pos;
	transformFeetCar_.Update();

	//前輪L
	transformWheelFrontL_.scl = BOSS_CAR_SIZE;
	transformWheelFrontL_.quaRot = Quaternion::Identity();
	transformWheelFrontL_.quaRotLocal =
		Quaternion::Mult(transformWheelFrontL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformWheelFrontL_.Update();
	//前輪R
	transformWheelFrontR_.scl = BOSS_CAR_SIZE;
	transformWheelFrontR_.quaRot = Quaternion::Identity();
	transformWheelFrontR_.quaRotLocal =
		Quaternion::Mult(transformWheelFrontR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(0.0f), UtilityMath::AXIS_Y));
	transformWheelFrontR_.Update();
	//後輪前L
	transformWheelBackFrontL_.scl = BOSS_CAR_SIZE;
	transformWheelBackFrontL_.quaRot = Quaternion::Identity();
	transformWheelBackFrontL_.quaRotLocal =
		Quaternion::Mult(transformWheelBackFrontL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformWheelBackFrontL_.Update();
	//後輪前R
	transformWheelBackFrontR_.scl = BOSS_CAR_SIZE;
	transformWheelBackFrontR_.quaRot = Quaternion::Identity();
	transformWheelBackFrontR_.quaRotLocal =
		Quaternion::Mult(transformWheelBackFrontR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(0.0f), UtilityMath::AXIS_Y));
	transformWheelBackFrontR_.Update();
	//後輪L
	transformWheelBackL_.scl = BOSS_CAR_SIZE;
	transformWheelBackL_.quaRot = Quaternion::Identity();
	transformWheelBackL_.quaRotLocal =
		Quaternion::Mult(transformWheelBackL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformWheelBackL_.Update();
	//後輪R
	transformWheelBackR_.scl = BOSS_CAR_SIZE;
	transformWheelBackR_.quaRot = Quaternion::Identity();
	transformWheelBackR_.quaRotLocal =
		Quaternion::Mult(transformWheelBackR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(0.0f), UtilityMath::AXIS_Y));
	transformWheelBackR_.Update();



	//胴体
	transformBody_.scl = BOSS_SIZE;
	transformBody_.quaRot = Quaternion::Identity();
	transformBody_.quaRotLocal =
		Quaternion::Mult(transformBody_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	transformBody_.Update();


	BoneParam();
}

void Boss::InitCollider(void)
{

	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::BOSS, &transform_, COL_LINE_START_POS, COL_LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::BOSS)].push_back(colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::BOSS, &transform_, COL_CAPSULE_START_POS, COL_CAPSULE_END_POS, COL_CAPSULE_END_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::BOSS)].push_back(colCapsule);
	colCapsule->SetTriger(false);

	ColliderSphere* colSphere = new ColliderSphere(ColliderBase::TAG::ROAD_ATTACK, &transform_, { 0.0f,0.0f,0.0f }, 200.0f);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ROAD_ATTACK)].push_back(colSphere);
	
	CollisionController::GetInstance().RegisterActor(this);

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, false);


}

void Boss::InitAnimation(void)
{
	CharaBase::InitAnimation();
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animation_->AddInternal(i, ANIM_SPEED);
	}
	animation_->Play(static_cast<int>(ANIM_TYPE::DIR));

	
}



void Boss::InitPost(void)
{
	//make_uniqueで初期化
	wave_ = std::make_unique< BBulletWave>(transform_);


	WeaponSet();

	WeaponInit();


	hp_ = MAX_HP + (MAX_HP_HALF * playerSize_);
	laserShotHp_ = hp_;
	weaponCannonL_->SetHp(hp_ * WEAPON_HP_CANNON);
	weaponCannonR_->SetHp(hp_ * WEAPON_HP_CANNON);
	weaponMGL_->SetHp(hp_ * WEAPON_HP_MG);
	weaponMGR_->SetHp(hp_ * WEAPON_HP_MG);
	weaponMPL_->SetHp(hp_ * WEAPON_HP_MP);
	weaponMPR_->SetHp(hp_ * WEAPON_HP_MP);
	weaponRG_->SetHp(hp_);


	
	wave_->Init();

	stateChanges_.emplace(static_cast<int>(STATE::IDLE),std::bind(&Boss::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&Boss::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::JUMP), std::bind(&Boss::ChangeStateJump, this));
	stateChanges_.emplace(static_cast<int>(STATE::JUMPBEFORE), std::bind(&Boss::ChangeStateJumpBefore, this));
	stateChanges_.emplace(static_cast<int>(STATE::ROADATTACK), std::bind(&Boss::ChangeStateRoadAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::LASER), std::bind(&Boss::ChangeStateLaserAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&Boss::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);
	
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
	animation_->Play(static_cast<int>(ANIM_TYPE::DIR));
	weaponRG_->ChangeState(WeaponRG::STATE::IDLE);
}

void Boss::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateAttack, this);
	attackCount_ = 0;
	if (laserShotHp_ == (MAX_HP + (MAX_HP_HALF * playerSize_)))
	{
		attackCount_ = FIRST_ATTACK_INTERVAL;
	}
	animation_->Play(static_cast<int>(ANIM_TYPE::ATTACK));
	
}

void Boss::ChangeStateJump(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateJump, this);

	animation_->Play(static_cast<int>(ANIM_TYPE::JUMP));
	// ジャンプ量の計算
	float jumpSpeed = POW_JUMP_INIT * TimeManager::GetInstance().GetDeltaTime();
	jumpPow_ = jumpSpeed;
	isJump_ = true;
}

void Boss::ChangeStateJumpBefore(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateJumpBefore, this);
	animation_->Play(static_cast<int>(ANIM_TYPE::JUMPBEFORE),false);
}

void Boss::ChangeStateRoadAttack(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateRoadAttack, this);
	transform_.modelId = transformFeetCar_.modelId;
	transform_.scl = transformFeetCar_.scl;
	transform_.quaRot = transformFeetCar_.quaRot;
	transform_.Update();
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_BODY);
	transformWheelFrontL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_FRONT_L);
	transformWheelFrontR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_FRONT_R);
	transformWheelBackFrontL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_FRONT_L);
	transformWheelBackFrontR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_FRONT_R);
	transformWheelBackL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_L);
	transformWheelBackR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_R);



	transformWheelBackFrontL_.quaRot = transform_.quaRot;
	transformWheelBackFrontR_.quaRot = transform_.quaRot;
	transformWheelFrontL_.quaRot = transform_.quaRot;
	transformWheelFrontR_.quaRot = transform_.quaRot;
	transformWheelBackL_.quaRot = transform_.quaRot;
	transformWheelBackR_.quaRot = transform_.quaRot;



	transformWheelBackFrontL_.Update();
	transformWheelBackFrontR_.Update();
	transformWheelBackL_.Update();
	transformWheelBackR_.Update();
	transformWheelFrontL_.Update();
	transformWheelFrontR_.Update();
	transformBody_.Update();
	roadIsAttack_ = true;
	roadCount_ = 0;
	roadAttackTime_ = 0;

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, true);


}

void Boss::ChangeStateLaserAttack(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateStateLaserAttack , this);
	weaponRG_->ChangeState(WeaponRG::STATE::PREPARATION);

	animation_->Play(static_cast<int>(ANIM_TYPE::JUMPBEFORE), false);
	if (laserShotHp_ != (MAX_HP + (MAX_HP_HALF * playerSize_)))
	{
		laserRotSpeed_ = FIRST_LASER_ROT_SPEED;
	}
}

void Boss::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateEnd, this);
	animation_->Play(static_cast<int>(ANIM_TYPE::JUMPBEFORE), false);
	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_PLAYER_BLAST, transformBody_.pos, { 0,0,0 }, EFFECT_SCL_LASER, EFFECT_PLAEY_SPEED, this);
	SoundManager::GetInstance().Play(SoundManager::SOUND::SE_HIT_BLAST);

	weaponCannonL_->SetHp(0);
	weaponCannonR_->SetHp(0);
	weaponMGL_->SetHp(0);
	weaponMGR_->SetHp(0);
	weaponMPL_->SetHp(0);
	weaponMPR_->SetHp(0);
	weaponRG_->SetHp(0);
}
//===========================================================================================================================================================================================================================================================

void Boss::BossTransformUpdate(void)
{
	transformFeetCar_.pos = transform_.pos;
	transformFeetCar_.quaRot = transform_.quaRot;

	transform_.Update();
	transformFeetCar_.Update();

	if (state_ == STATE::ROADATTACK)
	{
		// タイヤの座標を各ジョイントに合わせる
		transformWheelFrontL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_FRONT_L);
		transformWheelFrontR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_FRONT_R);
		transformWheelBackFrontL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_FRONT_L);
		transformWheelBackFrontR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_FRONT_R);
		transformWheelBackL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_L);
		transformWheelBackR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_R);

		// タイヤの向きを本体に合わせる
		transformWheelBackFrontL_.quaRot = transform_.quaRot;
		transformWheelBackFrontR_.quaRot = transform_.quaRot;
		transformWheelFrontL_.quaRot = transform_.quaRot;
		transformWheelFrontR_.quaRot = transform_.quaRot;
		transformWheelBackL_.quaRot = transform_.quaRot;
		transformWheelBackR_.quaRot = transform_.quaRot;

		// タイヤを回転させる
		transformWheelFrontL_.quaRotLocal = Quaternion::Mult(transformWheelFrontL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WHEEL_ROT), UtilityMath::AXIS_X));
		transformWheelFrontR_.quaRotLocal = Quaternion::Mult(transformWheelFrontR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(-WHEEL_ROT), UtilityMath::AXIS_X));
		transformWheelBackFrontL_.quaRotLocal = Quaternion::Mult(transformWheelBackFrontL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WHEEL_ROT), UtilityMath::AXIS_X));
		transformWheelBackFrontR_.quaRotLocal = Quaternion::Mult(transformWheelBackFrontR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(-WHEEL_ROT), UtilityMath::AXIS_X));
		transformWheelBackL_.quaRotLocal = Quaternion::Mult(transformWheelBackL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WHEEL_ROT), UtilityMath::AXIS_X));
		transformWheelBackR_.quaRotLocal = Quaternion::Mult(transformWheelBackR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(-WHEEL_ROT), UtilityMath::AXIS_X));
	}

	transformWheelBackFrontL_.Update();
	transformWheelBackFrontR_.Update();
	transformWheelBackL_.Update();
	transformWheelBackR_.Update();
	transformWheelFrontL_.Update();
	transformWheelFrontR_.Update();

	transformBody_.Update();
	BoneParam();
	
	WeaponSet();

	WeaponUpdate();

	SoundManager::GetInstance().Set3DPosition(SoundManager::SOUND::SE_BOSS_LANDING, transform_.pos);
	SoundManager::GetInstance().Set3DPosition(SoundManager::SOUND::SE_BOSS_ROAD, transform_.pos);
}

void Boss::UpdateProcess(void)
{	
	// カメラの追従対象に登録
	const std::unique_ptr<Camera>& camera = SceneManager::GetInstance().GetCamera();
	
	cameraPos_ = camera->GetPos();

	if (transform_.pos.y < DOWU_POS)
	{
		transform_.pos = POP_POS;
	}
	


	if (hp_ <= 0)
	{
		
		if (state_ != STATE::END)
		{
			if (isHostControl_)
			{
				ChangeState(STATE::END);
			}
		}
	}

	if (isHostControl_)
	{
		stateUpdate_();

		if (weaponMGL_->IsAttack() == true || weaponMGR_->IsAttack() == true)
		{
			if (SoundManager::GetInstance().IsPlaying(SoundManager::SOUND::SE_BOSS_MG_FIRE) == false)
			{
				isMGFire_ = true;
			}
		}
	}

	isLanging_ = false;
	isMGFire_ = false;
	isRoadFire_ = false;

	currentWaveScl = VAdd(currentWaveScl, WAVE_SCL_UP);
	EffectManager::GetInstance().UpdateScl(EffectManager::EFFECT::EFFECT_WAVE, this, currentWaveScl);

	BossTransformUpdate();

	wave_->SetPos(transform_.pos);
	wave_->Update();

	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_BODY, transformBody_.pos, hp_);
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_MGL_L, weaponMGL_->GetPos(), weaponMGL_->GetHp());
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_MGL_R, weaponMGR_->GetPos(), weaponMGR_->GetHp());
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_CANNON_L, weaponCannonL_->GetPos(), weaponCannonL_->GetHp());
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_CANNON_R, weaponCannonR_->GetPos(), weaponCannonR_->GetHp());
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_MP_L, weaponMPL_->GetPos(), weaponMPL_->GetHp());
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_MP_R, weaponMPR_->GetPos(), weaponMPR_->GetHp());
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_RG, weaponRG_->GetPos(), weaponRG_->GetHp());

}

void Boss::UpdateProcessPost(void)
{
	UpdateEffect();
}

//各ステイトのアップデート関数＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝－
void Boss::UpdateIdle(void)
{
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	LookPlayer();
	attackCount_++;
	if (hp_ <= laserShotHp_ && attackCount_ >= attackInterval_)
	{
		ChangeState(STATE::LASER);
		if (laserShotHp_== (MAX_HP + (MAX_HP_HALF * playerSize_)))
		{
			laserShotHp_ = (MAX_HP + (MAX_HP_HALF * playerSize_)) / HALF;
			
		}
		else if (laserShotHp_ == ((MAX_HP + (MAX_HP_HALF * playerSize_)) / HALF))
		{
			laserShotHp_ = (MAX_HP + (MAX_HP_HALF * playerSize_)) * LASER_END;
			
		}
		else
		{
			laserShotHp_ = -MAX_HP_HALF;
		}
		
	}
	else if (attackCount_ >= attackInterval_)
	{
		ChangeState(STATE::ATTACK);
	}
}

void Boss::UpdateAttack(void)
{
	LookPlayer();
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);

	if (static_cast<int>(ATTACK_TYPE::MAX) > 1)
	{
		do
		{
			int randomAttack = static_cast<int>(UtilityMath::RandRangeF(0.0f, static_cast<float>(ATTACK_TYPE::MAX)));
			 attackSelect_ = static_cast<ATTACK_TYPE>(randomAttack);
		} 
		while (attackSelect_ == lastAttackType_); // 前回と同じ攻撃ならもう一度振り直す！
	}

	// 今回選ばれた攻撃を「前回の攻撃」として保存しておく
	lastAttackType_ = attackSelect_;

	 //attackSelect_ = ATTACK_TYPE::ROAD;

	switch (attackSelect_)
	{
	case ATTACK_TYPE::JUMP:
		ChangeState(STATE::JUMPBEFORE);
		break;

	case ATTACK_TYPE::MG:
		if (weaponMGL_->GetIsAlive()==true)weaponMGL_->ChangeState(WeaponMGL::STATE::ATTACK);
		
		if (weaponMGR_->GetIsAlive() == true)weaponMGR_->ChangeState(WeaponMGR::STATE::ATTACK);
		
		isMGFire_ = true;
		ChangeState(STATE::IDLE);
		break;

	case ATTACK_TYPE::ROAD:
		ChangeState(STATE::ROADATTACK);
		break;

	case ATTACK_TYPE::CANNON:
		if(weaponCannonL_->GetIsAlive()==true)weaponCannonL_->ChangeState(WeaponCannon::STATE::ATTACK);
		if (weaponCannonR_->GetIsAlive() == true)weaponCannonR_->ChangeState(WeaponCannon::STATE::ATTACK);
		ChangeState(STATE::IDLE);
		break;
	case ATTACK_TYPE::MISSILE:
		if (weaponMPL_->GetIsAlive() == true)
		{
			weaponMPL_->ChangeState(WeaponMP::STATE::ATTACK);
			weaponMPL_->IsLR(true);
		}
		if (weaponMPR_->GetIsAlive() == true)
		{
			weaponMPR_->ChangeState(WeaponMP::STATE::ATTACK);
			weaponMPR_->IsLR(false);
		}
		ChangeState(STATE::IDLE);
		break;
	default:
		break;
	}

}

void Boss::UpdateJump(void)
{
	LookPlayer();
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	if (!isJump_)
	{
		wave_->SetIsAttack(true);
		isLanging_ = true;
		currentWaveScl = WAVE_SCL;
		EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_WAVE, transform_.pos, currentWaveScl, LANDING_SCL, EFFECT_PLAEY_SPEED,this);
		EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_LANDING, transform_.pos, { 0.0f,0.0f,0.0f }, LANDING_SCL, EFFECT_PLAEY_SPEED,this);
		ChangeState(STATE::IDLE);
	}
	else if (isJump_)
	{
		if (transform_.pos.y >= JUMP_MAX_UP)
		{
			jumpPow_ = POW_JUMP_DOUN;
		}

		if (jumpPow_ >= POW_JUMP_DOUN)
		{
			VECTOR movePow = VScale(jumpDir_, speed_);
			// 移動処理
			transform_.pos = VAdd(transform_.pos, movePow);
		}
	}


}

void Boss::UpdateJumpBefore(void)
{
	LookPlayer();
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	if (animation_->IsEnd() == true)
	{
		ChangeState(STATE::JUMP);
	}
}

void Boss::UpdateRoadAttack(void)
{
	SoundManager::GetInstance().Set3DPosition(SoundManager::SOUND::SE_BOSS_ROAD, transform_.pos);
	
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_BODY);
	/* マルチのためコメント化
	transformWheelFrontL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_FRONT_L);
	transformWheelFrontR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_FRONT_R);
	transformWheelBackFrontL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_FRONT_L);
	transformWheelBackFrontR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_FRONT_R);
	transformWheelBackL_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_L);
	transformWheelBackR_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_WHEEL_BACK_R);


	transformWheelFrontL_.quaRotLocal = Quaternion::Mult(transformWheelFrontL_.quaRotLocal,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(WHEEL_ROT), UtilityMath::AXIS_X));
	transformWheelFrontR_.quaRotLocal = Quaternion::Mult(transformWheelFrontR_.quaRotLocal,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(-WHEEL_ROT), UtilityMath::AXIS_X));
	transformWheelBackFrontL_.quaRotLocal = Quaternion::Mult(transformWheelBackFrontL_.quaRotLocal,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(WHEEL_ROT), UtilityMath::AXIS_X));
	transformWheelBackFrontR_.quaRotLocal = Quaternion::Mult(transformWheelBackFrontR_.quaRotLocal,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(-WHEEL_ROT), UtilityMath::AXIS_X));
	transformWheelBackL_.quaRotLocal = Quaternion::Mult(transformWheelBackL_.quaRotLocal,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(WHEEL_ROT), UtilityMath::AXIS_X));
	transformWheelBackR_.quaRotLocal = Quaternion::Mult(transformWheelBackR_.quaRotLocal,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(-WHEEL_ROT), UtilityMath::AXIS_X));
	*/
	if (!roadIsAttack_)
	{
		LookPlayer();

		roadLockTime_++;
		
		transform_.quaRot = transformFeetCar_.quaRot;

		/*　マルチのためコメント化
		transformWheelBackFrontL_.quaRot = transform_.quaRot;
		transformWheelBackFrontR_.quaRot = transform_.quaRot;
		transformWheelFrontL_.quaRot = transform_.quaRot;
		transformWheelFrontR_.quaRot = transform_.quaRot;
		transformWheelBackL_.quaRot = transform_.quaRot;
		transformWheelBackR_.quaRot = transform_.quaRot;
		*/
		

		if (roadLockTime_ >= MAX_ROAD_LOCK_TIME)
		{
			roadLockTime_ = 0;
			roadIsAttack_ = true;
			CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, true);
		}
		

	}
	if (roadIsAttack_)
	{

		isRoadFire_ = true;
		if (roadAttackTime_ >= 1)
		{
			isRoadFire_ = false;
		}
		speed_ = MOVE_SPEED_ROAD;
		VECTOR movePow = VScale(roadDir_, speed_);
		// 移動処理
		transform_.pos = VAdd(transform_.pos, movePow);
		transform_.Update();
		bool hitWall = CollisionController::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::ROAD_ATTACK, ColliderBase::TAG::WALL);
		if (hitWall==true)
		{
			roadAttackTime_ = 0;
			roadIsAttack_ = false;
			CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, false);
		}

		wallStopPos_ = transform_.pos;
		
		roadAttackTime_++;
		if (roadAttackTime_ >= MAX_ROAD_ATTACK_TIME)
		{
			roadCount_++;
			roadAttackTime_ = 0;
			roadIsAttack_ = false;
			CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, false);
		}
	}

	if (roadCount_ >= MAX_ROAD_COUNT)
	{
		transform_.modelId = transformFeet_.modelId;
		transform_.scl = transformFeet_.scl;
		CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, false);
		SoundManager::GetInstance().Stop(SoundManager::SOUND::SE_BOSS_ROAD);
		ChangeState(STATE::IDLE);
	}



}

void Boss::UpdateStateLaserAttack(void)
{

	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	if (weaponRG_->GetIsAttack() == true)
	{
		laserAttackRot_ += laserRotSpeed_;
		if (laserAttackRot_ >= LASER_MAX_ROT)
		{
			laserAttackRot_ = 0.0f;
			weaponRG_->ChangeState(WeaponRG::STATE::IDLE);
			
			ChangeState(STATE::IDLE);
		}
		transformBody_.quaRot = Quaternion::Mult(transformBody_.quaRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(laserRotSpeed_), UtilityMath::AXIS_Y));
	}
	
}

void Boss::UpdateEnd(void)
{
	weaponCannonL_->SetHp(0);
	weaponCannonR_->SetHp(0);
	weaponMGL_->SetHp(0);
	weaponMGR_->SetHp(0);
	weaponMPL_->SetHp(0);
	weaponMPR_->SetHp(0);
	weaponRG_->SetHp(0);

	if (endCount_ >= END_MAX_COUNT)
	{
		speed_ = MOVE_SPEED;
		
		VECTOR movePow = VScale(roadDir_, speed_);
		transformBody_.pos = VAdd(transformBody_.pos, movePow);
		transformBody_.Update();
	}
	else if (endCount_>= END_COUNT)
	{
		
		weaponCannonL_->ChangeState(WeaponCannon::STATE::END);
		weaponCannonR_->ChangeState(WeaponCannon::STATE::END);
		weaponMGL_->ChangeState(WeaponMGL::STATE::END);
		weaponMGR_->ChangeState(WeaponMGR::STATE::END);
		weaponMPL_->ChangeState(WeaponMP::STATE::END);
		weaponMPR_->ChangeState(WeaponMP::STATE::END);
		weaponRG_->ChangeState(WeaponRG::STATE::END);

		
	}



	endCount_++;
}

//===========================================================================================================================================================================================================================================================



void Boss::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
	MV1DrawModel(transformBody_.modelId);
	if (state_ == STATE::ROADATTACK)
	{
		MV1DrawModel(transformWheelBackFrontL_.modelId);
		MV1DrawModel(transformWheelBackFrontR_.modelId);
		MV1DrawModel(transformWheelBackL_.modelId);
		MV1DrawModel(transformWheelBackR_.modelId);
		MV1DrawModel(transformWheelFrontL_.modelId);
		MV1DrawModel(transformWheelFrontR_.modelId);
	}

	WeaponDraw();

	wave_->Draw();

	CharaBase::DrawShadowRound(200.0f);

#ifdef _DEBUG
	DrawFormatString(10, 400, 0xffffff, "hp:%d", hp_);
#endif
}




//機能関数
void Boss::LookPlayer(void)
{
	// 突進「中」は、すでに決まった `roadDir_` に向かって進むので、振り向かない
	if (state_ == STATE::ROADATTACK && roadIsAttack_)
	{
		return;
	}

	if (isHostControl_)
	{
		int gameTime = TimeManager::GetInstance().GetGameTime();
		if (static_cast<int>(gameTime) >= nextChangeMainTime_)
		{

			mainIdx_ = std::rand() % playerSize_;
			nextChangeMainTime_ = static_cast<int>(gameTime) + INTERVAL_SEC;
		}
	}
	
	mainPos_ = playerPos_[mainIdx_];
	VECTOR moveDir = VSub(mainPos_, transformBody_.pos);
	moveDir.y = 0.0f;
	moveDir = VNorm(moveDir);
	float targetAngle = atan2(moveDir.x, moveDir.z);
	transformBody_.quaRot = Quaternion::AngleAxis(targetAngle, UtilityMath::AXIS_Y);
	transformFeetCar_.quaRot = Quaternion::AngleAxis(targetAngle, UtilityMath::AXIS_Y);

	// 突進していない（準備中）なら、突進方向を常にプレイヤーに向ける
	jumpDir_ = moveDir;
	roadDir_ = moveDir;

}


//ウェポンの呼び出し纏めよう＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Boss::WeaponSet(void)
{
	// 各武器にボーン情報を設定（ここはそのまま）
	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform, ColliderBase::TAG::WEAPON_MG_L, mainPos_);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform, ColliderBase::TAG::WEAPON_MG_R, mainPos_);

	int gameTime = TimeManager::GetInstance().GetGameTime();

	if (isHostControl_)
	{
		int gameTime = TimeManager::GetInstance().GetGameTime();
		if (static_cast<int>(gameTime) >= nextChangeMpTime_)
		{

			mpIdx_ = std::rand() % playerSize_;
			nextChangeMpTime_ = static_cast<int>(gameTime) + INTERVAL_SEC_MP;
		}
	}
	
	mpPos_ = playerPos_[mpIdx_];


	weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform, ColliderBase::TAG::WEAPON_MP_L, mpPos_);
	weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform, ColliderBase::TAG::WEAPON_MP_R, mainPos_);

	weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform, ColliderBase::TAG::WEAPON_RG, mainPos_);

	if (isHostControl_)
	{
		int gameTime = TimeManager::GetInstance().GetGameTime();
		if (static_cast<int>(gameTime) >= nextChangeCannonTime_)
		{

			cannonIdx_ = std::rand() % playerSize_;
			nextChangeCannonTime_ = static_cast<int>(gameTime) + INTERVAL_SEC_CANNON;
		}
	}
	
	CannonPos_ = playerPos_[cannonIdx_];
	weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform, ColliderBase::TAG::WEAPON_CANNON_L, mainPos_);
	weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform, ColliderBase::TAG::WEAPON_CANNON_R, CannonPos_);
}

void Boss::WeaponLoad(void)
{
	weaponMGL_->Load();
	weaponMGR_->Load();
	weaponMPL_->Load();
	weaponMPR_->Load();
	weaponRG_->Load();
	weaponCannonL_->Load();
	weaponCannonR_->Load();
}

void Boss::WeaponInit(void)
{
	weaponMGL_->Init();
	weaponMGR_->Init();
	weaponMPL_->Init();
	weaponMPR_->Init();
	weaponRG_->Init();
	weaponCannonL_->Init();
	weaponCannonR_->Init();
}

void Boss::WeaponUpdate(void)
{
	weaponMGL_->Update();
	weaponMGR_->Update();
	weaponMPL_->Update();
	weaponMPR_->Update();
	weaponRG_->Update();
	weaponCannonL_->Update();
	weaponCannonR_->Update();
}

void Boss::WeaponDraw(void)
{
	weaponMGL_->Draw();
	weaponMGR_->Draw();
	weaponMPL_->Draw();
	weaponMPR_->Draw();
	weaponRG_->Draw();
	weaponCannonL_->Draw();
	weaponCannonR_->Draw();
}


/* マルチプレイ用関数 */

void Boss::SetHostControl(bool _isHostControl)
{
	isHostControl_ = _isHostControl;
}

NET_BOSS_ACTION Boss::GetNetworkAction(void) const
{
	NET_BOSS_ACTION action;
	action.position = transform_.pos;
	action.rotation = transformBody_.quaRot;
	action.bossHp = hp_;
	action.animationId = static_cast<int>(state_);
	action.targetPlayerId = mainIdx_;

	action.missilePodTargetId = mpIdx_;
	action.cannonTargetId = cannonIdx_;

	// 追加：最後に選ばれた攻撃のタイプを送信する
	action.attackSelect = static_cast<int>(lastAttackType_);

	action.weaponMachineGunLeftHp = weaponMGL_->GetHp();
	action.weaponMachineGunRightHp = weaponMGR_->GetHp();
	action.weaponMissilePodLeftHp = weaponMPL_->GetHp();
	action.weaponMissilePodRightHp = weaponMPR_->GetHp();
	action.weaponRailGunHp = weaponRG_->GetHp();
	action.weaponCannonLeftHp = weaponCannonL_->GetHp();
	action.weaponCannonRightHp = weaponCannonR_->GetHp();

	return action;
}

void Boss::SetNetworkAction(const NET_BOSS_ACTION& _action)
{
	const int PREV_HP = hp_;

	hp_ = _action.bossHp;

	const int difference = PREV_HP - hp_;

	transform_.pos = _action.position;

	transformBody_.quaRot = _action.rotation;

	if (difference > EFFECT_PLAEY_DAMEGE)
	{
		PlayEffect();
	}

	if (_action.missilePodTargetId >= 0 && _action.missilePodTargetId < playerSize_)
	{
		mpIdx_ = _action.missilePodTargetId;
	}
	if (_action.cannonTargetId >= 0 && _action.cannonTargetId < playerSize_)
	{
		cannonIdx_ = _action.cannonTargetId;
	}

	// クライアント側のウェポンHPをホストと同期する
	weaponMGL_->SetHp(_action.weaponMachineGunLeftHp);
	weaponMGR_->SetHp(_action.weaponMachineGunRightHp);
	weaponMPL_->SetHp(_action.weaponMissilePodLeftHp);
	weaponMPR_->SetHp(_action.weaponMissilePodRightHp);
	weaponRG_->SetHp(_action.weaponRailGunHp);
	weaponCannonL_->SetHp(_action.weaponCannonLeftHp);
	weaponCannonR_->SetHp(_action.weaponCannonRightHp);

	// クライアント側の場合、攻撃の変更を検知して武器を発射する
	if (isHostControl_ == false)
	{
		if (_action.attackSelect != static_cast<int>(lastAttackType_))
		{
			lastAttackType_ = static_cast<ATTACK_TYPE>(_action.attackSelect);

			switch (lastAttackType_)
			{
			case ATTACK_TYPE::MG:
				if (weaponMGL_->GetIsAlive() == true)
				{
					weaponMGL_->ChangeState(WeaponMGL::STATE::ATTACK);
				}
				if (weaponMGR_->GetIsAlive() == true)
				{
					weaponMGR_->ChangeState(WeaponMGR::STATE::ATTACK);
				}
				break;

			case ATTACK_TYPE::CANNON:
				if (weaponCannonL_->GetIsAlive() == true)
				{
					weaponCannonL_->ChangeState(WeaponCannon::STATE::ATTACK);
				}
				if (weaponCannonR_->GetIsAlive() == true)
				{
					weaponCannonR_->ChangeState(WeaponCannon::STATE::ATTACK);
				}
				break;

			case ATTACK_TYPE::MISSILE:
				if (weaponMPL_->GetIsAlive() == true)
				{
					weaponMPL_->ChangeState(WeaponMP::STATE::ATTACK);
					weaponMPL_->IsLR(true);
				}
				if (weaponMPR_->GetIsAlive() == true)
				{
					weaponMPR_->ChangeState(WeaponMP::STATE::ATTACK);
					weaponMPR_->IsLR(false);
				}
				break;
			default:
				break;
			}
		}
	}
	if (static_cast<int>(state_) != _action.animationId)
	{
		// ジャンプからIDLEに戻った瞬間の処理
		if (state_ == STATE::JUMP)
		{
			if (static_cast<STATE>(_action.animationId) == STATE::IDLE)
			{
				currentWaveScl = WAVE_SCL;
				EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_WAVE, transform_.pos, currentWaveScl, LANDING_SCL, EFFECT_PLAEY_SPEED, this);
				EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_LANDING, transform_.pos, { 0.0f, 0.0f, 0.0f }, LANDING_SCL, EFFECT_PLAEY_SPEED, this);

				// クライアント側でも衝撃波の当たり判定を有効にする
				wave_->SetIsAttack(true);
			}
		}

		// 突進から他のステートに戻った瞬間の処理
		if (state_ == STATE::ROADATTACK)
		{
			if (static_cast<STATE>(_action.animationId) != STATE::ROADATTACK)
			{
				// モデルとスケールを元の足に戻す
				transform_.modelId = transformFeet_.modelId;
				transform_.scl = transformFeet_.scl;

				// 突進の当たり判定をオフにする
				CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, false);

				// 突進のSEを止める
				SoundManager::GetInstance().Stop(SoundManager::SOUND::SE_BOSS_ROAD);
			}
		}

		ChangeState(static_cast<STATE>(_action.animationId));
	}

	if (_action.targetPlayerId >= 0 && _action.targetPlayerId < playerSize_)
	{
		mainIdx_ = _action.targetPlayerId;
	}

	transform_.Update();
	if (state_ == STATE::ROADATTACK)
	{
		transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_BODY);
	}
	else
	{
		transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	}
}
