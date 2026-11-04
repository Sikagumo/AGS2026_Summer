#include "ShaderRenderer.h"

#include <cstring>

#include "../Application.h" 

namespace
{
	// マジックナンバー排除用の定数
	constexpr int INVALID_HANDLE = -1;              // 無効なハンドルを示す値
	constexpr int TEXTURE_SLOT_0 = 0;               // テクスチャスロット0
	constexpr int TEXTURE_SLOT_1 = 1;               // テクスチャスロット1
	constexpr unsigned int BLEND_ALPHA_MAX = 255;   // 最大のアルファ値（不透明）
	constexpr float VERTEX_RHW = 1.0f;              // 2Dシェーダー用RHW値
	constexpr float VERTEX_POS_Z = 0.0f;            // 頂点のZ座標初期値
	constexpr float UV_MIN = 0.0f;                  // UV座標の最小値
	constexpr float UV_MAX = 1.0f;                  // UV座標の最大値
	constexpr size_t VERTEX_COUNT_2D = 4;           // 2D描画用頂点数
}

ShaderRenderer::ShaderRenderer(void)
	: currentVertexShaderHandleId_(INVALID_HANDLE)
	, currentPixelShaderHandleId_(INVALID_HANDLE)
	, currentTexture0HandleId_(INVALID_HANDLE)
	, currentTexture1HandleId_(INVALID_HANDLE)
	, constantBufferMap_()
{
}

ShaderRenderer::~ShaderRenderer(void)
{
	Release();
}

void ShaderRenderer::Initialize(void)
{
}

void ShaderRenderer::BeginBatch(void)
{
	// キャッシュを初期化する
	currentVertexShaderHandleId_ = INVALID_HANDLE;
	currentPixelShaderHandleId_ = INVALID_HANDLE;
	currentTexture0HandleId_ = INVALID_HANDLE;
	currentTexture1HandleId_ = INVALID_HANDLE;
}

void ShaderRenderer::ExecuteCommand(const RenderCommand& _renderCommand)
{
	if (_renderCommand.vertexShaderHandleId != INVALID_HANDLE && _renderCommand.vertexParameterSize > 0)
	{
		UpdateAndSetConstantBuffer(_renderCommand.vertexParameterData.data(), _renderCommand.vertexParameterSize, DX_SHADERTYPE_VERTEX, CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER);
	}

	if (_renderCommand.pixelParameterSize > 0)
	{
		UpdateAndSetConstantBuffer(_renderCommand.pixelParameterData.data(), _renderCommand.pixelParameterSize, DX_SHADERTYPE_PIXEL, CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER);
	}

	// テクスチャのバインド
	if (_renderCommand.textureHandleId != INVALID_HANDLE)
	{
		if (currentTexture0HandleId_ != _renderCommand.textureHandleId)
		{
			SetUseTextureToShader(TEXTURE_SLOT_0, _renderCommand.textureHandleId);
			currentTexture0HandleId_ = _renderCommand.textureHandleId;
		}
	}
	else
	{
		if (currentTexture0HandleId_ != INVALID_HANDLE)
		{
			SetUseTextureToShader(TEXTURE_SLOT_0, INVALID_HANDLE);
			currentTexture0HandleId_ = INVALID_HANDLE;
		}
	}

	if (_renderCommand.normalMapHandleId != INVALID_HANDLE)
	{
		if (currentTexture1HandleId_ != _renderCommand.normalMapHandleId)
		{
			SetUseTextureToShader(TEXTURE_SLOT_1, _renderCommand.normalMapHandleId);
			currentTexture1HandleId_ = _renderCommand.normalMapHandleId;
		}
	}
	else
	{
		if (currentTexture1HandleId_ != INVALID_HANDLE)
		{
			SetUseTextureToShader(TEXTURE_SLOT_1, INVALID_HANDLE);
			currentTexture1HandleId_ = INVALID_HANDLE;
		}
	}

	// 頂点シェーダの設定
	if (_renderCommand.vertexShaderHandleId != INVALID_HANDLE)
	{
		if (currentVertexShaderHandleId_ != _renderCommand.vertexShaderHandleId)
		{
			SetUseVertexShader(_renderCommand.vertexShaderHandleId);
			currentVertexShaderHandleId_ = _renderCommand.vertexShaderHandleId;
		}
	}
	else
	{
		if (currentVertexShaderHandleId_ != INVALID_HANDLE)
		{
			SetUseVertexShader(INVALID_HANDLE);
			currentVertexShaderHandleId_ = INVALID_HANDLE;
		}
	}

	// ピクセルシェーダの設定
	if (_renderCommand.pixelShaderHandleId != INVALID_HANDLE)
	{
		// 前回と違うシェーダが指定された時だけセットする
		if (currentPixelShaderHandleId_ != _renderCommand.pixelShaderHandleId)
		{
			SetUsePixelShader(_renderCommand.pixelShaderHandleId);
			currentPixelShaderHandleId_ = _renderCommand.pixelShaderHandleId;
		}
	}
	else
	{
		if (currentPixelShaderHandleId_ != INVALID_HANDLE)
		{
			SetUsePixelShader(INVALID_HANDLE);
			currentPixelShaderHandleId_ = INVALID_HANDLE;
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_MAX);

	if (_renderCommand.renderType == RENDER_TYPE::DRAW_2D)
	{
		const float drawWidth = static_cast<float>(Application::SCREEN_SIZE_X) * _renderCommand.scaleSize;
		const float drawHeight = static_cast<float>(Application::SCREEN_SIZE_Y) * _renderCommand.scaleSize;

		std::array<VERTEX2DSHADER, VERTEX_COUNT_2D> vertices;
		ApplyVertices(vertices, drawWidth, drawHeight);

		for (auto& vertex : vertices)
		{
			vertex.pos.x += _renderCommand.positionX;
			vertex.pos.y += _renderCommand.positionY;
		}

		DrawPrimitive2DToShader(vertices.data(), static_cast<int>(VERTEX_COUNT_2D), DX_PRIMTYPE_TRIANGLESTRIP);
	}
	else if (_renderCommand.renderType == RENDER_TYPE::DRAW_3D)
	{
		if (_renderCommand.isClamp)
		{
			SetTextureAddressModeUV(DX_TEXADDRESS_CLAMP, DX_TEXADDRESS_CLAMP);
		}
		else
		{
			SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);
		}

		MV1SetUseOrigShader(true);
		MV1DrawModel(_renderCommand.modelHandleId);
		MV1SetUseOrigShader(false);

		SetTextureAddressModeUV(DX_TEXADDRESS_CLAMP, DX_TEXADDRESS_CLAMP);
	}
}

