#pragma once

#include <string>
#include <Dxlib.h>

#include "../ShaderBase.h"

/// @brief ピクセルシェーダのリソース管理クラス
class ShaderPixel : public ShaderBase
{
public:
	/// @brief コンストラクタ
	ShaderPixel(void);

	/// @brief デストラクタ
	~ShaderPixel(void) override;

	/// @brief ピクセルシェーダの読み込み
	/// @param _path  シェーダのファイルパス
	void Initialize(const char* _shaderPath);

	/// @brief シェーダを解放する
	void Release(void);

	/// @brief ピクセルシェーダを適用をする
	void Apply(void) override;

	/// @brief ピクセルシェーダの適用を解除する
	void UnApply(void) override;

	/// @brief 現在のハンドルを取得
	int GetHandle(void) const { return pixelHandle_; }

private:
	// ピクセルシェーダのハンドル
	int pixelHandle_;
};

