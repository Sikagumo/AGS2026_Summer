#pragma once

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

struct DrawRequest
{
    int x = 0;
    int y = 0;
    float scale = 1.0f;
    int textureHandle = -1;
    int normalMapHandle = -1;
    IntegratedGpuBuffer buffer;

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