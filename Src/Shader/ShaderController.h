#pragma once

#include <memory>

#include "ShaderParameters.h"
#include "ShaderRenderer.h"
#include "ShaderLibrary.h"
#include "ShaderMaterial.h"

class ShaderController
{
public:

	/// @brief インスタンの生成
	static void CreateInstance(void);

	/// @brief インスタンスを取得 
	/// @return インスタンスの参照
	static ShaderController& GetInstance(void);

	/// @brief インスタンスの破棄
	static void DestroyInstance(void);

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief 描画処理
	/// @param _shaderType シェーダの種類
	/// @param _x X座標
	/// @param _y Y座標
	/// @param _textureHandle 画像ハンドル
	/// @param _scale 拡大率
	/// @param _material 使用するマテリアル
	/// @param _normalMapHandle ノーマルマップ
	void CreateShaderDraw(ShaderLibrary::SHADER_TYPE _shaderType, int _x, int _y, int _textureHandle,
		float _scale, const ShaderMaterial& _material, int _normalMapHandle = -1) const;

	/// @brief 描画処理
	/// @param _x X座標
	/// @param _y Y座標
	/// @param _material 使用するマテリアル
	void CreateShaderDrawRainy(int _x, int _y, const ShaderMaterial& _material) const;

	/// @brief 描画処理
	/// @param _x X座標
	/// @param _y Y座標
	/// @param _textureHandle 画像ハンドル
	/// @param _material 使用するマテリアル
	void CreateShaderDrawTexScale(int _x, int _y, int _modelId, int _textureHandle, const ShaderMaterial& _material) const;

	/// @brief 解放処理 
	void Release(void);

private:

	// 静的メンバ変数
	static ShaderController* instance_;

	// シェーダの描画クラス
	std::unique_ptr<ShaderRenderer> shaderRenderer_;

	// シェーダの管理クラス
	std::unique_ptr<ShaderLibrary> shaderLibrary_;


	/// @brief コンストラクタ 
	ShaderController(void);

	/// @brief デストラクタ
	~ShaderController(void);

	/// @brief コピーコンストラクタを禁止する
	ShaderController(const ShaderController&) = delete;

	/// @brief 代入演算子を禁止する
	ShaderController& operator=(const ShaderController&) = delete;

	/// @brief ムーブコンストラクタを禁止する
	ShaderController(ShaderController&&) = delete;

	/// @brief ムーブ代入演算子を禁止する
	ShaderController& operator=(ShaderController&&) = delete;

};

