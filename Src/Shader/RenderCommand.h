#pragma once

#include <vector>

/// @brief 描画命令の種別
enum class RENDER_TYPE
{
	DRAW_2D, // 画像などの板ポリゴン描画
	DRAW_3D  // 3Dモデルの描画
};

/// @brief 描画命令を保持する構造体
struct RenderCommand
{
	// 描画種別関連
	RENDER_TYPE renderType = RENDER_TYPE::DRAW_3D;	// 2Dか3Dかの判別

	// リソース関連
	int vertexShaderHandleId = - 1;					// 頂点シェーダのID（-1で不使用）
	int pixelShaderHandleId = -1;					// ピクセルシェーダのID
	int modelHandleId = -1;							// 3Dモデルのハンドル

	// 2D描画座標関連
	float positionX = 0.0f;							// 描画X座標
	float positionY = 0.0f;							// 描画Y座標
	float scaleSize = 0.0f;							// 描画スケール

	// テクスチャ関連
	int textureHandleId = -1;						// メインテクスチャのハンドル
	int normalMapHandleId = -1;			            // ノーマルマップのハンドル

	// パラメータ関連
	std::vector<unsigned char> vertexParameterData;           // 頂点シェーダ用定数バッファのデータ
	int vertexParameterSize = 0;                              // 頂点シェーダ用パラメータのサイズ
	std::vector<unsigned char> pixelParameterData;            // ピクセルシェーダ用定数バッファのデータ
	int pixelParameterSize = 0;                               // ピクセルシェーダ用パラメータのサイズ

	// クランプするかどうかのフラグ
	bool isClamp = false;                                     
};