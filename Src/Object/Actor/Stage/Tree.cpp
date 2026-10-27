#include "Tree.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Object/Collision/CollisionController.h"
#include "../../Collider/ColliderCapsule.h"

Tree::Tree(void)
	: ActorBase::ActorBase()
{
}

void Tree::Load(void)
{
	transform_.modelId = ResourceManager::GetInstance().
		LoadModelDuplicate(ResourceManager::SRC::MODEL_TREE);
}

void Tree::InitCollider(void)
{
	// –Ø‚ÌƒRƒ‰ƒCƒ_Š„‚è“–‚Ä
	VECTOR posEnd = UtilityMath::VECTOR_ZERO;
	posEnd.y = COLLISION_OFFSET_Y;
	ColliderCapsule* treeCol = new ColliderCapsule(ColliderBase::TAG::TREE, &transform_, UtilityMath::VECTOR_ZERO, posEnd, COLLISION_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::TREE)].push_back(treeCol);

	// “–‚½‚è”»’è“o˜^
	treeCol->SetTriger(false);
	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance()
		.SetCollisionActive(this, ColliderBase::TAG::TREE, true);
}