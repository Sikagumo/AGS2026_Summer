#include "ShaderManager.h"

#include <DxLib.h>

#include "../Application.h"

// 静的メンバ変数の初期化
static std::unique_ptr<ShaderManager> instance = nullptr;

void ShaderManager::CreateInstance(void)
{
    if (instance == nullptr)
    {
        instance.reset(new ShaderManager());
    }
}

ShaderManager& ShaderManager::GetInstance(void)
{
    return *instance;
}

void ShaderManager::DestroyInstance(void)
{
    instance.reset();
}

ShaderManager::ShaderManager(void)
{
}

void ShaderManager::Initialize(void)
{
    // NormalShaderのインスタンス化と初期化
    shaderNormal_ = std::make_unique<ShaderNormal>();
    shaderNormal_->Initialize((Application::PATH_SHADER + "NormalMap.cso").c_str());
}

void ShaderManager::Release(void)
{
    if (shaderNormal_)
    {
        shaderNormal_->Release();
        shaderNormal_.reset();
    }
}