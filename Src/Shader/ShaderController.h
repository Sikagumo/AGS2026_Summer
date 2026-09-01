#pragma once

#include <memory>
#include <vector>

#include "ShaderParameters.h"
#include "ShaderRenderer.h"
#include "RenderCommand.h"
#include "../Manager/Generic/ResourceManager.h" 

/// @brief シェーダシステムの窓口
class ShaderController
{
public:

	/// @brief インスタンスの生成
	static void CreateInstance(void);

	/// @brief インスタンスを取得 
	/// @return インスタンスの参照
	static ShaderController& GetInstance(void);

	/// @brief インスタンスの破棄
	static void DestroyInstance(void);

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief 解放処理 
	void Release(void);

	/// @brief 溜まった描画命令をソートして実行する
	void ExecuteDrawCommands(void);

	/// @brief 2Dモデル用の描画命令追加処理
	/// @tparam ParameterType 定数バッファの構造体型
	/// @param _pixelShaderSource ピクセルシェーダのリソース指定
	/// @param _x X座標
	/// @param _y Y座標
	/// @param _scale 拡大率
	/// @param _parameters 定数バッファパラメータ構造体
	/// @param _textureHandle 画像ハンドル
	/// @param _normalMapHandle ノーマルマップのハンドル
	template <typename ParameterType>
	void Draw2D(ResourceManager::SRC _pixelShaderSource, int _x, int _y, float _scale,
		const ParameterType& _parameters, int _textureHandle = -1, int _normalMapHandle = -1)
	{
		const Resource& pixelShaderResource = ResourceManager::GetInstance().Load(_pixelShaderSource);
		if (pixelShaderResource.GetHandleId() == -1)
		{
			return;
		}

		RenderCommand renderCommand;
		renderCommand.renderType = RENDER_TYPE::DRAW_2D;
		renderCommand.vertexShaderHandleId = -1;
		renderCommand.pixelShaderHandleId = pixelShaderResource.GetHandleId();
		renderCommand.modelHandleId = -1;

		renderCommand.positionX = static_cast<float>(_x);
		renderCommand.positionY = static_cast<float>(_y);
		renderCommand.scaleSize = _scale;

		renderCommand.textureHandleId = _textureHandle;
		renderCommand.normalMapHandleId = _normalMapHandle;

		// ピクセルシェーダ用パラメータとして格納
		const unsigned char* BYTE_POINTER = reinterpret_cast<const unsigned char*>(&_parameters);
		renderCommand.pixelParameterData.assign(BYTE_POINTER, BYTE_POINTER + sizeof(ParameterType));
		renderCommand.pixelParameterSize = sizeof(ParameterType);

		renderCommand.vertexParameterSize = 0;

		renderCommandQueue_.push_back(renderCommand);
	}

	/// @brief 3Dモデル用の描画命令追加処理
	/// @tparam VertexParameterType 頂点シェーダの定数バッファ構造体型
	/// @tparam PixelParameterType ピクセルシェーダの定数バッファ構造体型
	/// @param _vertexShaderSource 頂点シェーダのリソース指定
	/// @param _pixelShaderSource ピクセルシェーダのリソース指定
	/// @param _modelHandle 3Dモデルのハンドル
	/// @param _vertexParameters 頂点シェーダ用定数バッファパラメータ構造体
	/// @param _pixelParameters ピクセルシェーダ用定数バッファパラメータ構造体
	/// @param _textureSlot バインドするレジスタのスロット番号
	/// @param _textureHandle バインドするテクスチャのハンドル
	template <typename VertexParameterType, typename PixelParameterType>
	void Draw3D(ResourceManager::SRC _vertexShaderSource, ResourceManager::SRC _pixelShaderSource,
		int _modelHandle, const VertexParameterType& _vertexParameters, const PixelParameterType& _pixelParameters,
		int _textureSlot = -1, int _textureHandle = -1, bool _isClamp = false)
	{
		const Resource& VERTEX_SHADER_RESOURCE = ResourceManager::GetInstance().Load(_vertexShaderSource);
		const Resource& PIXEL_SHADER_RESOURCE = ResourceManager::GetInstance().Load(_pixelShaderSource);

		if (PIXEL_SHADER_RESOURCE.GetHandleId() == -1)
		{
			return;
		}

		RenderCommand renderCommand;
		renderCommand.renderType = RENDER_TYPE::DRAW_3D;
		renderCommand.vertexShaderHandleId = VERTEX_SHADER_RESOURCE.GetHandleId();
		renderCommand.pixelShaderHandleId = PIXEL_SHADER_RESOURCE.GetHandleId();
		renderCommand.modelHandleId = _modelHandle;

		renderCommand.positionX = 0.0f;
		renderCommand.positionY = 0.0f;
		renderCommand.scaleSize = 1.0f;

		renderCommand.textureHandleId = _textureHandle;
		renderCommand.normalMapHandleId = -1;
		renderCommand.isClamp = _isClamp;

		// 頂点パラメータ関連
		const unsigned char* VERTEX_BYTE_POINTER = reinterpret_cast<const unsigned char*>(&_vertexParameters);
		renderCommand.vertexParameterData.assign(VERTEX_BYTE_POINTER, VERTEX_BYTE_POINTER + sizeof(VertexParameterType));
		renderCommand.vertexParameterSize = sizeof(VertexParameterType);

		// ピクセルパラメータ関連
		const unsigned char* PIXEL_BYTE_POINTER = reinterpret_cast<const unsigned char*>(&_pixelParameters);
		renderCommand.pixelParameterData.assign(PIXEL_BYTE_POINTER, PIXEL_BYTE_POINTER + sizeof(PixelParameterType));
		renderCommand.pixelParameterSize = sizeof(PixelParameterType);

		renderCommandQueue_.push_back(renderCommand);
	}

private:

	// シングルトンのインスタンス
	static ShaderController* instance_;                                   

	// シェーダの描画クラス
	std::unique_ptr<ShaderRenderer> shaderRenderer_;                      

	// 描画命令のリスト
	std::vector<RenderCommand> renderCommandQueue_;                        

	/// @brief コンストラクタ 
	ShaderController(void);

	/// @brief デストラクタ
	~ShaderController(void);

	/// @brief コピーコンストラクタを禁止する
	ShaderController(const ShaderController&) = delete;

	/// @brief 代入演算子を禁止する
	ShaderController& operator=(const ShaderController&) = delete;
};