#pragma once

#include <string>

#include "IGuiComponent.h"
#include "../Shader/ShaderParameters.h"

class ShaderEditorComponent : public IGuiComponent
{
public:
	/// @brief コンストラクタ
	/// @param _name ImGuiで表示するウィンドウ（またはパーツ）の名前
	/// @param _targetBuffer 編集対象のパラメータバッファへのポインタ
	ShaderEditorComponent(const std::string& _name, IntegratedGpuBuffer* _targetBuffer);

	/// @brief デストラクタ
	~ShaderEditorComponent(void) override = default;

	/// @brief GUI描画処理
	void DrawUI(void) override;

private:

	// GUIの表示名
	std::string name_;                 

	// 編集対象のデータへのポインタ
	IntegratedGpuBuffer* targetBuffer_;   
};

