#include "ActorBase.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../Collider/ColliderBase.h"

ActorBase::ActorBase(void)
	: 
	resMng_(ResourceManager::GetInstance()),
	sceneMng_(SceneManager::GetInstance()),
	transform_(Transform())
{
}

ActorBase::~ActorBase(void)
{
}

void ActorBase::Init(void)
{
	// リソースロード
	InitLoad();

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

	if (sceneMng_.GetIsDebugMode())
	{
		// 所有しているコライダの描画
		for (const auto& [type, collider] : ownColliders_)
		{
			collider->Draw();
		}
	}
}

void ActorBase::Release(void)
{
	transform_.Release();

	// 自身のコライダ解放
	for (auto& own : ownColliders_)
	{
		delete own.second;
	}
}

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}

const ColliderBase* ActorBase::GetOwnCollider(int key) const
{
	if (ownColliders_.count(key) == 0)
	{
		return nullptr;
	}
	return ownColliders_.at(key);
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
