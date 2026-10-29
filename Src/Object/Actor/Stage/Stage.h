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

	/// @brief コンストラクタ
	Stage(void);

	/// @brief デストラクタ
	~Stage(void)override = default;

	/// @brief リソース読み込み処理
	void Load(void)override;

	/// @brief 更新処理
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

	/// @brief 各オブジェクト用解放処理
	void ReleasePost(void)override;


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

	// 木オブジェクトの配置するフレーム名(当たり判定あり)
	const std::string POS_FRAME_NAME_FRONT = "tree_In";

	// 木オブジェクトの配置するフレーム名(描画のみ)
	const std::string POS_FRAME_NAME_BACK  = "tree_Out";

	// スカイドーム
	Transform skyDome_;

	// 描画用モデル
	Transform viewTrans_;

	// ステージモデルのテクスチャハンドル
	int viewStageTexHandle_;

	// ステージモデルのテクスチャシェーダのパラメータ
	TextureScaleShaderParams texScaleParams_;

	// 木のモデル
	Transform treePosModel_;

	// 木オブジェクト(当たり判定あり)
	std::vector<std::unique_ptr<Tree>> treesFront_;

	// 木オブジェクト(描画のみ)
	std::vector<Transform> treesBack_;
};