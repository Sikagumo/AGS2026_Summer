#include "Stage.h"
#include "../../Actor/ActorBase.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Object/Collision/CollisionController.h"
#include "../../../Utility/UtilityMath.h"
#include "../../Collider/ColliderModel.h"


Stage::Stage(void)
{
}

void Stage::Load(void)
{
	transform_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_STAGE_COLLISION);
	viewTrans_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_STAGE);
	skyDome_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_SKYDOME);

	treePosHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_TREE_POSITION);

	
	int frameFront = MV1SearchFrame(treePosHandle_, POS_FRAME_NAME_FRONT.c_str());
	int maxFront = MV1GetFrameChildNum(treePosHandle_, frameFront);

	for (int i = 0; i < maxFront; i++)
	{
		std::unique_ptr<Transform> tree = std::make_unique<Transform>();
		tree->modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_TREE);

		treesFront_.emplace_back(std::move(tree));
	}


	int frameBack = MV1SearchFrame(treePosHandle_, POS_FRAME_NAME_BACK.c_str());
	int maxBack = MV1GetFrameChildNum(treePosHandle_, frameBack);
	for (int i = 0; i < maxBack; i++)
	{
		std::unique_ptr<Transform> tree = std::make_unique<Transform>();
		tree->modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_TREE);

		treesBack_.emplace_back(std::move(tree));
	}
}

void Stage::InitTransform(void)
{
	constexpr float SCALE = 0.425f;
	constexpr VECTOR LOCAL_POS = { 0.0f, 10.0f, 0.0f };
	VECTOR localPos = LOCAL_POS;

	viewTrans_.InitTransform(SCALE,
		Quaternion::Identity(), Quaternion::Identity(),
		localPos);

	constexpr float COLLISION_POS_Y = -117.5f;
	localPos.y += COLLISION_POS_Y;
	transform_.InitTransform(SCALE,
		Quaternion::Identity(), Quaternion::Identity(),
		localPos);
	
	constexpr float SKYDOME_SCALE = 150.0f;
	skyDome_.InitTransform(SKYDOME_SCALE,
		Quaternion::Identity(), Quaternion::Identity());


	constexpr float TREE_SCALE = 1.25f;

	int frameFront = MV1SearchFrame(treePosHandle_, POS_FRAME_NAME_FRONT.c_str());
	int maxFront = MV1GetFrameChildNum(treePosHandle_, frameFront);
	for (int i = 0; i < maxFront; i++)
	{
		float rot = static_cast<float>(360 - GetRand(360 * 2));
		VECTOR pos = MV1GetFramePosition(treePosHandle_, (frameFront + (i + 1)));

		treesFront_.at(i)->InitTransform(TREE_SCALE
			, Quaternion::Identity(), Quaternion::AngleAxis(UtilityMath::Deg2RadF(rot), UtilityMath::AXIS_Y)
			, pos);
	}

	int frameBack = MV1SearchFrame(treePosHandle_, POS_FRAME_NAME_BACK.c_str());
	int maxBack = MV1GetFrameChildNum(treePosHandle_, frameBack);
	for (int i = 0; i < maxBack; i++)
	{
		float rot = static_cast<float>(360 - GetRand(360 * 2));
		VECTOR pos = MV1GetFramePosition(treePosHandle_, (frameBack + (i + 1)));

		treesBack_.at(i)->InitTransform(TREE_SCALE
			, Quaternion::Identity(), Quaternion::AngleAxis(UtilityMath::Deg2RadF(rot), UtilityMath::AXIS_Y)
			, pos);
	}
}

void Stage::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ割り当て
	ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &transform_);
	ownColliders_[static_cast<int>(ColliderBase::TAG::STAGE)].push_back(colModel);

	colModel->SetTriger(true);

	for (const auto& excludeName : EXCLUDE_STAGE_NAMES)
	{
		colModel->AddExcludeFrameIds(excludeName);
	}


	ColliderModel* wallCollider = new ColliderModel(ColliderBase::TAG::WALL, &transform_);

	// 壁のコライダ割り当て
	ownColliders_[static_cast<int>(ColliderBase::TAG::WALL)].push_back(wallCollider);
	wallCollider->SetTriger(false);

	for (const auto& excludeName : EXCLUDE_WALL_NAMES)
	{
		wallCollider->AddExcludeFrameIds(excludeName);
	}

	CollisionController::GetInstance().RegisterActor(this);
}


void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
}

void Stage::Update(void)
{
	skyDome_.Rotate(UtilityMath::AXIS_Y, 0.0025f);
}

void Stage::Draw(void)
{
	MV1DrawModel(skyDome_.modelId);

	MV1DrawModel(viewTrans_.modelId);

	for (auto& treeFront : treesFront_)
	{
		MV1DrawModel(treeFront->modelId);
	}

	for (auto& treeBack : treesBack_)
	{
		MV1DrawModel(treeBack->modelId);
	}

#ifdef _DEBUG
	ActorBase::Draw();

	// 以前の MV1DrawModel(collisionTrans_.modelId); はこれと被るので消すかコメントアウト
	// // 自分が持っているすべてのコライダーを描画する
	for (const auto& [tagId, colliderList] : ownColliders_)
	{
		for (auto* collider : colliderList)
		{
			if (collider != nullptr)
			{
				// 例として緑色を指定（タグごとに色を変えてもOKです）
				collider->Draw();
			}
		}
	}

	DrawFormatString(10, 120, 0xffffff, "ステージの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
#endif
}