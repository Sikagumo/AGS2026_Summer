#pragma once

#include <string>

#include "IGuiComponent.h"
#include "../Shader/ShaderParameters.h" 

class ShaderEditorComponent : public IGuiComponent
{
public:
	/// @brief コンストラクタ
	/// @param _name ImGuiで表示するウィンドウ名
	/// @param _params 編集対象のパラメータ構造体へのポインタ
	ShaderEditorComponent(const std::string& _name, NormalWaveShaderParams* _params);

	/// @brief デストラクタ
	~ShaderEditorComponent(void) override = default;

	/// @brief GUI描画処理
	void DrawUI(void) override;

private:

	// GUIの表示名
	std::string name_;

	// 編集対象のパラメータ構造体へのポインタ
	NormalWaveShaderParams* params_;
};