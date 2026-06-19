#include "ShaderPixelBase.h"

#include <DxLib.h>

ShaderPixelBase::ShaderPixelBase(void)
{
}

ShaderPixelBase::~ShaderPixelBase(void)
{
}

void ShaderPixelBase::Initialize(const char* shaderPath)
{
    shaderHandle_ = LoadPixelShader(shaderPath);

    constantBuffer_ = CreateShaderConstantBuffer(sizeof(float) * 4 * 4);
}