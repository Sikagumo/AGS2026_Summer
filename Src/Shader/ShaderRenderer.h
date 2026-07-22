#pragma once

#include <DxLib.h>
#include <array>

#include "ShaderBase.h"
#include "ShaderParameters.h"

class ShaderRenderer
{
public:
	/// @brief コンストラクタ
	ShaderRenderer(void);

	/// @brief デストラクタ 
	~ShaderRenderer(void);

	/// @brief 初期化 
	void Initialize(void);

	/// @brief ピクセルシェーダの描画
	/// @param _shader 使用するピクセルシェーダのポインタ
	/// @param request 描画のリクエスト情報
	void PixelShaderDraw(ShaderBase* _shader, const DrawRequest& _request) const;

	/// @brief 雨シェーダの描画
	/// @param _shader 使用するピクセルシェーダのポインタ
	/// @param request 描画のリクエスト情報
	void RainyShaderDraw(ShaderBase* _shader, const DrawRequest& _request) const;

	/// @brief 解放処理 
	void Release(void);

private:

	// 定数バッファのハンドル
	int constBufferHandle_;
	int constBufferHandleRain_;

	/// @brief 頂点情報の適用
	/// @param _v 頂点情報の配列
	/// @param _w 画像の幅
	/// @param _h 画像の高さ
	void ApplyVertices(std::array<VERTEX2DSHADER, 4>& _v, float _w, float _h) const;
};

