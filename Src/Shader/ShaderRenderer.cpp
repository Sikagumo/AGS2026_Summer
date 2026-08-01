#include "ShaderRenderer.h"

#include <cstring>

#include "../Application.h"

ShaderRenderer::ShaderRenderer(void)
	: constBufferHandle_(-1), constBufferHandleRain_(-1), constBufferHandleTexScale_(-1)
{
}

ShaderRenderer::~ShaderRenderer(void)
{
	Release();
}

void ShaderRenderer::Initialize(void)
{
	constBufferHandle_ = CreateShaderConstantBuffer(sizeof(IntegratedGpuBuffer));
	constBufferHandleRain_ = CreateShaderConstantBuffer(sizeof(IntegratedGpuBufferRain));
	constBufferHandleTexScale_ = CreateShaderConstantBuffer(sizeof(IntegratedGpuBufferTexScale));
}

void ShaderRenderer::PixelShaderDraw(ShaderBase* _shader, const DrawRequest& _request) const
{
	if (!_shader || constBufferHandle_ == -1 || _request.textureHandle == -1)
	{
		return;
	}

	void* gpuBuffer = GetBufferShaderConstantBuffer(constBufferHandle_);
	if (gpuBuffer)
	{
		std::memcpy(gpuBuffer, &_request.buffer, sizeof(IntegratedGpuBuffer));
		UpdateShaderConstantBuffer(constBufferHandle_);
	}

	const float width = static_cast<float>(Application::SCREEN_SIZE_X) * _request.scale;
	const float height = static_cast<float>(Application::SCREEN_SIZE_Y) * _request.scale;

	std::array<VERTEX2DSHADER, 4> vertices;
	ApplyVertices(vertices, width, height);

	for (auto& v : vertices)
	{
		v.pos.x += static_cast<float>(_request.x);
		v.pos.y += static_cast<float>(_request.y);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetShaderConstantBuffer(constBufferHandle_, DX_SHADERTYPE_PIXEL, CONSTANT_BUF_SLOT_PS);
	SetUseTextureToShader(0, _request.textureHandle);

	if (_request.normalMapHandle != -1)
	{
		SetUseTextureToShader(1, _request.normalMapHandle);
	}

	_shader->Apply();
	DrawPrimitive2DToShader(vertices.data(), 4, DX_PRIMTYPE_TRIANGLESTRIP);
	_shader->UnApply();

	if (_request.normalMapHandle != -1)
	{
		SetUseTextureToShader(1, -1);
	}
	SetUseTextureToShader(0, -1);
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 0);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ShaderRenderer::RainyShaderDraw(ShaderBase* _shader, const DrawRequest& _request) const
{
	if (!_shader || constBufferHandleRain_ == -1)
	{
		return;
	}

	void* gpuBuffer = GetBufferShaderConstantBuffer(constBufferHandleRain_);
	if (gpuBuffer)
	{
		std::memcpy(gpuBuffer, &_request.bufferRain, sizeof(IntegratedGpuBufferRain));
		UpdateShaderConstantBuffer(constBufferHandleRain_);
	}

	const float width = static_cast<float>(Application::SCREEN_SIZE_X) * _request.scale;
	const float height = static_cast<float>(Application::SCREEN_SIZE_Y) * _request.scale;

	std::array<VERTEX2DSHADER, 4> vertices;
	ApplyVertices(vertices, width, height);

	for (auto& v : vertices)
	{
		v.pos.x += static_cast<float>(_request.x);
		v.pos.y += static_cast<float>(_request.y);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetShaderConstantBuffer(constBufferHandleRain_, DX_SHADERTYPE_PIXEL, CONSTANT_BUF_SLOT_PS);

	if (_request.textureHandle != -1)
	{
		SetUseTextureToShader(0, _request.textureHandle);
	}

	if (_request.normalMapHandle != -1)
	{
		SetUseTextureToShader(1, _request.normalMapHandle);
	}

	_shader->Apply();
	DrawPrimitive2DToShader(vertices.data(), 4, DX_PRIMTYPE_TRIANGLESTRIP);
	_shader->UnApply();

	if (_request.normalMapHandle != -1)
	{
		SetUseTextureToShader(1, -1);
	}

	SetUseTextureToShader(0, -1);
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 0);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


void ShaderRenderer::TexScaleShaderDraw(ShaderBase* _shaderPS, ShaderBase* _shaderVS, const DrawRequest& _request) const
{
	if (!_shaderVS || !_shaderPS || constBufferHandleTexScale_ == -1 || _request.modelId == -1)
	{
		return;
	}

	// 定数バッファへUVスケール値を書き込む
	void* gpuBuffer = GetBufferShaderConstantBuffer(constBufferHandleTexScale_);
	if (gpuBuffer)
	{
		std::memcpy(gpuBuffer, &_request.bufferTexScale, sizeof(IntegratedGpuBufferTexScale));
		UpdateShaderConstantBuffer(constBufferHandleTexScale_);
	}

	// オリジナルシェーダ設定(ON)
	MV1SetUseOrigShader(true);

	// 頂点シェーダー用の定数バッファを定数バッファレジスタ(b7)にセット
	SetShaderConstantBuffer(constBufferHandleTexScale_, DX_SHADERTYPE_VERTEX, CONSTANT_BUF_SLOT_VS);

	// 頂点シェーダ設定
	_shaderVS->Apply();

	// テクスチャの設定
	if (_request.textureHandle != -1)
	{
		SetUseTextureToShader(0, _request.textureHandle);
	}

	// ピクセルシェーダ設定
	_shaderPS->Apply();

	// テクスチャアドレスタイプをWRAPに変更
	SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);

	// モデル描画
	MV1DrawModel(_request.modelId);


	// 後始末
	//-----------------------------------------

	// テクスチャ解除
	SetUseTextureToShader(0, -1);

	// テクスチャアドレスタイプを元に戻す
	SetTextureAddressModeUV(DX_TEXADDRESS_CLAMP, DX_TEXADDRESS_CLAMP);

	// ピクセルシェーダ解除
	_shaderPS->UnApply();

	// 頂点シェーダ解除
	_shaderVS->UnApply();

	// 頂点シェーダー用の定数バッファレジスタを解除
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_VERTEX, CONSTANT_BUF_SLOT_VS);

	// オリジナルシェーダ設定(OFF)
	MV1SetUseOrigShader(false);
	//-----------------------------------------
}

