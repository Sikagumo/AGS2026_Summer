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

	/// @brief 作ったスクリーンを取得する
	/// @return 作ったスクリーン
	int GetScreenHandle(void) const { return screenHandle_; }

	int GetCommonConstantBuffer(void) const { return commonConstantBuffer_; }

	template <typename T>
	void UpdateAndSetCommonConstantBuffer(const T& _constantData, int _shaderType = DX_SHADERTYPE_PIXEL, int _slotIndex = 4)
	{
		if (commonConstantBuffer_ == -1) return;

		// メモリを取得してコピー
		T* bufferPointer = static_cast<T*>(GetBufferShaderConstantBuffer(commonConstantBuffer_));
		if (bufferPointer) {
			*bufferPointer = _constantData;
		}

		// GPUへ転送＆セット
		UpdateShaderConstantBuffer(commonConstantBuffer_);
		SetShaderConstantBuffer(commonConstantBuffer_, _shaderType, _slotIndex);
	}

private:

	// シングルトン用インスタンス
	static ShaderManager* instance_;

	std::unique_ptr<ShaderNormal> shaderNormal_;
	std::unique_ptr<ShaderWave>   shaderWave_;

	// スクリーンのハンドル
	int screenHandle_;

	// バッファ変数
	int commonConstantBuffer_;

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

