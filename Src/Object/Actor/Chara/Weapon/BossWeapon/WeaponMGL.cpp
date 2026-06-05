#include <DxLib.h>
#include "../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../Utility/UtilityMath.h"
#include "../../../../Collider/ColliderBase.h"
#include "../../../../Collider/ColliderCapsule.h"
#include "../../../../Collider/ColliderLine.h"
#include "../../../../Manager/CollisionManager.h"
#include "../Bullet/Boss/BBulletMG.h"
#include "WeaponMGL.h"

WeaponMGL::WeaponMGL()
	:bulletDir_{ 0.0f,0.0f,0.0f }
	, bulletCount_(MAX_BULLET_COUNT)
{
	
}

void WeaponMGL::ReleasePost(void)
{
}

void WeaponMGL::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos)
{
	bone_.id = _id;
	bone_.transform = _trans;
	bone_.playerPos = _playerPos;
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
	transform_.quaRot = bone_.transform.quaRot;
	transform_.quaRotLocal=Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y);

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
		bulletCount_--;
		transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
		
		LookPlayer();

		
		CreateBullets();
		
		
		for (std::shared_ptr<BBulletBase> shot : bullets_)
		{
			shot->Update();
		}
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

		for (std::shared_ptr<BBulletBase> shot : bullets_)
		{
			shot->Draw();
		}
	}

	DrawFormatString(10, 320, 0xffffff, "MGL_HP:%d", hp_);
	DrawFormatString(10, 340, 0xffffff, "MGL_BULLET:%d", bullets_.size());
#endif
}

void WeaponMGL::LookPlayer(void)
{

	VECTOR moveDir;

	// プレイヤーの位置に向かう方向を計算
	moveDir = VSub(bone_.playerPos, transform_.pos);

	moveDir = VNorm(moveDir);

	float horizontalDistance = sqrtf(moveDir.z * moveDir.z + moveDir.x * moveDir.x);

	float targetAngle = atan2(moveDir.y,horizontalDistance);

	Quaternion weaponPitch = Quaternion::AngleAxis(-targetAngle, UtilityMath::AXIS_X);

	transform_.quaRot = Quaternion::Mult( bone_.transform.quaRot, weaponPitch);
	bulletDir_ = moveDir;
}

void WeaponMGL::CreateBullets(void)
{
	std::shared_ptr<BBulletBase> bullet = GetValidBullet();

	VECTOR localPos = { -50.0f,10.0f,140.0f };

	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(localPos);

	// 位置を加算して最終的なワールド座標にする
	VECTOR bulletpos = VAdd(transform_.pos, localRotPos);
	bullet->Init();

	bullet->CreateBullets(bulletpos, bulletDir_, 5.0f);
}

std::shared_ptr<BBulletBase> WeaponMGL::GetValidBullet(void)
{
	size_t bulletCount = bullets_.size();
	for (size_t i = 0; i < bulletCount; i++)
	{
		if (!bullets_[i]->GetIsAlive())
		{
			return bullets_[i];
		}
	}


	std::shared_ptr<BBulletBase> bullet = std::make_shared<BBulletMG>(transform_);
	// 可変長配列に追加
	bullets_.push_back(bullet);


	return bullet;
}
