#include "Stage.h"
#include "../../Actor/ActorBase.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Object/Manager/CollisionManager.h"
#include "../../../Utility/UtilityMath.h"
#include "../../Collider/ColliderModel.h"


Stage::Stage(void)
{
}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{
	ActorBase::Draw();
}

void Stage::Load(void)
{
	transform_.modelId = resourceManager_.LoadHandleId(ResourceManager::SRC::MODEL_STAGE);
}

void Stage::InitTransform(void)
{
	transform_.InitTransform(1.0f,
							 Quaternion::Identity(), Quaternion::Identity(),
							 { 0.0f, -100.0f, 0.0f });
}

void Stage::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ割り当て
	ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &transform_);
	ownColliders_.emplace(static_cast<int>(ColliderBase::SHAPE::MODEL), colModel);
	colModel->SetTriger(false);

	CollisionManager::GetInstance().RegisterActor(this);
}


void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
}
