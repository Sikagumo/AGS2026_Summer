#pragma once

#include <array>

#include "../SceneBase.h"

/// @brief リザルト画面シーンクラス
class SceneResult : public SceneBase
{
public:

	/// @brief デフォルトコンストラクタ
	SceneResult(void) = default;

	/// @brief コンストラクタ
	/// @param _isGameOver ゲームオーバーにするか否か
	SceneResult(bool _isGameOver);

	/// @brief デストラクタ
	~SceneResult(void) override = default;

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

	// 画像ハンドル関連
	std::array<int, 4> imageResult_; // リザルト用テキスト等の画像ハンドル配列
	int img_;                        // 背景または結果表示の画像ハンドル

	// ゲームオーバーかどうか
	bool isGameOver_;                

	/// @brief Gui用の更新処理
	void UpdateGui(void) override;
};