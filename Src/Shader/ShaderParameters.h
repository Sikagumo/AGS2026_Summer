#pragma once
#include <DxLib.h>

struct alignas(16) IntegratedGpuBuffer
{
    // ライティング
    float lightX = 0.5f;
    float lightY = 0.5f;
    float lightZ = 0.5f;
    float ambient = 0.1f;

    // エフェクト
    float time = 0.0f;
    float waveSpeed = 1.0f;
    float waveForce = 0.01f;
    float useNormal = 1.0f;
};

struct alignas(16) IntegratedGpuBufferRain
{
    // 画面解像度 (px)
    float resolutionX = 0.0f;
    float resolutionY = 0.0f;
    float time = 0.0f;
    float dummy1;

    // 雨の強さ(0.0～1.0)
    float intensity = 0.0f;
    float intensityBack = 0.0f;
    float dummy2;
    float dummy3;

    COLOR_F rainColor = COLOR_F();
};

struct DrawRequest
{
    int x = 0;
    int y = 0;
    float scale = 1.0f;
    int textureHandle = -1;
    int normalMapHandle = -1;
    IntegratedGpuBuffer buffer;
    IntegratedGpuBufferRain bufferRain;

    // コンストラクタ
    DrawRequest(void) = default;

    DrawRequest(int _x, int _y, int _textureHandle, float _scale)
        : x(_x)
        , y(_y)
        , scale(_scale)
        , textureHandle(_textureHandle)
        , normalMapHandle(-1)
    {

    }
};