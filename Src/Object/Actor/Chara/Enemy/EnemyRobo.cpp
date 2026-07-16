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
#include "EnemyRobo.h"

EnemyRobo::EnemyRobo()
{
}

EnemyRobo::~EnemyRobo()
{
}

void EnemyRobo::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMY_ROBO));
	transform_.Update();
}

void EnemyRobo::InitTransform(void)
{
	transform_.scl = { 2,2,2};
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(180.0f), UtilityMath::AXIS_Y);

	transform_.pos = { 0.0f,100.0f,100.0f };

	transform_.Update();
}

void EnemyRobo::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::ENEMYROBO, &transform_, COL_LINE_START_POS, COL_LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ENEMYROBO)].push_back(colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMYROBO, &transform_, COL_CAPSULE_START_POS, COL_CAPSULE_END_POS, COL_CAPSULE_END_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ENEMYROBO)].push_back(colCapsule);
	colCapsule->SetTriger(false);

	VECTOR handPos= MV1GetFramePosition(transform_.modelId, 52);
	VECTOR handLocalPos = VSub(handPos, transform_.pos);

	ColliderSphere* colSphere = new ColliderSphere(ColliderBase::TAG::ENEMY_ATTACK, &transform_, handLocalPos, 20.0f);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ENEMY_ATTACK)].push_back(colSphere);
	colSphere->SetTriger(false);

	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ENEMY_ATTACK, false);
}

void EnemyRobo::InitAnimation(void)
{
	CharaBase::InitAnimation();
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animation_->AddInternal(i, 20.0f);
	}
	animation_->Play(static_cast<int>(ANIM_TYPE::DIR));
}

void EnemyRobo::InitPost(void)
{


	stateChanges_.emplace(static_cast<int>(STATE::IDLE), std::bind(&EnemyRobo::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&EnemyRobo::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::MOVE), std::bind(&EnemyRobo::ChangeStateMove, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&EnemyRobo::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);
}

void EnemyRobo::UpdateProcess(void)
{
	stateUpdate_();
}

void EnemyRobo::UpdateProcessPost(void)
{
}

void EnemyRobo::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);


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

	DrawFormatString(10, 200, 0xffffff, "enemyの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
#endif

}

void EnemyRobo::ChangeState(STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	EnemyRobo::ChangeState(state);
}

void EnemyRobo::ChangeState(int state)
{

	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void EnemyRobo::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateIdle, this);
}

void EnemyRobo::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateAttack, this);
	float time = TimeManager::GetInstance().GetGameTime();
	if ((static_cast<int>(time) % 2) == 0)
	{
		animation_->Play(static_cast<int>(ANIM_TYPE::ATTACKA), false);
	}
	else
	{
		animation_->Play(static_cast<int>(ANIM_TYPE::ATTACKB), false);
	}
	

}

void EnemyRobo::ChangeStateMove(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateStateMove, this);
	animation_->Play(static_cast<int>(ANIM_TYPE::WARK), false);

}

void EnemyRobo::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateEnd, this);
}

void EnemyRobo::UpdateIdle(void)
{
	
}

void EnemyRobo::UpdateAttack(void)
{


	VECTOR handPos = MV1GetFramePosition(transform_.modelId, 52);
	VECTOR handLocalPos = VSub(handPos, transform_.pos);

	

	if (animation_->IsEnd()==true)
	{
		ChangeState(STATE::IDLE);
	}
}

void EnemyRobo::UpdateStateMove(void)
{
	LockPlayer();
}

void EnemyRobo::UpdateEnd(void)
{
}

void EnemyRobo::LockPlayer(void)
{
	
	VECTOR moveDir = VSub(playerPos_, transform_.pos);
	moveDir.y = 0.0f;
	moveDir = VNorm(moveDir);
	float targetAngle = atan2(moveDir.x, moveDir.z);
	transform_.quaRot = Quaternion::AngleAxis(targetAngle, UtilityMath::AXIS_Y);
}
