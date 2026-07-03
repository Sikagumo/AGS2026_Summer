#include "ShaderPixel.h"

ShaderPixel::ShaderPixel(void)
    : pixelHandle_(-1)
{
}

ShaderPixel::~ShaderPixel(void)
{
    Release();
}

void ShaderPixel::Initialize(const char* _shaderPath)
{
    // 既にロードされている場合は解放
    Release();

    // ピクセルシェーダを読み込む
    pixelHandle_ = LoadPixelShader(_shaderPath);
}
void ShaderPixel::Release(void)
{
    if (pixelHandle_ != -1)
    {
        DeleteShader(pixelHandle_);
        pixelHandle_ = -1;
    }
}

void ShaderPixel::Apply(void)
{
    if (pixelHandle_ != -1)
    {
        SetUsePixelShader(pixelHandle_);
    }
}

void ShaderPixel::UnApply(void)
{
    SetUsePixelShader(-1);
}