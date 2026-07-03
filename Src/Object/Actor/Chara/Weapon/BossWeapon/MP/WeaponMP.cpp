#include <DxLib.h>
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderSphere.h"
#include "../../../../../Collider/ColliderLine.h"
#include "../../../../../Collision/CollisionController.h"

#include "../../Bullet/Boss/BBulletMissile.h"
#include "WeaponMP.h"

WeaponMP::WeaponMP(void) 
	:isLR_(false)
	,muzzleCount_(0)
	,bulletDir_({ 0.0f,0.0f,1.0f })
	,attackCount_(0)
	, outCount_(0)
{
	
}

void WeaponMP::ReleasePost(void)
{
}

void WeaponMP::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos)
{
	bone_.id = _id;
	bone_.transform = _trans;
	bone_.playerPos = _playerPos;
	tag_ = _tag;
}

const VECTOR WeaponMP::GetPos(void) const
{
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(localPos_);

	// 位置を加算して最終的なワールド座標にする
	return VAdd(transform_.pos, localRotPos);
}

void WeaponMP::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_WEAPON_RK));
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
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::LINE), std::vector<ColliderBase*>{ colLine });


	ColliderSphere* colSphere = new ColliderSphere(
		tag_, &transform_, SPHERE_START_POS, SPHERE_RADIUS);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::SPHERE), std::vector<ColliderBase*>{ colSphere });
	colSphere->SetTriger(false);

	CollisionController::GetInstance().RegisterActor(this);
}

void WeaponMP::InitAnimation(void)
{
}

void WeaponMP::InitPost(void)
{
	isAlive_ = true;
	localPos_ = LINE_START_POS;
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&WeaponMP::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&WeaponMP::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&WeaponMP::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);


	for (int i = 0; i < MUZZLE_MAX_COUNT; ++i)
	{
		muzzlePos_[i] = MUZZLE_POS[i];
	}

}

void WeaponMP::UpdateProcess(void)
{
	// HPがなくなったら死亡処理（左右共通）
	if (hp_ <= 0 && isAlive_)
	{
		ChangeState(static_cast<int>(STATE::END));
	}
	for (auto& bullet : bullets_)
	{
		bullet->SetPlayerPos(bone_.playerPos);
		bullet->Update();

	}


	

	stateUpdate_();
}

void WeaponMP::UpdateProcessPost(void)
{
}



void WeaponMP::DrawPre(void)
{
	for (auto& bullet : bullets_) {

		bullet->Draw();

	}
	if (isAlive_)
	{
		
		for (auto& [id, colliderVector] : ownColliders_)
		{
			for (auto* collider : colliderVector)
			{
				if (collider == nullptr)
				{
					continue;
				}
				collider->Draw();
			}
		}
	}
	
	
}

void WeaponMP::ChangeState(STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	ChangeState(state);
}

void WeaponMP::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void WeaponMP::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&WeaponMP::UpdateIdle, this);
}

void WeaponMP::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&WeaponMP::UpdateAttack, this);
	attackCount_ = 0;
	
}

void WeaponMP::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&WeaponMP::UpdateEnd, this);
	isAlive_ = false;
	CollisionController::GetInstance().SetCollisionActive(this, tag_, false);
}

void WeaponMP::UpdateAttack(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	outCount_++;
	if (outCount_ >= ATTACK_DELAY)
	{
		CreateBullets();
		attackCount_++;
		outCount_ = 0;
	}
	if (attackCount_>=MAX_ATTACK_COUNT)
	{
		ChangeState(STATE::IDLE);
	}

	
	
	
	

}

void WeaponMP::UpdateIdle(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
}

void WeaponMP::UpdateEnd(void)
{
}

void WeaponMP::CreateBullets(void)
{
	std::shared_ptr<BBulletBase> bullet = GetValidBullet();

	// ★ 1. まず現在の muzzleCount_ (0, 1, 2) を使って位置を決定する
	VECTOR localRotPos = transform_.quaRot.PosAxis(muzzlePos_[muzzleCount_]);
	VECTOR bulletpos = VAdd(transform_.pos, localRotPos);

	bullet->CreateBullets(bulletpos, bulletDir_, ATTACK_RADIUS);
	bullet->Init();
	bullet->SetTransform(transform_);
	bullet->SetPlayerPos(bone_.playerPos);

	// ★ 2. 現在の muzzleCount_ (0, 1, 2) を使って高さを計算する
	if (isLR_)
	{
		float maxPos = MIN_FALL_POS + (UP_FALL_POS * (muzzleCount_ * 2));
		bullet->SetUpMaxPos_(maxPos);
	}
	else
	{
		float maxPos = MIN_FALL_POS + (UP_FALL_POS * (muzzleCount_ * 2 + 1));
		bullet->SetUpMaxPos_(maxPos);
	}

	// ★ 3. すべての処理が終わった後に、次の発射のためにカウントを進める
	muzzleCount_++;
	if (muzzleCount_ > MUZZLE_MAX_COUNT / 2 - 1) { // 2 を超えたら 0 に戻す
		muzzleCount_ = 0;
	}
}

std::shared_ptr<BBulletBase> WeaponMP::GetValidBullet(void)
{
	for (auto& bullet : bullets_) {
		if (!bullet->GetIsAlive()) return bullet;
	}
	std::shared_ptr<BBulletBase> bullet = std::make_shared<BBulletMissile>();
	bullets_.emplace_back(bullet);
	bullet->Load();
	return bullet;
}
