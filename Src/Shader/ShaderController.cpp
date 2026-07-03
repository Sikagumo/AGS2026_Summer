#include "ShaderController.h"

// Ã“Iƒƒ“ƒo•Ï”‚Ì‰Šú‰»
ShaderController* ShaderController::instance_ = nullptr;

void ShaderController::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new ShaderController();
    }
}

ShaderController& ShaderController::GetInstance(void)
{
    return *instance_;
}

void ShaderController::DestroyInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

ShaderController::ShaderController(void)
    : shaderLibrary_(nullptr)
    , shaderRenderer_(nullptr)
{
    shaderLibrary_ = std::make_unique<ShaderLibrary>();
    shaderRenderer_ = std::make_unique<ShaderRenderer>();
}

ShaderController::~ShaderController(void)
{
    Release();
}

void ShaderController::Initialize(void)
{
    shaderLibrary_->Initialize();
    shaderRenderer_->Initialize();
}

void ShaderController::Draw(SHADER_TYPE _shaderType, const DrawRequest& _request) const
{
    ShaderBase* shader = shaderLibrary_->GetShader(_shaderType);
    
    if (shader)
    {
        shaderRenderer_->PixelShaderDraw(shader, _request);
    }
}

void ShaderController::Release(void)
{
    shaderRenderer_->Release();
    shaderLibrary_->Release();
}
