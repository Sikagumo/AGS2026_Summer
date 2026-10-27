#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../../Actor/ActorBase.h"
#include "../../../Shader/ShaderParameters.h"
#include "./Tree.h"

class Tree;

class Stage : public ActorBase
{
public:

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		MODEL = 0,
		MAX,
	};

	Stage(void);

	~Stage(void)override = default;

	void Load(void)override;

	void Update(void)override;

	// 描画処理
	void Draw(void)override;
	void DrawTree(void);


protected:

	/// @brief 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	/// @brief 衝突判定の初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override {};

	/// @brief 初期化後の個別処理
	void InitPost(void) override;


private:

	// ステージのテクスチャ
	static constexpr float STAGE_TEXTURE_SCALE = 50.0f;

	// 木のスケール
	static constexpr float TREE_SCALE = 1.25f;

	// 木の縦位置
	static constexpr float TREE_POS_Y = -25.0f;

	// 壁の除外フレーム名称
	const std::vector<std::string> EXCLUDE_WALL_NAMES =
	{
		"Collision_Ground",
	};

	// ステージの除外フレーム名称
	const std::vector<std::string> EXCLUDE_STAGE_NAMES =
	{
		"Collision_Wall",
	};

	// 木の配置するフレーム名
	const std::string POS_FRAME_NAME_FRONT = "tree_In";
	const std::string POS_FRAME_NAME_BACK  = "tree_Out";

	// スカイドーム
	Transform skyDome_;

	// 描画用モデル
	Transform viewTrans_;

	// 描画用モデルのテクスチャ
	int viewStageTexHandle_;
	TextureScaleShaderParams texScaleParams_;

	// 木のモデル
	Transform treePosModel_;
	std::vector<std::shared_ptr<Tree>> treesFront_;
	std::vector<Transform> treesBack_;
};