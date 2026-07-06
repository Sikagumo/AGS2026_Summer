#include <DxLib.h>
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Manager/Decoration/SoundManager.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderCapsule.h"
#include "../../../../../Collider/ColliderLine.h"
#include "../../../../../Collision/CollisionController.h"
#include "../../Bullet/Boss/BBulletMG.h"
#include "WeaponMGR.h"

WeaponMGR::WeaponMGR():MGBase()
{
	
}





void WeaponMGR::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_MG_R));
	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_MG_FIRE, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_MG_FIRE));
}

void WeaponMGR::ReleasePost(void)
{
}



void WeaponMGR::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMGR::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::SHAPE::LINE)].push_back(colLine);


	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	ownColliders_[static_cast<int>(tag_)].push_back(colCapsule);
	colCapsule->SetTriger(false);

	CollisionController::GetInstance().RegisterActor(this);
}

void WeaponMGR::InitAnimation(void)
{
}

void WeaponMGR::InitPost(void)
{
	isAlive_ = true;
	localPos_ = LINE_START_POS;
	look = 1;
	for (int i = 0; i < MUZZLE_MAX_COUNT; ++i)
	{
		muzzlePos_[i] = MUZZLE_POS[i];
	}
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&WeaponMGR::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&WeaponMGR::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&WeaponMGR::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);
}

void WeaponMGR::UpdateProcess(void)
{
	
	MGBase::UpdateCommon();
}

void WeaponMGR::UpdateProcessPost(void)
{
	
}



void WeaponMGR::DrawPre(void)
{

	
	for (std::shared_ptr<BBulletBase> shot : bullets_)
	{
		if (shot->GetIsAlive() == true)
		{
			shot->Draw();
		}
		
	}

#ifdef _DEBUG


	DrawFormatString(10, 250, 0xffffff, "MG_L_Bullet%d", bullets_.size());


	

#endif
}

