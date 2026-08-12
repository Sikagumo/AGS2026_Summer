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
    float dummy1 = 0.0f;

    // 雨の強さ(0.0～1.0)
    float intensity = 0.0f;
    float intensityBack = 0.0f;
    float dummy2 = 0.0f;;
    float dummy3 = 0.0f;;

    COLOR_F rainColor = COLOR_F();
};

struct alignas(16) IntegratedGpuBufferTexScale
{
    // スケール
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float dummy1 = 0.0f;
    float dummy2 = 0.0f;
};

struct DrawRequest
{
    int x = 0;
    int y = 0;
    float scale = 1.0f;
    int textureHandle = -1;
    int normalMapHandle = -1;
    int modelId = -1;
    IntegratedGpuBuffer buffer;
    IntegratedGpuBufferRain bufferRain;
    IntegratedGpuBufferTexScale bufferTexScale;

    // コンストラクタ
    DrawRequest(void) = default;

    DrawRequest(int _x, int _y, int _textureHandle, float _scale, int _modelId = -1)
        : x(_x)
        , y(_y)
        , scale(_scale)
        , textureHandle(_textureHandle)
        , normalMapHandle(-1)
        , modelId(_modelId)
    {

    }
};