void ShaderRenderer::Release(void)
{
	if (constBufferHandle_ != -1)
	{
		DeleteShaderConstantBuffer(constBufferHandle_);
		constBufferHandle_ = - 1;
	}

	if (constBufferHandleRain_ != -1)
	{
		DeleteShaderConstantBuffer(constBufferHandleRain_);
		constBufferHandleRain_ = - 1;
	}

	if (constBufferHandleTexScale_ != -1)
	{
		DeleteShaderConstantBuffer(constBufferHandleTexScale_);
		constBufferHandleTexScale_ = - 1;
	}
}

void ShaderRenderer::ApplyVertices(std::array<VERTEX2DSHADER, 4>& _vertices, 
	float _width, float _height) const
{
	for (auto& v : _vertices)
	{
		v.rhw = 1.0f;
		v.pos.z = 0.0f;
		v.dif = GetColorU8(255, 255, 255, 255);
		v.spc = GetColorU8(0, 0, 0, 0);
		v.su = 1.0f;
		v.sv = 1.0f;
	}

	// 左上
	_vertices[0].pos = VGet(0.0f, 0.0f, 0.0f);
	_vertices[0].u = 0.0f;
	_vertices[0].v = 0.0f;

	// 右上
	_vertices[1].pos = VGet(_width, 0.0f, 0.0f);
	_vertices[1].u = 1.0f;
	_vertices[1].v = 0.0f;

	// 左下
	_vertices[2].pos = VGet(0.0f, _height, 0.0f);
	_vertices[2].u = 0.0f;
	_vertices[2].v = 1.0f;

	// 右下
	_vertices[3].pos = VGet(_width, _height, 0.0f);
	_vertices[3].u = 1.0f;
	_vertices[3].v = 1.0f;
}
