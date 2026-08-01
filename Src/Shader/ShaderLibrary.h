#pragma once

#include <memory>
#include <unordered_map>

#include "ShaderBase.h"
#include "ShaderPixel/ShaderPixel.h"
#include "ShaderVertex/ShaderVertex.h"

class ShaderLibrary
{
public:

	enum class SHADER_TYPE
	{
		NORMAL,
		WAVE,
		NORMAL_WAVE,
		RAINY,
		TEX_SCALE_PS,
		TEX_SCALE_VS,
	};

	/// @brief コンストラクタ
	ShaderLibrary(void);

	/// @brief デストラクタ 
	~ShaderLibrary(void);

	/// @brief 初期化 
	void Initialize(void);

	/// @brief 解放処理 
	void Release(void);
	
	/// @brief シェーダの取得
	/// @param _shaderType シェーダの種類
	/// @return シェーダのポインタ
	ShaderBase* GetShader(SHADER_TYPE _shaderType) const;


private:

	std::unordered_map<SHADER_TYPE, std::unique_ptr<ShaderPixel>> pixelShaders_;
	std::unordered_map<SHADER_TYPE, std::unique_ptr<ShaderVertex>> vertexShaders_;

	/// @brief ピクセルシェーダの読み込み
	/// @param _shaderType シェーダの種類
	/// @param _path シェーダのファイルパス
	void LoadPixelShader(SHADER_TYPE _shaderType, const char* _path);

	/// @brief 頂点シェーダの読み込み
	/// @param _shaderType シェーダの種類
	/// @param _path シェーダのファイルパス
	void LoadVertexShader(SHADER_TYPE _shaderType, const char* _path);
};

