#include "ShaderBase.h"

ShaderBase::ShaderBase(void)
    : shaderHandle_(-1)
    , constantBuffer_(-1)
{
}

void ShaderBase::Release(void)
{
    if (shaderHandle_ != -1)
    {
        DeleteShader(shaderHandle_);
        shaderHandle_ = -1;
    }

    if (constantBuffer_ != -1)
    {
        DeleteShaderConstantBuffer(constantBuffer_);
        constantBuffer_ = -1;
    }
}