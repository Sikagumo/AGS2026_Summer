#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ShaderPixel/ShaderNormal.h"

class ShaderManager
{
public:
	/// @brief インスタンの生成
	static void CreateInstance(void);

	/// @brief インスタンスを取得 
	/// @return インスタンスの参照
	static ShaderManager& GetInstance(void);

	/// @brief インスタンスの破棄
	void DestroyInstance(void);

	/// @brief 初期化処理 
	void Initialize(void);

	/// @brief 解放処理 
	void Release(void);

	/// @brief NormalShaderへのアクセス
	ShaderNormal* GetNormalShader(void) const { return shaderNormal_.get(); }

private:

	// シングルトン用インスタンス
	static ShaderManager* instance_;

	std::unique_ptr<ShaderNormal> shaderNormal_;

	ShaderManager(void);
	~ShaderManager(void) = default;

	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;
};

