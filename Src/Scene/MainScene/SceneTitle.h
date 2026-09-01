#pragma once

#include <array>
#include <memory>

#include "../SceneBase.h"
#include "../../Common/Vector2.h"
#include "../../Object/Collider2D/Collider2DBase.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"
#include "../../ImGUI/ShaderEditorComponent.h"

/// @brief タイトル画面シーンクラス
class SceneTitle : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneTitle(void);

	/// @brief デストラクタ
	~SceneTitle(void) override = default;

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

	// メニュー項目の列挙型
	enum class MENU_ITEM
	{
		SOLO,   // ソロプレイ
		MULTI,  // マルチプレイ
		OPTION, // オプション
		EXIT,   // 終了
		COUNT   // 項目数
	};

	// 定数関連
	static constexpr int MENU_BUTTON_NUM = 4; // メニューボタンの数

	// 画像ハンドル関連
	int imageTitle_;                               // タイトルロゴの画像ハンドル
	std::array<int, 8> imageMenu_;                 // メニュー項目（4つ × 選択/非選択）の画像ハンドル
	std::array<Collider2DBase::TAG_2D, 4> buttonTags; // ボタンのタグ配列
	int peachHandle_;                              // 桃の画像
	int peachNormalHandle_;                        // 桃のノーマルマップ
	int waveHandle_;                               // 波の画像
	int waveNormalHandle_;                         // 波のノーマルマップ
	int oniSimaHandle_;                            // 鬼ヶ島の画像
	int oniSimaNormalHandle_;                      // 鬼ヶ島のノーマルマップ
	int titleNormalHandle_;                        // タイトルのノーマルマップ
	int backgroundHandle_;                         // 背景の画像
	int backNormalHandle_;                         // 背景のノーマルマップ

	// 制御・状態関連
	float time_;                 // 経過時間
	int selectedIdx_;            // 選択中のメニューインデックス
	int inputIntervalCounter_;   // 入力間隔調整用カウンター
	Vector2F prevMousePos_;      // 前回のマウス座標
	bool isSelectMenu_;          // メニュー選択中かどうか

	// GUIコンポーネント関連
	std::shared_ptr<ShaderEditorComponent> peachGui_;   // 桃用GUI
	std::shared_ptr<ShaderEditorComponent> waveGui_;    // 波用GUI
	std::shared_ptr<ShaderEditorComponent> oniSimaGui_; // 鬼ヶ島用GUI
	std::shared_ptr<ShaderEditorComponent> titleGui_;   // タイトル用GUI

	// コライダー関連
	std::unique_ptr<Collider2DCircle> cursorCollider_;     // マウスカーソル用の円コライダー
	std::unique_ptr<Collider2DBox> soloPlayButtonCollider_; // ソロプレイボタン用コライダー
	std::unique_ptr<Collider2DBox> multiPlayButtonCollider_; // マルチプレイボタン用コライダー
	std::unique_ptr<Collider2DBox> optionButtonCollider_;  // 設定ボタン用コライダー
	std::unique_ptr<Collider2DBox> exitButtonCollider_;    // 終了ボタン用コライダー
	std::unique_ptr<Collider2DBox> peachCollider_;         // 桃用コライダー
	std::unique_ptr<Collider2DBox> waveCollider_;          // 波用コライダー
	std::unique_ptr<Collider2DBox> oniSimaCollider_;       // 鬼ヶ島用コライダー

	/// @brief UI初期化処理
	void InitUI(void);

	/// @brief デバッグ描画 
	void DrawDebug(void);

	/// @brief タイトルメニュー別遷移処理
	void ProcessMenuState(void);

	/// @brief Gui用の更新処理
	void UpdateGui(void) override;
};