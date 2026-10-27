#pragma once
#include "../../Actor/ActorBase.h"

class Tree : public ActorBase
{
public:

	/// @brief コンストラクタ
	Tree(void);

	/// @brief デストラクタ
	~Tree(void)override;

	/// @brief リソース読み込み処理
	void Load(void)override;

	/// @brief 更新処理
	void Update(void)override {};


protected:

	/// @brief 大きさ、回転、座標の初期化
	void InitTransform(void) override {};

	/// @brief 衝突判定の初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override {};

	/// @brief 初期化後の個別処理
	void InitPost(void) override {};


private:

	// 木の位置
	static constexpr float COLLISION_OFFSET_Y = 250.0f;

	// 木モデルの当たり判定の半径
	static constexpr float COLLISION_RADIUS = 27.5f;
};