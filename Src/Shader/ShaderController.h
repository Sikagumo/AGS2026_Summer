#pragma once

#include <memory>

#include "ShaderParameters.h"
#include "ShaderRenderer.h"
#include "ShaderLibrary.h"

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
	/// @param _request 描画のリクエスト情報 
	void Draw(SHADER_TYPE _shaderType, const DrawRequest& _request) const;

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

