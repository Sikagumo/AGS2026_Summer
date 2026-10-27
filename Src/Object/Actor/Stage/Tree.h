#pragma once
#include "../../Actor/ActorBase.h"

class Tree : public ActorBase
{
public:

	Tree(void);

	~Tree(void)override = default;

	void Load(void)override;

	void Update(void)override {};


protected:

	/// @brief 大きさ、回転、座標を初期化する
	void InitTransform(void) override {};

	/// @brief 衝突判定を初期化する
	void InitCollider(void) override;

	/// @brief アニメーションを初期化する
	void InitAnimation(void) override {};

	/// @brief 初期化後の個別処理を行う
	void InitPost(void) override {};


private:

	// 木の位置
	static constexpr float COLLISION_OFFSET_Y = 250.0f;

	// 木モデルの当たり判定の半径
	static constexpr float COLLISION_RADIUS = 27.5f;
};