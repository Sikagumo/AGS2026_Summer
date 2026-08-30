#pragma once

#include <DxLib.h>

/// @brief ノーマルマップ・波・ライティングシェーダ用パラメータ
struct alignas(16) NormalWaveShaderParams
{
    // 1ブロック目（16バイト）：ライティング情報
    float lightVectorX = 0.5f;
    float lightVectorY = 0.5f;
    float lightVectorZ = 0.5f;
    float ambientRate = 0.1f;

    // 2ブロック目（16バイト）：波・制御フラグ情報
    float timeValue = 0.0f;
    float waveSpeedValue = 1.0f;
    float waveForceValue = 0.01f;
    float useNormalMapFlag = 1.0f;
};

/// @brief 雨エフェクトシェーダ用パラメータ
struct alignas(16) RainShaderParams
{
    // 画面解像度 (px)
    float resolutionX = 0.0f;
    float resolutionY = 0.0f;
    float timeValue = 0.0f;
    float padding1 = 0.0f;

    // 雨の強さ(0.0～1.0)
    float rainIntensity = 0.0f;
    float rainIntensityBack = 0.0f;
    float padding2 = 0.0f;
    float padding3 = 0.0f;

    COLOR_F rainColor = COLOR_F();
};

/// @brief テクスチャスケールシェーダ用パラメータ
struct alignas(16) TextureScaleShaderParams
{
    // スケール
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float padding1 = 0.0f;
    float padding2 = 0.0f;
};