#include "ShaderLibrary.h"
#include "../Application.h"

ShaderLibrary::ShaderLibrary(void)
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
}

void ShaderLibrary::Release(void)
{
	pixelShaders_.clear();
}

void ShaderLibrary::LoadPixelShader(SHADER_TYPE _shaderType, const char* _path)
{
	auto shader = std::make_unique<ShaderPixel>();
	
	shader->Initialize(_path);

	// シェーダの所有権を移動して格納
	pixelShaders_[_shaderType] = std::move(shader);
}

ShaderBase* ShaderLibrary::GetShader(SHADER_TYPE _shaderType) const
{
	auto iterator = pixelShaders_.find(_shaderType);
	
	if (iterator != pixelShaders_.end())
	{
		return iterator->second.get();
	}
	return nullptr;
}

