#include "ShaderRenderer.h"
#include <cstring>
#include "../Application.h" 

ShaderRenderer::ShaderRenderer(void)
{
}

ShaderRenderer::~ShaderRenderer(void)
{
    Release();
}

void ShaderRenderer::Initialize(void)
{
}

void ShaderRenderer::ExecuteCommand(const RenderCommand& _renderCommand)
{
    // 頂点シェーダ用定数バッファの更新と設定
    if (_renderCommand.vertexShaderHandleId != -1 && _renderCommand.vertexParameterSize > 0)
    {
        UpdateAndSetConstantBuffer(_renderCommand.vertexParameterData.data(), _renderCommand.vertexParameterSize, DX_SHADERTYPE_VERTEX, CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER);
    }

    // ピクセルシェーダ用定数バッファの更新と設定
    if (_renderCommand.pixelParameterSize > 0)
    {
        UpdateAndSetConstantBuffer(_renderCommand.pixelParameterData.data(), _renderCommand.pixelParameterSize, DX_SHADERTYPE_PIXEL, CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER);
    }

    // テクスチャのバインド
    if (_renderCommand.textureHandleId != -1)
    {
        SetUseTextureToShader(0, _renderCommand.textureHandleId);
    }

    if (_renderCommand.normalMapHandleId != -1)
    {
        SetUseTextureToShader(1, _renderCommand.normalMapHandleId);
    }

    // シェーダの設定
    if (_renderCommand.vertexShaderHandleId != -1)
    {
        SetUseVertexShader(_renderCommand.vertexShaderHandleId);
    }

    SetUsePixelShader(_renderCommand.pixelShaderHandleId);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

    // 描画タイプの分岐
    if (_renderCommand.renderType == RENDER_TYPE::DRAW_2D)
    {
        float drawWidth = static_cast<float>(Application::SCREEN_SIZE_X) * _renderCommand.scaleSize;
        float drawHeight = static_cast<float>(Application::SCREEN_SIZE_Y) * _renderCommand.scaleSize;

        std::array<VERTEX2DSHADER, 4> vertices;
        ApplyVertices(vertices, drawWidth, drawHeight);

        for (auto& vertex : vertices)
        {
            vertex.pos.x += _renderCommand.positionX;
            vertex.pos.y += _renderCommand.positionY;
        }

        DrawPrimitive2DToShader(vertices.data(), 4, DX_PRIMTYPE_TRIANGLESTRIP);
    }
    else if (_renderCommand.renderType == RENDER_TYPE::DRAW_3D)
    {
        MV1SetUseOrigShader(true);
        MV1DrawModel(_renderCommand.modelHandleId);
        MV1SetUseOrigShader(false);
    }

    // 状態のアンバインド（状態リーク防止のための後始末）
    SetUseTextureToShader(0, -1);
    SetUseTextureToShader(1, -1);
    SetUsePixelShader(-1);

    if (_renderCommand.vertexShaderHandleId != -1)
    {
        SetUseVertexShader(-1);
    }

    SetShaderConstantBuffer(-1, DX_SHADERTYPE_VERTEX, CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ShaderRenderer::Release(void)
{
    for (auto& pair : constantBufferMap_)
    {
        if (pair.second != -1)
        {
            DeleteShaderConstantBuffer(pair.second);
        }
    }
    constantBufferMap_.clear();
}

int ShaderRenderer::UpdateAndSetConstantBuffer(const void* _parameterPointer, int _parameterSize, int _shaderType, int _slotBegin)
{
    if (_parameterPointer == nullptr || _parameterSize <= 0)
    {
        return -1;
    }

    int constantBufferHandle = -1;

    if (constantBufferMap_.find(_parameterSize) == constantBufferMap_.end())
    {
        constantBufferHandle = CreateShaderConstantBuffer(_parameterSize);
        constantBufferMap_[_parameterSize] = constantBufferHandle;
    }
    else
    {
        constantBufferHandle = constantBufferMap_[_parameterSize];
    }

    if (constantBufferHandle == -1)
    {
        return -1;
    }

    void* bufferPointer = GetBufferShaderConstantBuffer(constantBufferHandle);
    if (bufferPointer != nullptr)
    {
        std::memcpy(bufferPointer, _parameterPointer, _parameterSize);
        UpdateShaderConstantBuffer(constantBufferHandle);
        SetShaderConstantBuffer(constantBufferHandle, _shaderType, _slotBegin);
    }

    return constantBufferHandle;
}

void ShaderRenderer::ApplyVertices(std::array<VERTEX2DSHADER, 4>& _vertices, float _width, float _height) const
{
    for (auto& vertex : _vertices)
    {
        vertex.rhw = 1.0f;
        vertex.pos.z = 0.0f;
        vertex.dif = GetColorU8(255, 255, 255, 255);
        vertex.spc = GetColorU8(0, 0, 0, 0);
        vertex.su = 1.0f;
        vertex.sv = 1.0f;
    }

    // 左上
    _vertices[0].pos = VGet(0.0f, 0.0f, 0.0f);
    _vertices[0].u = 0.0f;
    _vertices[0].v = 0.0f;

    // 右上
    _vertices[1].pos = VGet(_width, 0.0f, 0.0f);
    _vertices[1].u = 1.0f;
    _vertices[1].v = 0.0f;

    // 左下
    _vertices[2].pos = VGet(0.0f, _height, 0.0f);
    _vertices[2].u = 0.0f;
    _vertices[2].v = 1.0f;

    // 右下
    _vertices[3].pos = VGet(_width, _height, 0.0f);
    _vertices[3].u = 1.0f;
    _vertices[3].v = 1.0f;
}