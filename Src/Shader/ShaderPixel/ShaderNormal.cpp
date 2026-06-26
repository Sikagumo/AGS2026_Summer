#include "ShaderNormal.h"
#include <DxLib.h>

// ï`âÊÇÃèuä‘ÇæÇØégÇ§ÅAHLSLÇ∆100%àÍívÇ∑ÇÈìùçáç\ë¢ëÃ
struct alignas(16) IntegratedGpuBuffer
{
    float lightX = 1.0f; 
    float lightY = 1.0f;  
    float lightZ = 1.0f;  
    float ambient = 0.0f;
    float unusedPadding[3] = { 0.0f, 0.0f, 0.0f };
    float useNormal = 1.0f;
};

ShaderNormal::ShaderNormal(void)
    : lightX_(0.6f)
    , lightY_(0.6f)
    , lightZ_(1.0f)
    , ambient_(0.0f)
{
}

void ShaderNormal::Initialize(const char* _shaderPath)
{
    ShaderPixelBase::Initialize(_shaderPath);
    constantBuffer_ = CreateShaderConstantBuffer(sizeof(IntegratedGpuBuffer));
}

void ShaderNormal::SetLightDirection(float _x, float _y, float _z)
{
    lightX_ = _x; lightY_ = _y; lightZ_ = _z;
}

void ShaderNormal::SetAmbient(float _ambient)
{
    ambient_ = _ambient;
}

void ShaderNormal::Draw(int _x, int _y, int _textureHandle, int _normalMapHandle, float _scale)
{
    if (_textureHandle == -1 || _normalMapHandle == -1 || shaderHandle_ == -1 || constantBuffer_ == -1)
    {
        return;
    }

    float texWidth, texHight;
    GetGraphSizeF(_textureHandle, &texWidth, &texHight);
    float width = texWidth * _scale;
    float high = texHight * _scale;

    InitBaseVertexes(width, high);

    vertices_[0].pos = VGet(static_cast<float>(_x), static_cast<float>(_y), 0.0f);
    vertices_[1].pos = VGet(static_cast<float>(_x) + width, static_cast<float>(_y), 0.0f);
    vertices_[2].pos = VGet(static_cast<float>(_x), static_cast<float>(_y) + high, 0.0f);
    vertices_[3].pos = VGet(static_cast<float>(_x) + width, static_cast<float>(_y) + high, 0.0f);

    IntegratedGpuBuffer gpuBuffer{};
    gpuBuffer.lightX = lightX_;
    gpuBuffer.lightY = lightY_;
    gpuBuffer.lightZ = lightZ_;
    gpuBuffer.ambient = ambient_;

    ShaderPixelBase::UpdateConstantBuffer(gpuBuffer);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    SetUseTextureToShader(0, _textureHandle);
    SetUseTextureToShader(1, _normalMapHandle);
    SetUsePixelShader(shaderHandle_);

    DrawPrimitive2DToShader(vertices_.data(), static_cast<int>(vertices_.size()), DX_PRIMTYPE_TRIANGLESTRIP);

    SetUsePixelShader(-1); SetUseTextureToShader(1, -1); SetUseTextureToShader(0, -1);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 4); SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}