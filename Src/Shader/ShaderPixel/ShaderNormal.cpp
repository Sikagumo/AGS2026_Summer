#include "ShaderNormal.h"

#include <DxLib.h>

#include "../../Application.h"

ShaderNormal::ShaderNormal(void)
{
}

ShaderNormal::~ShaderNormal(void)
{
}

void ShaderNormal::Initialize(const char* shaderPath)
{
    ShaderPixelBase::Initialize(shaderPath);
    constantBuffer_ = CreateShaderConstantBuffer(sizeof(LightingParameter));
}

void ShaderNormal::SetLightDirection(float _x, float _y, float _z)
{
    param_.lightX = _x;
    param_.lightY = _y;
    param_.lightZ = _z;
}

void ShaderNormal::Draw(int _x, int _y, int _textureHandle, int _normalMapHandle, float _scale)
{
    if (_textureHandle == -1 || _normalMapHandle == -1
        || shaderHandle_ == -1 || constantBuffer_ == -1)
    {
        return;
    }

    float texWidth, texHight;
    GetGraphSizeF(_textureHandle, &texWidth, &texHight);
    InitBaseVertexes(texWidth, texHight);

    float width = texWidth * _scale;
    float high = texHight * _scale;

    vertices_[0].pos = VGet(static_cast<float>(_x), static_cast<float>(_y), 0.0f);
    vertices_[1].pos = VGet(static_cast<float>(_x) + width, static_cast<float>(_y), 0.0f);
    vertices_[2].pos = VGet(static_cast<float>(_x), static_cast<float>(_y) + high, 0.0f);
    vertices_[3].pos = VGet(static_cast<float>(_x) + width, static_cast<float>(_y) + high, 0.0f);

    ShaderPixelBase::UpdateConstantBuffer(param_);
    SetShaderConstantBuffer(constantBuffer_, DX_SHADERTYPE_PIXEL, 4);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

    SetUseTextureToShader(0, _textureHandle); 
    SetUseTextureToShader(1, _normalMapHandle);  
 
    SetUsePixelShader(shaderHandle_);

    DxLib::DrawPrimitive2DToShader(vertices_.data(), 
        static_cast<int>(vertices_.size()), DX_PRIMTYPE_TRIANGLESTRIP);

    SetUsePixelShader(-1);
    SetUseTextureToShader(1, -1);
    SetUseTextureToShader(0, -1);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 4);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}