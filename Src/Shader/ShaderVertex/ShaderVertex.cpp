#include "ShaderVertex.h"

ShaderVertex::ShaderVertex(void)
    : vertexHandle_(-1)
{
}

ShaderVertex::~ShaderVertex(void)
{
    Release();
}

void ShaderVertex::Initialize(const char* _shaderPath)
{
    // 既にロードされている場合は解放
    Release();

    // ピクセルシェーダを読み込む
    vertexHandle_ = LoadVertexShader(_shaderPath);
}
void ShaderVertex::Release(void)
{
    if (vertexHandle_ != -1)
    {
        DeleteShader(vertexHandle_);
        vertexHandle_ = -1;
    }
}

void ShaderVertex::Apply(void)
{
    if (vertexHandle_ != -1)
    {
        SetUseVertexShader(vertexHandle_);
    }
}

void ShaderVertex::UnApply(void)
{
    SetUseVertexShader(-1);
}