#pragma once
#include <string>

#include "IGuiComponent.h"
#include "../Shader/ShaderParameters.h" 

/// @brief シェーダーのパラメータをGUI上で編集するコンポーネントクラス
class ShaderEditorComponent : public IGuiComponent
{
public:
	/// @brief コンストラクタ
	/// @param _name ImGuiで表示するウィンドウ名
	/// @param _parameters 編集対象のパラメータ構造体へのポインタ
	ShaderEditorComponent(const std::string& _name, NormalWaveShaderParams* _parameters);

	/// @brief デストラクタ
	~ShaderEditorComponent(void) override = default;

	/// @brief GUI描画処理
	void DrawUI(void) override;

private:
	// 情報関連
	std::string name_;                   // GUIの表示名
	NormalWaveShaderParams* parameters_; // 編集対象のパラメータ構造体へのポインタ
};