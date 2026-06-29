#pragma once

struct alignas(16) IntegratedGpuBuffer
{
    float lightX = 1.0f;
    float lightY = 1.0f;
    float lightZ = 1.0f;
    float ambient = 0.0f;

    float time = 0.0f;       // 波用（使わない時は 0.0f）
    float waveSpeed = 0.0f;  // 波用（使わない時は 0.0f）
    float waveForce = 0.0f;  // 波用（使わない時は 0.0f）
    float useNormal = 1.0f;  // 立体感フラグ
};