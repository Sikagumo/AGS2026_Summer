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
	transform_.scl = { 3,3,3 };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(180.0f), UtilityMath::AXIS_Y);

	transform_.pos = { 0.0f,50.0f,0.0f };

	transform_.Update();
}

void EnemyRobo::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::ENEMY, &transform_, COL_LINE_START_POS, COL_LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::BOSS)].push_back(colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_, COL_CAPSULE_START_POS, COL_CAPSULE_END_POS, COL_CAPSULE_END_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::BOSS)].push_back(colCapsule);
	colCapsule->SetTriger(false);
}

void EnemyRobo::InitAnimation(void)
{
}

void EnemyRobo::InitPost(void)
{
}

void EnemyRobo::UpdateProcess(void)
{
}

void EnemyRobo::UpdateProcessPost(void)
{
}

void EnemyRobo::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
}

void EnemyRobo::ChangeState(STATE _state)
{
}

void EnemyRobo::ChangeState(int state)
{
}

void EnemyRobo::ChangeStateIdle(void)
{
}

void EnemyRobo::ChangeStateAttack(void)
{
}

void EnemyRobo::ChangeStateMOVE(void)
{
}

void EnemyRobo::ChangeStateEnd(void)
{
}

void EnemyRobo::UpdateIdle(void)
{
}

void EnemyRobo::UpdateAttack(void)
{
}

void EnemyRobo::UpdateStateMOVE(void)
{
}

void EnemyRobo::UpdateEnd(void)
{
}
