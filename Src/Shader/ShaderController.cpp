#include "ShaderController.h"

// 静的メンバ変数の初期化
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

void ShaderController::CreateShaderDraw(ShaderLibrary::SHADER_TYPE _shaderType, int _x, int _y, int _textureHandle,
    float _scale, const ShaderMaterial& _material, int _normalMapHandle) const
{
    // 描画リクエストのベースを作る
    DrawRequest req(_x, _y, _textureHandle, _scale);
    req.normalMapHandle = _normalMapHandle;

    req.buffer.lightX = _material.GetLightDirX();
    req.buffer.lightY = _material.GetLightDirY();
    req.buffer.lightZ = _material.GetLightDirZ();
    req.buffer.ambient = _material.GetAmbient();

    req.buffer.time = _material.GetTime();
    req.buffer.waveSpeed = _material.GetWaveSpeed();
    req.buffer.waveForce = _material.GetWaveForce();
    req.buffer.useNormal = _material.IsUseNormalMap() ? 1.0f : 0.0f;

    // 職人に渡す
    ShaderBase* shader = shaderLibrary_->GetShader(_shaderType);
    if (shader)
    {
        shaderRenderer_->PixelShaderDraw(shader, req);
    }
}

void ShaderController::CreateShaderDrawRainy(int _x, int _y, const ShaderMaterial& _material) const
{
    // 描画リクエストのベースを作る
    DrawRequest req(_x, _y, -1, 1.0f);

    req.bufferRain.resolutionX = _material.GetResolutionX();
    req.bufferRain.resolutionY = _material.GetResolutionY();
    req.bufferRain.time = _material.GetTime();

    req.bufferRain.intensity = _material.GetRainIntensity();
    req.bufferRain.intensityBack = _material.GetRainIntensityBack();
    req.bufferRain.rainColor = _material.GetRainColor();

    // 職人に渡す
    ShaderBase* shader = shaderLibrary_->GetShader(ShaderLibrary::SHADER_TYPE::RAINY);
    if (shader)
    {
        shaderRenderer_->RainyShaderDraw(shader, req);
    }
}


void ShaderController::CreateShaderDrawTexScale(int _x, int _y, int _modelId, int _textureHandle, const ShaderMaterial& _material) const
{
    // 描画リクエストのベースを作る
    DrawRequest req(_x, _y, _textureHandle, 1.0f, _modelId);

    req.bufferTexScale.scaleX = _material.GetTexScaleX();
    req.bufferTexScale.scaleY = _material.GetTexScaleY();

    // 職人に渡す
    ShaderBase* shaderPS = shaderLibrary_->GetShader(ShaderLibrary::SHADER_TYPE::TEX_SCALE_PS);
    ShaderBase* shaderVS = shaderLibrary_->GetShader(ShaderLibrary::SHADER_TYPE::TEX_SCALE_VS);

    if (shaderPS && shaderVS)
    {
        shaderRenderer_->TexScaleShaderDraw(shaderPS, shaderVS, req);
    }
}

void ShaderController::Release(void)
{
    shaderRenderer_->Release();
    shaderLibrary_->Release();
}
