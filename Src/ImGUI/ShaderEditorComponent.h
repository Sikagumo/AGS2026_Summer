#pragma once

#include <string>

#include "IGuiComponent.h"
#include "../Shader/ShaderMaterial.h"

class ShaderEditorComponent : public IGuiComponent
{
public:
	/// @brief コンストラクタ
	/// @param _name ImGuiで表示するウィンドウ名
	/// @param _material 編集対象のマテリアル（ファサード）へのポインタ
	ShaderEditorComponent(const std::string& _name, ShaderMaterial* _material);

	/// @brief デストラクタ
	~ShaderEditorComponent(void) override = default;

	/// @brief GUI描画処理
	void DrawUI(void) override;

private:

	// GUIの表示名
	std::string name_;

	// 編集対象のデータへのポインタ
	ShaderMaterial* material_;
};

