#include "Stage.h"
#include "../../Actor/ActorBase.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Object/Collision/CollisionController.h"
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


#ifdef _DEBUG
	// 以前の MV1DrawModel(collisionTrans_.modelId); はこれと被るので消すかコメントアウト

	// 自分が持っているすべてのコライダーを描画する
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

	bool isHit = CollisionController::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::WALL, ColliderBase::TAG::PLAYER);

	if (isHit)
	{
		DrawString(0, 600, "当たってる", 0x000000);
	}

	DrawFormatString(10, 120, 0xffffff, "ステージの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
#endif
}

void Stage::Load(void)
{
	transform_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_STAGE);
	collisionTrans_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_STAGE_COLLISION);
}

void Stage::InitTransform(void)
{
	//constexpr float SCALE = 5.0f;
	constexpr float SCALE = 0.425f;
	constexpr VECTOR LOCAL_POS = { 0.0f, -50.0f, 0.0f };
	VECTOR localPos = LOCAL_POS;

	transform_.InitTransform(SCALE,
							 Quaternion::Identity(), Quaternion::Identity(),
							 localPos);

	constexpr float COLLISION_POS_Y = -50.0f;
	localPos.y += COLLISION_POS_Y;
	collisionTrans_.InitTransform(SCALE,
								  Quaternion::Identity(), Quaternion::Identity(),
								  localPos);
}

void Stage::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(collisionTrans_.modelId);

	// モデルのコライダ割り当て
	ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &collisionTrans_);
	ownColliders_[static_cast<int>(ColliderBase::TAG::STAGE)].push_back(colModel);

	colModel->SetTriger(false);

	for (const auto& excludeName : EXCLUDE_STAGE_NAMES)
	{
		colModel->AddExcludeFrameIds(excludeName);
	}


	ColliderModel* wallCollider = new ColliderModel(ColliderBase::TAG::WALL, &collisionTrans_);

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
