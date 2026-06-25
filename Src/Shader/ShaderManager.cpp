#include "ShaderManager.h"

#include <DxLib.h>

#include "../Application.h"

// 静的メンバ変数の初期化
ShaderManager* ShaderManager::instance_ = nullptr;

void ShaderManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new ShaderManager();
    }
}

ShaderManager& ShaderManager::GetInstance(void)
{
    return *instance_;
}

void ShaderManager::DestroyInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

ShaderManager::ShaderManager(void)
    : screenHandle_(-1)
{
}

ShaderManager::~ShaderManager(void)
{
}

void ShaderManager::Initialize(void)
{
    screenHandle_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

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