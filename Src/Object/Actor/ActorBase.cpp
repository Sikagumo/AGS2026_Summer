#include "ActorBase.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../Collider/ColliderBase.h"

ActorBase::ActorBase(void)
	: timeManager_(TimeManager::GetInstance())
	, transform_(Transform())
{
}

void ActorBase::Init(void)
{
	// Transform初期化
	InitTransform();

	// 衝突判定の初期化an
	InitCollider();

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();

}

void ActorBase::Draw(void)
{
	// 前描画
	DrawPre();

	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}

	// 後描画
	DrawLate();

	//if (sceneMng_.GetIsDebugMode())
	//{
	//	// 所有しているコライダの描画
	//	for (const auto& [type, collider] : ownColliders_)
	//	{
	//		collider->Draw();
	//	}
	//}
}

void ActorBase::Release(void)
{
	ReleasePost();

	transform_.Release();

	CollisionController::GetInstance().UnregisterActor(this);

	// 自身のコライダーの解放
	for (auto& [key, colliders] : ownColliders_)
	{
		for (auto* collider : colliders)
		{
			delete collider;
		}
		colliders.clear();
	}
	ownColliders_.clear();
}

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}

Transform& ActorBase::GetTransform(void)
{
	return transform_;
}

const std::vector<ColliderBase*>* ActorBase::GetOwnCollider(int _key) const
{
	auto it = ownColliders_.find(_key);
	if (it == ownColliders_.end())
	{
		return nullptr;
	}

	return &it->second;
}

void ActorBase::AddHitCollider(const ColliderBase* hitCollider)
{
	for (const auto& c : hitColliders_)
	{
		// 衝突相手の登録
		if (c == hitCollider) { return; }
	}
	hitColliders_.emplace_back(hitCollider);
}
void ActorBase::ClearHitCollider(void)
{
	hitColliders_.clear();
}
