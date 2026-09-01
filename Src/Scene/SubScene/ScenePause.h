#pragma once

#include <vector>
#include <string>
#include <memory>
#include <array>

#include "../SceneBase.h"
#include "../../Application.h"

class Collider2DCircle;
class Collider2DBox;

/// @brief ポーズ画面シーンクラス
class ScenePause : public SceneBase
{
public:

	/// @brief コンストラクタ
	ScenePause(void);

	/// @brief デストラクタ
	~ScenePause(void) override;

	/// @brief ロード処理
	void Load(void) override;

	/// @brief ロード終了処理
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

	/// @brief GUIの更新処理
	void UpdateGui(void) override {}

private:

	// 定数関連
	static constexpr int EXPAND_INTERVAL = 30;                                        // 広がるまでにかかるフレーム数
	static constexpr int MENU_TOP_OFFSET = Application::SCREEN_HALF_Y - 50;           // メニュー上部の余白
	static constexpr int MENU_ITEM_HEIGHT = 100;                                      // メニュー項目の縦幅
	static constexpr float BUTTON_WIDTH = 350.0f;                                     // ボタンの当たり判定の横幅
	static constexpr float BUTTON_HEIGHT = 60.0f;                                     // ボタンの当たり判定の縦幅
	static constexpr float YES_NO_BUTTON_WIDTH = 180.0f;                              // YES/NOボタンの当たり判定の横幅
	static constexpr float YES_NO_BUTTON_HEIGHT = 35.0f;                              // YES/NOボタンの当たり判定の縦幅
	static constexpr int STICK_INTERVAL = 15;                                         // スティック入力のインターバル

	// ポーズ画面の進行フェーズ関連
	enum class PHASE
	{
		APPEAR,    // 出現中フェーズ
		NORMAL,    // 通常操作フェーズ
		DISAPPEAR, // 消滅中フェーズ
		YES_NO     // 確認ダイアログ
	};

	// メニュー項目の種類関連
	enum class MENU_ITEM
	{
		RETURN_GAME,   // ゲームに戻る
		RETURN_TITLE,  // タイトルに戻る
		COUNT          // 項目数
	};

	// UI文字画像の識別列挙型
	enum class PAUSE_TEXT_UI
	{
		RETURN_GAME,   // ゲームに戻る
		RETURN_TITLE,  // タイトルに戻る
		YES,           // はい
		NO,            // いいえ
		MAX
	};

	// 状態関連
	PHASE currentPhase_;       // 現在のフェーズ
	int frame_;                // 経過フレーム
	int selectedIndex_;        // 選択中のインデックス
	bool isYes_;               // YESが選択されているか

	// 入力関連
	int inputIntervalCounter_; // スティックの入力間隔カウンター
	bool isPhaseChanged_;      // フェーズ変更直後の入力無効化フラグ

	// 画像ハンドル関連
	std::array<int, static_cast<size_t>(PAUSE_TEXT_UI::MAX)> selectTextHandles_;     // 選択中文字画像配列
	std::array<int, static_cast<size_t>(PAUSE_TEXT_UI::MAX)> noSelectTextHandles_;   // 非選択中文字画像配列 
	int backGroundHandle_;                                                           // 背景画像ハンドル

	// 当たり判定関連
	std::unique_ptr<Collider2DCircle> cursorCollider_;         // マウスカーソル用コライダー
	std::unique_ptr<Collider2DBox> returnGameButtonCollider_;  // ゲームに戻るボタン用コライダー
	std::unique_ptr<Collider2DBox> returnTitleButtonCollider_; // タイトルに戻るボタン用コライダー
	std::unique_ptr<Collider2DBox> yesButtonCollider_;         // 「はい」ボタン用コライダー
	std::unique_ptr<Collider2DBox> noButtonCollider_;          // 「いいえ」ボタン用コライダー

	// 更新処理関数関連
	/// @brief 出現時の更新処理
	void ProcessAppear(void);

	/// @brief 通常時の更新処理
	void ProcessNormal(void);

	/// @brief 消滅時の更新処理
	void ProcessDisappear(void);

	/// @brief YES/NO選択時の更新処理
	void ProcessYesNo(void);

	/// @brief メニューの状態に応じた処理
	void ProcessMenuState(void);

	/// @brief YESを選択した時の処理
	void ExecuteYesAction(void);

	// 描画処理関数関連
	/// @brief 出現時の描画処理
	void DrawAppear(void);

	/// @brief 通常時の描画処理
	void DrawNormal(void);

	/// @brief YES/NO選択時の描画処理
	void DrawYesNo(void);

	/// @brief 背景枠の描画処理
	/// @param _rate 枠の広がり具合
	void DrawFrame(float _rate);

	/// @brief メニュー項目の描画処理
	void DrawMenu(void);

	/// @brief 文字画像のハンドルを取得する
	/// @param _textType 文字画像種別
	/// @param _isSelected 選択中かどうか
	/// @return 画像ハンドル
	int GetTextUIHandle(PAUSE_TEXT_UI _textType, bool _isSelected) const;
};