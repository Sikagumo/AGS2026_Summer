#pragma once

#include <memory>
#include "IGuiComponent.h" 

/// @brief アクティブなGUIコンポーネントを画面に表示するだけの管理者クラス
class GuiController
{
public:
	/// @brief インスタンスの生成
	static void CreateInstance(void);

	/// @brief インスタンスの取得
	/// @return GuiControllerの参照
	static GuiController& GetInstance(void);

	/// @brief インスタンスの破棄
	static void DestroyInstance(void);

	/// @brief 画面に表示するGUIをセットする
	/// @param _gui 表示したいGUIコンポーネントの共有ポインタ
	void SetActiveGui(std::shared_ptr<IGuiComponent> _gui);

	/// @brief 現在表示中のGUIを閉じる
	void ClearGui(void);

	/// @brief アクティブなGUIがセットされていれば描画する
	void DrawUI(void);

private:

	// シングルトンインスタンス
	static GuiController* instance_;           

	// 現在アクティブなGUIコンポーネント
	std::shared_ptr<IGuiComponent> activeGui_; 

	/// @brief コンストラクタ
	GuiController(void) = default;

	/// @brief デストラクタ
	~GuiController(void) = default;

	/// @brief コピー禁止
	GuiController(const GuiController&) = delete;

	/// @brief 代入禁止 
	GuiController& operator=(const GuiController&) = delete;
};