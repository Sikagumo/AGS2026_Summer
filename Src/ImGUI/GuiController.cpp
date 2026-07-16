#include "GuiController.h"

// 静的メンバ変数の初期化
GuiController* GuiController::instance_ = nullptr;

void GuiController::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new GuiController();
	}
}

GuiController& GuiController::GetInstance(void)
{
	return *instance_;
}

void GuiController::DestroyInstance(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void GuiController::SetActiveGui(std::shared_ptr<IGuiComponent> _gui)
{
	// 渡されたコンポーネントをアクティブ（表示対象）にする
	activeGui_ = _gui;
}

void GuiController::ClearGui(void)
{
	// ポインタを空にして表示を消す
	activeGui_ = nullptr;
}

void GuiController::DrawUI(void)
{
	// アクティブなGUIがセットされていれば、その描画処理を呼ぶ
	if (activeGui_ != nullptr)
	{
		activeGui_->DrawUI();
	}
}