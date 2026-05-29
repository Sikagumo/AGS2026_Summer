#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderCapsule.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "WeaponMGL.h"

WeaponMGL::WeaponMGL()
{
	
}

void WeaponMGL::ReleasePost(void)
{
}

void WeaponMGL::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag)
{
	bone_.id = _id;
	bone_.transform = _trans;
	tag_ = _tag;
}

const VECTOR WeaponMGL::GetPos(void) const
{
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(localPos_);

	// 位置を加算して最終的なワールド座標にする
	return VAdd(transform_.pos, localRotPos);
}

void WeaponMGL::Load(void)
{
	transform_.SetModel(resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_MG_L));
}

void WeaponMGL::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));
	transform_.quaRotLocal=
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));

	transform_.pos= MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponMGL::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::CAPSULE), colCapsule);
	colCapsule->SetTriger(false);

	CollisionManager::GetInstance().RegisterActor(this);

}

void WeaponMGL::InitAnimation(void)
{
}

void WeaponMGL::InitPost(void)
{
	isAlive_ = true;
	localPos_ = LINE_START_POS;
}

void WeaponMGL::UpdateProcess(void)
{
	if (isAlive_)
	{
		transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	}
	if (hp_ <= 0)
	{
		isAlive_ = false;
		CollisionManager::GetInstance().SetCollisionActive(this, tag_, false);
	}
}

void WeaponMGL::UpdateProcessPost(void)
{
}



void WeaponMGL::DrawPre(void)
{
#ifdef _DEBUG
	if (isAlive_)
	{

		for (auto& col : ownColliders_)
		{
			col.second->Draw();
		}
	}

	DrawFormatString(10, 320, 0xffffff, "MGL_HP:%d", hp_);

#endif
}
