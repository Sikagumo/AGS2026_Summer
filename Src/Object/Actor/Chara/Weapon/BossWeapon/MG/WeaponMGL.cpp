#include <DxLib.h>
#include <algorithm>
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Manager/Decoration/SoundManager.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderCapsule.h"
#include "../../../../../Collider/ColliderLine.h"
#include "../../../../../Collision/CollisionController.h"
#include "../../Bullet/Boss/BBulletMG.h"
#include "WeaponMGL.h"

WeaponMGL::WeaponMGL()
	: MGBase()
{
	
}

void WeaponMGL::ReleasePost(void)
{
}



void WeaponMGL::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_MG_L));

	

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_BOSS_MG_FIRE, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_BOSS_MG_FIRE));
}

void WeaponMGL::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = bone_.transform.quaRot;
	transform_.quaRotLocal=Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y);

	transform_.pos= MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMGL::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::STAGE)].push_back( colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	ownColliders_[static_cast<int>(tag_)].push_back(colCapsule);
	colCapsule->SetTriger(false);

	CollisionController::GetInstance().RegisterActor(this);

}

void WeaponMGL::InitAnimation(void)
{
}

void WeaponMGL::InitPost(void)
{
	hp_ = 1250;
	isAlive_ = true;
	localPos_ = LINE_START_POS;
	look = -1;
	for (int i = 0; i < MUZZLE_MAX_COUNT; ++i)
	{
		muzzlePos_[i] = MUZZLE_POS[i];
	}
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&WeaponMGL::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&WeaponMGL::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&WeaponMGL::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);
}

void WeaponMGL::UpdateProcess(void)
{
	MGBase::UpdateCommon();

}

void WeaponMGL::UpdateProcessPost(void)
{
}



void WeaponMGL::DrawPre(void)
{

	for (std::shared_ptr<BBulletBase> shot : bullets_)
	{
		if (shot->GetIsAlive()==true)
		{
			shot->Draw();
		}
	}
#ifdef _DEBUG
	

	
	DrawFormatString(10, 270, 0xffffff, "MG_L_Bullet%d", bullets_.size());
	DrawFormatString(10, 300, 0xffffff, "MG_L_ROT%f,%f,%f", transform_.quaRot.x, transform_.quaRot.y, transform_.quaRot.z);

#endif
}



