#pragma once

#include <array>

#include "../SceneBase.h"

class SceneResult : public SceneBase
{
public:

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

	std::array<int, 4> imageResult_;

	int img_;

	bool isGameOver_;

	/// @brief Gui用の更新処理
	void UpdateGui(void) override;

};

