#include "ShaderManager.h"

#include <DxLib.h>

#include "ShaderParameters.h"
#include "../Application.h"

// ê√ìIÉÅÉìÉoïœêîÇÃèâä˙âª
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
    , commonConstantBuffer_(-1)
{
}

ShaderManager::~ShaderManager(void)
{
    Release();
}

void ShaderManager::Initialize(void)
{
    screenHandle_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

    commonConstantBuffer_ = CreateShaderConstantBuffer(sizeof(IntegratedGpuBuffer));

    shaderNormal_ = std::make_unique<ShaderNormal>();
    shaderNormal_->Initialize((Application::PATH_SHADER + "NormalMap.cso").c_str());

    shaderWave_ = std::make_unique<ShaderWave>();
    shaderWave_->Initialize((Application::PATH_SHADER + "NormalMap.cso").c_str());

}

void ShaderManager::DrawNormalAndWave(int _x, int _y, int _textureHandle, int _normalMapHandle, float _scale)
{

    if (!shaderNormal_ || !shaderWave_)
    {
        return;
    }

    if (_textureHandle == -1 || _normalMapHandle == -1)
    {
        return;
    }

    IntegratedGpuBuffer gpuBuffer{};
    gpuBuffer.lightX = shaderNormal_->GetLightX();
    gpuBuffer.lightY = shaderNormal_->GetLightY();
    gpuBuffer.lightZ = shaderNormal_->GetLightZ();
    gpuBuffer.ambient = shaderNormal_->GetAmbient();
    gpuBuffer.time = shaderWave_->GetTime();
    gpuBuffer.waveSpeed = shaderWave_->GetSpeed();
    gpuBuffer.waveForce = shaderWave_->GetForce();
    gpuBuffer.useNormal = 1.0f;

    UpdateAndSetCommonConstantBuffer(gpuBuffer);

    float texWidth = Application::SCREEN_SIZE_X;
    float texHight = Application::SCREEN_SIZE_Y;

    float width = texWidth * _scale;
    float high = texHight * _scale;

    std::array<VERTEX2DSHADER, 4> localVertices;
    localVertices[0].pos = VGet(static_cast<float>(_x), static_cast<float>(_y), 0.0f);
    localVertices[0].u = 0.0f; 
    localVertices[0].v = 0.0f;
    localVertices[1].pos = VGet(static_cast<float>(_x) + width, static_cast<float>(_y), 0.0f);
    localVertices[1].u = 1.0f;
    localVertices[1].v = 0.0f;
    localVertices[2].pos = VGet(static_cast<float>(_x), static_cast<float>(_y) + high, 0.0f); 
    localVertices[2].u = 0.0f;
    localVertices[2].v = 1.0f;
    localVertices[3].pos = VGet(static_cast<float>(_x) + width, static_cast<float>(_y) + high, 0.0f); 
    localVertices[3].u = 1.0f; 
    localVertices[3].v = 1.0f;

    for (auto& v : localVertices)
    {
        v.rhw = 1.0f; v.spc = GetColorU8(0, 0, 0, 0); v.su = 1.0f; v.sv = 1.0f;
        v.dif = GetColorU8(255, 255, 255, 255);
    }

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    SetUseTextureToShader(0, _textureHandle);
    SetUseTextureToShader(1, _normalMapHandle);
    SetUsePixelShader(shaderNormal_->GetShaderHandle());
    DrawPrimitive2DToShader(localVertices.data(), static_cast<int>(localVertices.size()), DX_PRIMTYPE_TRIANGLESTRIP);
    SetUsePixelShader(-1);
    SetUseTextureToShader(1, -1);
    SetUseTextureToShader(0, -1);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 4);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ShaderManager::Release(void)
{
    if (commonConstantBuffer_ != -1)
    {
        DeleteShaderConstantBuffer(commonConstantBuffer_);
        commonConstantBuffer_ = -1;
    }

    if (shaderNormal_)
    {
        shaderNormal_->Release();
    }

    if (shaderWave_)
    {
        shaderWave_->Release();
    }
}