#include <DxLib.h>
#include "../../../../../../Manager/Generic/ResourceManager.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../Collider/ColliderCapsule.h"
#include "../../../../../Collider/ColliderLine.h"
#include "../../../../../Collision/CollisionController.h"
#include "WeaponRG.h"

WeaponRG::WeaponRG(void)
	: WeaponBase()
	, ChargeCount_(0)
	, isAttack_(false)
{
		
}

void WeaponRG::ReleasePost(void)
{
}

void WeaponRG::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos)
{
	bone_.id = _id;
	bone_.transform = _trans;
	bone_.playerPos = _playerPos;
	tag_ = _tag;
}

const VECTOR WeaponRG::GetPos(void) const
{
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = transform_.quaRot.PosAxis(localPos_);

	// 位置を加算して最終的なワールド座標にする
	return VAdd(transform_.pos, localRotPos);
}

void WeaponRG::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_BOSS_WEAPON_RG));
}


void WeaponRG::InitTransform(void)
{
	transform_.scl = WEAPON_SIZE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(WEAPON_ROT), UtilityMath::AXIS_Y));
	transform_.quaRotLocal =
		Quaternion::Mult(transform_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(90.0f), UtilityMath::AXIS_X));

	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
	transform_.Update();
}

void WeaponRG::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &transform_, LINE_START_POS, LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::STAGE)].push_back(colLine);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_, CAPSULE_START_POS, CAPSULE_END_POS, CAPSULE_RADIUS);
	ownColliders_[static_cast<int>(tag_)].push_back(colCapsule);
	colCapsule->SetTriger(false);

	CollisionController::GetInstance().RegisterActor(this);
}

void WeaponRG::InitAnimation(void)
{
}

void WeaponRG::InitPost(void)
{
	isAlive_ = true;
	stateChanges_.emplace(static_cast<int>(STATE::IDLE), std::bind(&WeaponRG::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&WeaponRG::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&WeaponRG::ChangeStateEnd, this));
	stateChanges_.emplace(static_cast<int>(STATE::PREPARATION), std::bind(&WeaponRG::ChangePreparation, this));

	ChangeState(STATE::IDLE);

	localPos_ = LINE_START_POS;
}

void WeaponRG::UpdateProcess(void)
{
	
	if (hp_ <= 0 && isAlive_)
	{
		ChangeState(static_cast<int>(STATE::END));
	}

	stateUpdate_();

}

void WeaponRG::UpdateProcessPost(void)
{
}



void WeaponRG::DrawPre(void)
{
	if (isAlive_)
	{

	}

#ifdef _DEBUG
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
#endif


}

void WeaponRG::LookPlayer(void)
{
	transform_.quaRot = bone_.transform.quaRot;
}

void WeaponRG::ChangeState(STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	ChangeState(state);
}


void WeaponRG::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void WeaponRG::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&WeaponRG::UpdateIdle, this);
	isAttack_ = false;
	ChargeCount_ = 0;
	localUpRot_ = 0.0f;
}

void WeaponRG::ChangePreparation(void)
{
	stateUpdate_ = std::bind(&WeaponRG::UpdatePreparation, this);

}


void WeaponRG::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&WeaponRG::UpdateAttack, this);
}

void WeaponRG::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&WeaponRG::UpdateEnd, this);
	isAlive_ = false;
	CollisionController::GetInstance().SetCollisionActive(this, tag_, false);
}

void WeaponRG::UpdatePreparation(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);

	if (localUpRot_ < MAX_UP_ROT)
	{
		localUpRot_ += UP_ROT;
		if (localUpRot_ > MAX_UP_ROT)
		{
			localUpRot_ = MAX_UP_ROT;
		}
	}
	else
	{
		ChargeCount_++;
		if (ChargeCount_ >= MAX_CHARGE_COUNT)
		{
			isAttack_ = true;
			ChangeState(static_cast<int>(STATE::ATTACK));
		}
	}

	transform_.quaRot = Quaternion::Mult(bone_.transform.quaRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(localUpRot_), UtilityMath::AXIS_X));

}

void WeaponRG::UpdateAttack(void)
{
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);

	transform_.quaRot = Quaternion::Mult(transform_.quaRot,bone_.transform.quaRot);
}

void WeaponRG::UpdateIdle(void)
{
	LookPlayer();
	transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
}

void WeaponRG::UpdateEnd(void)
{
}
