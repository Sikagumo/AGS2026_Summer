#include "ShaderNormal.h"

ShaderNormal::ShaderNormal(void)
{
}

ShaderNormal::~ShaderNormal(void)
{
}

void ShaderNormal::Initialize(const char* _shaderPath)
{
	ShaderPixelBase::Initialize(_shaderPath);

	// このシェーダー専用の定数バッファサイズを確保（構造体サイズ）
	constantBuffer_ = CreateShaderConstantBuffer(sizeof(LightingParameter));
}

void ShaderNormal::Draw(int _x, int _y, int _textureHandle, 
	int _normalMapHandle, const LightingParameter& _param)
{
	// シェーダ適用
	SetUsePixelShader(shaderHandle_);

	// テクスチャを適用
	SetUseTextureToShader(0, _textureHandle);

	// ノーマルマップを適用
	SetUseTextureToShader(2, _normalMapHandle);

	// 定数バッファを更新
	UpdateConstantBuffer(_param);

	// 描画
	DrawGraph(_x, _y, _textureHandle, true);

	// 後処理
	SetUsePixelShader(-1);
	SetUseTextureToShader(0, -1);
	SetUseTextureToShader(2, -1);
}