void ShaderRenderer::EndBatch(void)
{
	// 全ての描画が終わった後にまとめて状態をアンバインドする
	SetUseTextureToShader(TEXTURE_SLOT_0, INVALID_HANDLE);
	SetUseTextureToShader(TEXTURE_SLOT_1, INVALID_HANDLE);
	SetUsePixelShader(INVALID_HANDLE);
	SetUseVertexShader(INVALID_HANDLE);

	SetShaderConstantBuffer(INVALID_HANDLE, DX_SHADERTYPE_VERTEX, CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER);
	SetShaderConstantBuffer(INVALID_HANDLE, DX_SHADERTYPE_PIXEL, CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// キャッシュをリセットする
	currentVertexShaderHandleId_ = INVALID_HANDLE;
	currentPixelShaderHandleId_ = INVALID_HANDLE;
	currentTexture0HandleId_ = INVALID_HANDLE;
	currentTexture1HandleId_ = INVALID_HANDLE;
}

void ShaderRenderer::Release(void)
{
	for (auto& pair : constantBufferMap_)
	{
		if (pair.second != INVALID_HANDLE)
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
		return INVALID_HANDLE;
	}

	int constantBufferHandle = INVALID_HANDLE;

	if (constantBufferMap_.find(_parameterSize) == constantBufferMap_.end())
	{
		constantBufferHandle = CreateShaderConstantBuffer(_parameterSize);
		constantBufferMap_[_parameterSize] = constantBufferHandle;
	}
	else
	{
		constantBufferHandle = constantBufferMap_[_parameterSize];
	}

	if (constantBufferHandle == INVALID_HANDLE)
	{
		return INVALID_HANDLE;
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
		vertex.rhw = VERTEX_RHW;
		vertex.pos.z = VERTEX_POS_Z;
		vertex.dif = GetColorU8(255, 255, 255, 255);
		vertex.spc = GetColorU8(0, 0, 0, 0);
		vertex.su = UV_MAX;
		vertex.sv = UV_MAX;
	}

	// 左上
	_vertices[0].pos = VGet(UV_MIN, UV_MIN, VERTEX_POS_Z);
	_vertices[0].u = UV_MIN;
	_vertices[0].v = UV_MIN;

	// 右上
	_vertices[1].pos = VGet(_width, UV_MIN, VERTEX_POS_Z);
	_vertices[1].u = UV_MAX;
	_vertices[1].v = UV_MIN;

	// 左下
	_vertices[2].pos = VGet(UV_MIN, _height, VERTEX_POS_Z);
	_vertices[2].u = UV_MIN;
	_vertices[2].v = UV_MAX;

	// 右下
	_vertices[3].pos = VGet(_width, _height, VERTEX_POS_Z);
	_vertices[3].u = UV_MAX;
	_vertices[3].v = UV_MAX;
}