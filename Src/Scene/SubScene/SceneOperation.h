#pragma once

#include "../SceneBase.h"

/// @brief 操作説明画面シーンクラス
class SceneOperation : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneOperation(void);

	/// @brief デストラクタ
	~SceneOperation(void) override = default;

	/// @brief 読み込み処理
	void Load(void) override;

	/// @brief 読み込み完了処理
	void EndLoad(void) override;

	/// @brief 初期化処理
	void Initialize(void) override;

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 描画処理
	void Draw(void) override;

	/// @brief 解放処理
	void Release(void) override;

protected:

	/// @brief Gui用の更新処理
	void UpdateGui(void) override {}

private:

	// 画像ハンドル関連
	int operationImageHandle_;                           // 操作説明画像ハンドル
	int backGroundHandle_;                               // 背景UI背景画像ハンドル

};