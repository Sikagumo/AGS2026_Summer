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
	               
	hp_(1000),
	boneName_(BONE_NAME::WEAPON_JOINT_MGL_L), 
	jumpDir_({ 0.0f, 0.0f, 0.0f }),          
	speed_(MOVE_SPEED_INIT),
	jumpCount_(0),                           
	attackCount_(0),                         
	player1Pos_({ 0.0f, 0.0f, 0.0f }),      
	player2Pos_({ 0.0f, 0.0f, 0.0f }),       
	player3Pos_({ 0.0f, 0.0f, 0.0f }),       
	player4Pos_({ 0.0f, 0.0f, 0.0f }),       
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

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_ROAD, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_ROAD));

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
	transformFeetCar_.scl = { 5.0f,5.0f,5.0f };
	transformFeetCar_.quaRot = Quaternion::Identity();
	transformFeetCar_.quaRotLocal =
		Quaternion::Mult(transformFeetCar_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformFeetCar_.pos = transform_.pos;
	transformFeetCar_.Update();

	//前輪L
	transformWheelFrontL_.scl = { 5.0f,5.0f,5.0f };
	transformWheelFrontL_.quaRot = Quaternion::Identity();
	transformWheelFrontL_.quaRotLocal =
		Quaternion::Mult(transformWheelFrontL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformWheelFrontL_.Update();
	//前輪R
	transformWheelFrontR_.scl = { 5.0f,5.0f,5.0f };
	transformWheelFrontR_.quaRot = Quaternion::Identity();
	transformWheelFrontR_.quaRotLocal =
		Quaternion::Mult(transformWheelFrontR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(0.0f), UtilityMath::AXIS_Y));
	transformWheelFrontR_.Update();
	//後輪前L
	transformWheelBackFrontL_.scl = { 5.0f,5.0f,5.0f };
	transformWheelBackFrontL_.quaRot = Quaternion::Identity();
	transformWheelBackFrontL_.quaRotLocal =
		Quaternion::Mult(transformWheelBackFrontL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformWheelBackFrontL_.Update();
	//後輪前R
	transformWheelBackFrontR_.scl = { 5.0f,5.0f,5.0f };
	transformWheelBackFrontR_.quaRot = Quaternion::Identity();
	transformWheelBackFrontR_.quaRotLocal =
		Quaternion::Mult(transformWheelBackFrontR_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(0.0f), UtilityMath::AXIS_Y));
	transformWheelBackFrontR_.Update();
	//後輪L
	transformWheelBackL_.scl = { 5.0f,5.0f,5.0f };
	transformWheelBackL_.quaRot = Quaternion::Identity();
	transformWheelBackL_.quaRotLocal =
		Quaternion::Mult(transformWheelBackL_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformWheelBackL_.Update();
	//後輪R
	transformWheelBackR_.scl = { 5.0f,5.0f,5.0f };
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
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::BOSS, &transform_, COL_CAPSULE_START_POS, COL_CAPSULE_END_POS, COL_CAPSULE_END_RADIUS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
	colCapsule->SetTriger(false);

	ColliderSphere* colSphere = new ColliderSphere(ColliderBase::TAG::ROAD_ATTACK, &transform_, { 0.0f,0.0f,0.0f }, 200.0f);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colSphere);
	

	

	CollisionController::GetInstance().RegisterActor(this);

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ROAD_ATTACK, false);


}

void Boss::InitAnimation(void)
{
	CharaBase::InitAnimation();
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animation_->AddInternal(i, 20.0f);
	}
	animation_->Play(static_cast<int>(ANIM_TYPE::DIR));

	//EffectManager::GetInstance().Add(EffectManager::EFFECT::EFFECT_WAVE, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_WAVE));
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
	stateChanges_.emplace(static_cast<int>(STATE::JUMPBEFORE), std::bind(&Boss::ChangeStateJumpBefore, this));
	stateChanges_.emplace(static_cast<int>(STATE::ROADATTACK), std::bind(&Boss::ChangeStateRoadAttack, this));
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
}

void Boss::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateAttack, this);
	attackCount_ = 0;
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

void Boss::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&Boss::UpdateEnd, this);
}
//===========================================================================================================================================================================================================================================================

void Boss::BossTransformUpdate(void)
{

	LookPlayer();

	transform_.Update();
	transformFeetCar_.Update();
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
	SoundManager::GetInstance().Set3DPosition(SoundManager::SOUND::SE_ROAD, transform_.pos);
}

