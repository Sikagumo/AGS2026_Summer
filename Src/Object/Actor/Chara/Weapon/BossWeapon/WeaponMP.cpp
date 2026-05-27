#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderSphere.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "WeaponMP.h"

WeaponMP::WeaponMP(void)
{
	
}

void WeaponMP::ReleasePost(void)
{
}

void WeaponMP::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag)
{
	bone_.id = _id;
	bone_.transform = _trans;
	tag_ = _tag;
}

VECTOR WeaponMP::GetPos(void) const
{
	return transform_.pos;
}

void WeaponMP::Load(void)
{
	transform_.SetModel(resourceManager_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_RK));
}


void WeaponMP::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(0), UtilityMath::AXIS_Y));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMP::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(tag_, &transform_, { 0.0f,0.0f,-40.0f }, { 0.0f,-1.0f,-40.0f });
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);


	ColliderSphere* colSphere = new ColliderSphere(
		tag_, &transform_, { 0.0f,0.0f,-40.0f },40.0f);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), colSphere);
	colSphere->SetTriger(false);

	CollisionManager::GetInstance().RegisterActor(this);
}

void WeaponMP::InitAnimation(void)
{
}

void WeaponMP::InitPost(void)
{
	isAlive_ = true;
}

void WeaponMP::UpdateProcess(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
	if (hp_ <= 0)
	{
		isAlive_ = false;
	}
}

void WeaponMP::UpdateProcessPost(void)
{
}



void WeaponMP::DrawPre(void)
{
	if (isAlive_)
	{
		
		for (auto& col : ownColliders_)
		{
			col.second->Draw();
		}
	}
}
