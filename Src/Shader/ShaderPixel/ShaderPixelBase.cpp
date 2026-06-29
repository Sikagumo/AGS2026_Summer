#include "ShaderPixelBase.h"

ShaderPixelBase::ShaderPixelBase(void)
    : vertices_{ 0, 0, 0, 0 }
{
}

ShaderPixelBase::~ShaderPixelBase(void)
{
}

void ShaderPixelBase::Initialize(const char* _shaderPath)
{
    // ピクセルシェーダの読み込み
    shaderHandle_ = LoadPixelShader(_shaderPath);
}

void ShaderPixelBase::InitBaseVertexes(float w, float h)
{
    for (int i = 0; i < 4; i++)
    {
        vertices_[i].rhw = 1.0f;
        vertices_[i].pos.z = 0.0f;
        vertices_[i].dif = GetColorU8(255, 255, 255, 255);
        vertices_[i].spc = GetColorU8(0, 0, 0, 0);
        vertices_[i].su = 1.0f;
        vertices_[i].sv = 1.0f;
    }

    // 左上, 右上, 左下, 右下 の順でセット
    vertices_[0].pos = VGet(0.0f, 0.0f, 0.0f);
    vertices_[0].u = 0.0f; vertices_[0].v = 0.0f;

    vertices_[1].pos = VGet(w, 0.0f, 0.0f);
    vertices_[1].u = 1.0f; vertices_[1].v = 0.0f;

    vertices_[2].pos = VGet(0.0f, h, 0.0f);
    vertices_[2].u = 0.0f; vertices_[2].v = 1.0f;
    
    vertices_[3].pos = VGet(w, h, 0.0f);
    vertices_[3].u = 1.0f; vertices_[3].v = 1.0f;
}