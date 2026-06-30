#pragma once
class IParameterEditor
{
public:
	/// @brief コンストラクタ
	IParameterEditor(void) = default;

	/// @brief デストラクタ
	virtual ~IParameterEditor(void) = default;

	/// @brief GUI描画処理
	virtual void DrawUI(void) = 0;
};

