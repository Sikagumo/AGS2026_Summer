#include "ShaderLibrary.h"
#include "../Application.h"

ShaderLibrary::ShaderLibrary(void)
	: pixelShaders_{}, vertexShaders_{}
{
}

ShaderLibrary::~ShaderLibrary(void)
{
	Release();
}

void ShaderLibrary::Initialize(void)
{
	LoadPixelShader(SHADER_TYPE::NORMAL, (Application::PATH_SHADER + "NormalMap.cso").c_str());
	LoadPixelShader(SHADER_TYPE::WAVE, (Application::PATH_SHADER + "NormalMap.cso").c_str());
	LoadPixelShader(SHADER_TYPE::NORMAL_WAVE, (Application::PATH_SHADER + "NormalMap.cso").c_str());
	LoadPixelShader(SHADER_TYPE::RAINY, (Application::PATH_SHADER + "Rainy.cso").c_str());
	
	LoadPixelShader(SHADER_TYPE::TEX_SCALE_PS, (Application::PATH_SHADER + "TexScalePS.cso").c_str());
	LoadVertexShader(SHADER_TYPE::TEX_SCALE_VS, (Application::PATH_SHADER + "TexScaleVS.cso").c_str());
}

void ShaderLibrary::Release(void)
{
	pixelShaders_.clear();
	vertexShaders_.clear();
}

void ShaderLibrary::LoadPixelShader(SHADER_TYPE _shaderType, const char* _path)
{
	auto shader = std::make_unique<ShaderPixel>();
	
	shader->Initialize(_path);

	// シェーダの所有権を移動して格納
	pixelShaders_[_shaderType] = std::move(shader);
}

void ShaderLibrary::LoadVertexShader(SHADER_TYPE _shaderType, const char* _path)
{
	auto shader = std::make_unique<ShaderVertex>();

	shader->Initialize(_path);

	// シェーダの所有権を移動して格納
	vertexShaders_[_shaderType] = std::move(shader);
}

ShaderBase* ShaderLibrary::GetShader(SHADER_TYPE _shaderType) const
{
	// ピクセルシェーダ
	auto pixelShader = pixelShaders_.find(_shaderType);
	if (pixelShader != pixelShaders_.end())
	{
		return pixelShader->second.get();
	}

	// 頂点シェーダ
	auto vertexShader = vertexShaders_.find(_shaderType);
	if (vertexShader != vertexShaders_.end())
	{
		return vertexShader->second.get();
	}
	return nullptr;
}

