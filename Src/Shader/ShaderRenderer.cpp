#include "ShaderRenderer.h"

#include <cstring>

#include "../Application.h"

ShaderRenderer::ShaderRenderer(void)
	: constBufferHandle_(-1)
{
}

ShaderRenderer::~ShaderRenderer(void)
{
	Release();
}

void ShaderRenderer::Initialize(void)
{
	constBufferHandle_ = CreateShaderConstantBuffer(sizeof(IntegratedGpuBuffer));
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
	SetShaderConstantBuffer(constBufferHandle_, DX_SHADERTYPE_PIXEL, 4);
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

void ShaderRenderer::Release(void)
{
	if (constBufferHandle_ != -1)
	{
		DeleteShaderConstantBuffer(constBufferHandle_);
		constBufferHandle_ = - 1;
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

	// ç∂è„
	_vertices[0].pos = VGet(0.0f, 0.0f, 0.0f);
	_vertices[0].u = 0.0f;
	_vertices[0].v = 0.0f;

	// âEè„
	_vertices[1].pos = VGet(_width, 0.0f, 0.0f);
	_vertices[1].u = 1.0f;
	_vertices[1].v = 0.0f;

	// ç∂â∫
	_vertices[2].pos = VGet(0.0f, _height, 0.0f);
	_vertices[2].u = 0.0f;
	_vertices[2].v = 1.0f;

	// âEâ∫
	_vertices[3].pos = VGet(_width, _height, 0.0f);
	_vertices[3].u = 1.0f;
	_vertices[3].v = 1.0f;
}
