#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ShaderPixel/ShaderNormal.h"
#include "ShaderPixel/ShaderWave.h"

class ShaderManager
{
public:
	/// @brief インスタンの生成
	static void CreateInstance(void);

	/// @brief インスタンスを取得 
	/// @return インスタンスの参照
	static ShaderManager& GetInstance(void);

	/// @brief インスタンスの破棄
	static void DestroyInstance(void);

	/// @brief 初期化処理 
	void Initialize(void);

	void DrawNormalAndWave(int _x, int _y, int _textureHandle, int _normalMapHandle, float _scale);

	/// @brief 解放処理 
	void Release(void);

	/// @brief NormalShaderへのアクセス
	ShaderNormal* GetShaderNormal(void) const { return shaderNormal_.get(); }

	ShaderWave* GetShaderWave(void) const { return shaderWave_.get(); }

	/// @brief  
	/// @return 
	int GetScreenHandle(void) const { return screenHandle_; }

private:

	// シングルトン用インスタンス
	static ShaderManager* instance_;

	std::unique_ptr<ShaderNormal> shaderNormal_;
	std::unique_ptr<ShaderWave>   shaderWave_;

	// スクリーンのハンドル
	int screenHandle_;

	/// @brief コンストラクタ 
	ShaderManager(void);

	/// @brief デストラクタ
	~ShaderManager(void);

	/// @brief コピーコンストラクタを禁止する
	ShaderManager(const ShaderManager&) = delete;

	/// @brief 代入演算子を禁止する
	ShaderManager& operator=(const ShaderManager&) = delete;

	/// @brief ムーブコンストラクタを禁止する
	ShaderManager(ShaderManager&&) = delete;

	/// @brief ムーブ代入演算子を禁止する
	ShaderManager& operator=(ShaderManager&&) = delete;
};

