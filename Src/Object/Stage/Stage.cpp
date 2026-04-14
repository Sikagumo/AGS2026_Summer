#include "Stage.h"
#include "../Actor/ActorBase.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"
#include "../Collider/ColliderModel.h"


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

void Stage::InitLoad(void)
{
	
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
	for (const std::string& name : EXCLUDE_FRAME_NAMES)
	{
		colModel->AddExcludeFrameIds(name);
	}
	for (const std::string& name : TARGET_FRAME_NAMES)
	{
		colModel->AddTargetFrameIds(name);
	}
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}


void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
}
