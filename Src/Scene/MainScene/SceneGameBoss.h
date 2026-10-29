#pragma once

#include <memory>

#include "../SceneBase.h"

class SceneManager;
class Boss;

/// @brief ゲーム本編のメインロジックを管理するシーンクラス（ボス戦）
class SceneGameBoss : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneGameBoss(void);

	/// @brief デストラクタ
	~SceneGameBoss(void) override;

	/// @brief リソースの読み込み開始
	void Load(void) override;

	/// @brief リソースの読み込み完了処理
	void EndLoad(void) override;

	/// @brief 初期化処理
	void Initialize(void) override;

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 描画処理
	void Draw(void) override;

	/// @brief 解放処理
	void Release(void) override;

private:

	// シーンマネージャーの参照
	SceneManager& sceneManager_;

	// ボスオブジェクト
	std::unique_ptr<Boss> boss_; 

	/// @brief デバッグ情報の描画
	void DrawDebug(void);
};