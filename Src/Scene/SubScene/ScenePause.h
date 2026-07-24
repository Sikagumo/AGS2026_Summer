#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../SceneBase.h"

class Collider2DCircle;
class Collider2DBox;

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
	static constexpr int MARGIN_SIZE = 50;                     // 端からの余白
	static constexpr int EXPAND_INTERVAL = 30;                 // 広がるまでにかかるフレーム数
	static constexpr int MENU_TOP_OFFSET = MARGIN_SIZE + 100;  // メニュー上部の余白
	static constexpr int MENU_LEFT_OFFSET = MARGIN_SIZE + 300; // メニュー左部の余白
	static constexpr int MENU_ITEM_HEIGHT = 40;                // メニュー項目の縦幅
	static constexpr float BUTTON_WIDTH = 300.0f;              // ボタンの当たり判定の横幅
	static constexpr float BUTTON_HEIGHT = 35.0f;              // ボタンの当たり判定の縦幅
	static constexpr float YES_NO_BUTTON_WIDTH = 80.0f;        // YES/NOボタンの当たり判定の横幅
	static constexpr float YES_NO_BUTTON_HEIGHT = 40.0f;       // YES/NOボタンの当たり判定の縦幅

	enum class PHASE
	{
		APPEAR,
		NORMAL,
		DISAPPEAR,
		YES_NO
	};

	enum class MENU_ITEM
	{
		RETURN_GAME,   // ゲームに戻る
		RETURN_TITLE,  // タイトルに戻る
		COUNT
	};

	// 状態関連
	PHASE currentPhase_; // 現在のフェーズ
	int frame_;          // 経過フレーム
	int selectedIndex_;  // 選択中のインデックス
	bool isYes_;         // YESが選択されているか

	// メニュー関連
	std::string yesNoTitle_;              // ダイアログのタイトル
	std::vector<std::string> menuItems_;  // メニューの項目
	std::vector<std::string> yesNoItems_; // YES/NOダイアログの項目

	// 当たり判定関連
	std::unique_ptr<Collider2DCircle> cursorCollider_;             // マウスカーソル用
	std::unique_ptr<Collider2DBox> returnGameButtonCollider_;      // ゲームに戻るボタン用
	std::unique_ptr<Collider2DBox> returnTitleButtonCollider_;     // タイトルに戻るボタン用
	std::unique_ptr<Collider2DBox> yesButtonCollider_;             // 「はい」ボタン用
	std::unique_ptr<Collider2DBox> noButtonCollider_;              // 「いいえ」ボタン用

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

	/// @brief 出現時の描画処理
	void DrawAppear(void);

	/// @brief 通常時の描画処理
	void DrawNormal(void);

	/// @brief YES/NO選択時の描画処理
	void DrawYesNo(void);

	/// @brief 背景枠の描画処理
	/// @param rate 枠の広がり具合(0.0f～1.0f)
	void DrawFrame(float rate);

	/// @brief メニュー項目の描画処理
	void DrawMenu(void);

};