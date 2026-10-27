#include "Tree.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Object/Collision/CollisionController.h"
#include "../../Collider/ColliderCapsule.h"

Tree::Tree(void)
	: ActorBase::ActorBase()
{
}

Tree::~Tree(void)
{
	// 解放処理
	Release();
}

void Tree::Load(void)
{
	// リソース読み込み
	transform_.modelId = ResourceManager::GetInstance().
		LoadModelDuplicate(ResourceManager::SRC::MODEL_TREE);
}

void Tree::InitCollider(void)
{
	// 木のコライダ割り当て
	VECTOR posEnd = UtilityMath::VECTOR_ZERO;
	posEnd.y = COLLISION_OFFSET_Y;
	ColliderCapsule* treeCol = new ColliderCapsule(ColliderBase::TAG::TREE, &transform_, UtilityMath::VECTOR_ZERO, posEnd, COLLISION_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::TREE)].push_back(treeCol);

	// 当たり判定登録
	treeCol->SetTriger(false);
	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::TREE, true);
}