void Boss::UpdateProcess(void)
{	
	

	isLanging_ = false;
	isMGFire_ = false;
	if (weaponMGL_->IsAttack() == true || weaponMGR_->IsAttack() == true)
	{
		if (SoundManager::GetInstance().IsPlaying(SoundManager::SOUND::SE_MG_FIRE) == false)
		{
			isMGFire_ = true;
		}
	}
	isRoadFire_ = false;

	stateUpdate_();

	BossTransformUpdate();
	
	wave_->SetPos(transform_.pos);
	wave_->Update();
	
	

	// カメラの追従対象に登録
	const std::unique_ptr<Camera>& camera = SceneManager::GetInstance().GetCamera();
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_BODY, transformBody_.pos, hp_);
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_MGL_L, weaponMGL_->GetPos(), weaponMGL_->GetHp());
	camera->SetLockOnTargets(Camera::LOCKON_TARGET::BOSS_WEAPON_MGL_R, weaponMGR_->GetPos(), weaponMGR_->GetHp());
}

void Boss::UpdateProcessPost(void)
{
	
}

//各ステイトのアップデート関数＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝－
void Boss::UpdateIdle(void)
{
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	attackCount_++;
	if (attackCount_ >= attackInterval_)
	{
		ChangeState(STATE::ATTACK);
	}
}

void Boss::UpdateAttack(void)
{

	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	int randomAttack = static_cast<int>(UtilityMath::RandRangeF(0.0f, static_cast<float>(ATTACK_TYPE::MAX)));
	ATTACK_TYPE attackSelect = /*ATTACK_TYPE::CANNON;*/static_cast<ATTACK_TYPE>(randomAttack);

	switch (attackSelect)
	{
	case ATTACK_TYPE::JUMP:
		ChangeState(STATE::JUMPBEFORE);
		break;

	case ATTACK_TYPE::MG:
		weaponMGL_->ChangeState(WeaponMGL::STATE::ATTACK);
		weaponMGR_->ChangeState(WeaponMGR::STATE::ATTACK);
		isMGFire_ = true;
		ChangeState(STATE::IDLE);
		break;

	case ATTACK_TYPE::ROAD:
		ChangeState(STATE::ROADATTACK);
		break;

	case ATTACK_TYPE::CANNON:
		weaponCannonL_->ChangeState(WeaponCannon::STATE::ATTACK);
		weaponCannonR_->ChangeState(WeaponCannon::STATE::ATTACK);
		ChangeState(STATE::IDLE);
		break;

	default:
		break;
	}

}

void Boss::UpdateJump(void)
{
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	if (!isJump_)
	{
		wave_->SetIsAttack(true);
		isLanging_ = true;
		EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_WAVE, transform_.pos, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 1.0f);
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
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_FEET_BODY);
	if (animation_->IsEnd() == true)
	{
		ChangeState(STATE::JUMP);
	}
}

void Boss::UpdateRoadAttack(void)
{
	SoundManager::GetInstance().Set3DPosition(SoundManager::SOUND::SE_ROAD, transform_.pos);
	
	transformBody_.pos = MV1GetFramePosition(transform_.modelId, JOINT_CAR_BODY);
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

	if (!roadIsAttack_)
	{
		roadLockTime_++;
		
		transform_.quaRot = transformFeetCar_.quaRot;
		transformWheelBackFrontL_.quaRot = transform_.quaRot;
		transformWheelBackFrontR_.quaRot = transform_.quaRot;
		transformWheelFrontL_.quaRot = transform_.quaRot;
		transformWheelFrontR_.quaRot = transform_.quaRot;
		transformWheelBackL_.quaRot = transform_.quaRot;
		transformWheelBackR_.quaRot = transform_.quaRot;
		
		

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
		SoundManager::GetInstance().Stop(SoundManager::SOUND::SE_ROAD);
		ChangeState(STATE::IDLE);
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


#ifdef _DEBUG


	DrawFormatString(10, 100, 0xffffff, "bossの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
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
	
	VECTOR moveDir = VSub(player1Pos_, transformBody_.pos);
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
	weaponMGL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_L)].transform, ColliderBase::TAG::WEAPON_MG_L,player1Pos_);
	weaponMGR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MGL_R)].transform, ColliderBase::TAG::WEAPON_MG_R,player1Pos_);
	weaponMPL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_L)].transform, ColliderBase::TAG::WEAPON_MP_L,player1Pos_);
	weaponMPR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_MP_R)].transform, ColliderBase::TAG::WEAPON_MP_R,player1Pos_);
	weaponRG_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_RG)].transform, ColliderBase::TAG::WEAPON_RG,player1Pos_);
	weaponCannonL_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_L)].transform, ColliderBase::TAG::WEAPON_CANNON_L,player1Pos_);
	weaponCannonR_->SetBone(boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].id, boneId_[static_cast<int>(BONE_NAME::WEAPON_JOINT_CANNON_R)].transform, ColliderBase::TAG::WEAPON_CANNON_R,player1Pos_);
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
