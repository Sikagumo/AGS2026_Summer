#pragma once

/// @brief すべてのGUIコンポーネントの親インターフェース
class IGuiComponent
{
public:

    /// @brief コンストラクタ 
    IGuiComponent(void) = default;

    /// @brief デストラクタ 
    virtual ~IGuiComponent(void) = default;

    /// @brief ImGuiの描画処理
    virtual void DrawUI(void) = 0;
};