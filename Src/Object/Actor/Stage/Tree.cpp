#include "Tree.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Object/Collision/CollisionController.h"
#include "../../Collider/ColliderCapsule.h"


constexpr float TREE_RADIUS = 27.5f;

Tree::Tree(void)
	: ActorBase::ActorBase(), isActive_(true)
{
}

void Tree::Load(void)
{
	transform_.modelId = ResourceManager::GetInstance().
		LoadModelDuplicate(ResourceManager::SRC::MODEL_TREE);
}

void Tree::InitTransform(void)
{
}

void Tree::InitCollider(void)
{
	// 木のコライダ割り当て
	VECTOR posEnd = UtilityMath::VECTOR_ZERO;
	posEnd.y = 250.0f;
	constexpr float TREE_RADIUS = 27.5f;

	ColliderCapsule* treeCol = new ColliderCapsule(ColliderBase::TAG::TREE, &transform_, UtilityMath::VECTOR_ZERO, posEnd, TREE_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::TREE)].push_back(treeCol);

	treeCol->SetTriger(false);

	// 当たり判定登録
	CollisionController::GetInstance().RegisterActor(this);

	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::TREE, true);
}


void Tree::Update(void)
{
	// レーザー衝突時、無効化
	if (CollisionController::GetInstance()
		.IsTagCollidingWithTag(ColliderBase::TAG::TREE, ColliderBase::TAG::LASER))
	{
		CollisionController::GetInstance()
			.SetCollisionActive(this, ColliderBase::TAG::TREE, false);

		isActive_ = false;
	}
}