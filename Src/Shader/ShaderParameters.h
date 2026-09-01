#pragma once

#include <DxLib.h>

/// @brief ノーマルマップ・波・ライティングシェーダ用パラメータ
struct NormalWaveShaderParams
{
	// ライティング情報
	float lightVectorX = 0.5f;     // ライトベクトルX
	float lightVectorY = 0.5f;     // ライトベクトルY
	float lightVectorZ = 0.5f;     // ライトベクトルZ
	float ambientRate = 0.1f;      // アンビエント係数

	// 波・制御フラグ情報
	float timeValue = 0.0f;        // 経過時間
	float waveSpeedValue = 1.0f;   // 波のスピード
	float waveForceValue = 0.01f;  // 波の強さ
	float useNormalMapFlag = 1.0f; // ノーマルマップ使用フラグ
};

/// @brief 雨エフェクトシェーダ用パラメータ
struct RainShaderParams
{
	// 解像度・時間情報
	float resolutionX = 0.0f;      // 画面解像度X
	float resolutionY = 0.0f;      // 画面解像度Y
	float timeValue = 0.0f;        // 経過時間
	float padding1 = 0.0f;         // パディング1

	// 雨の強度情報
	float rainIntensity = 0.0f;    // 雨の強さ
	float rainIntensityBack = 0.0f;// 背景の雨の強さ
	float padding2 = 0.0f;         // パディング2
	float padding3 = 0.0f;         // パディング3

	// カラー情報
	COLOR_F rainColor = COLOR_F(); // 雨の色
};

/// @brief テクスチャスケールシェーダ用パラメータ
struct TextureScaleShaderParams
{
	// スケール情報
	float scaleX = 1.0f;           // スケールX
	float scaleY = 1.0f;           // スケールY
	float padding1 = 0.0f;         // パディング1
	float padding2 = 0.0f;         // パディング2
};