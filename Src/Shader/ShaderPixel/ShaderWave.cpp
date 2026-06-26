#include "ShaderWave.h"
#include <DxLib.h>
#include <cstring>

struct alignas(16) IntegratedGpuBuffer
{
    float unusedPadding[4] = { 0.0f, 0.0f, 0.0f, 0.0f};
    float time = 0.0f; 
    float waveSpeed = 0.0f; 
    float waveForce = 0.0f; 
    float useNormal = 0.0f;
};

ShaderWave::ShaderWave(void)
    : time_(0.0f)
    , speed_(0.0f)
    , force_(0.0f)
{
}

void ShaderWave::Initialize(const char* _shaderPath)
{
    ShaderPixelBase::Initialize(_shaderPath);
}

void ShaderWave::SetWaveParam(float _time, float _speed, float _force)
{
    time_ = _time; speed_ = _speed; force_ = _force;
}

void ShaderWave::Draw(int _x, int _y, int _textureHandle, float _scale)
{
    if (_textureHandle == -1 || shaderHandle_ == -1 || constantBuffer_ == -1)
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
    gpuBuffer.time = time_;
    gpuBuffer.waveSpeed = speed_;
    gpuBuffer.waveForce = force_;

    ShaderPixelBase::UpdateConstantBuffer(gpuBuffer);

    SetShaderConstantBuffer(constantBuffer_, DX_SHADERTYPE_PIXEL, 4);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    SetUseTextureToShader(0, _textureHandle); 
    SetUsePixelShader(shaderHandle_);

    DrawPrimitive2DToShader(vertices_.data(), static_cast<int>(vertices_.size()), DX_PRIMTYPE_TRIANGLESTRIP);

    SetUsePixelShader(-1); SetUseTextureToShader(0, -1);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 4); SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}