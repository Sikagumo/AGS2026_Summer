#pragma once

#include <string>
#include <Dxlib.h>

#include "../ShaderBase.h"

/// @brief 頂点シェーダのリソース管理クラス
class ShaderVertex : public ShaderBase
{
public:

	/// @brief コンストラクタ
	ShaderVertex(void);

	/// @brief デストラクタ
	~ShaderVertex(void) override;

	/// @brief 頂点シェーダの読み込み
	/// @param _path  シェーダのファイルパス
	void Initialize(const char* _shaderPath);

	/// @brief シェーダを解放する
	void Release(void);

	/// @brief 頂点シェーダを適用をする
	void Apply(void) override;

	/// @brief 頂点シェーダの適用を解除する
	void UnApply(void) override;

	/// @brief 現在のハンドルを取得
	int GetHandle(void) const { return vertexHandle_; }

private:
	// 頂点シェーダのハンドル
	int vertexHandle_;
};

