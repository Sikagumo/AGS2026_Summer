#pragma once

#include "ShaderPixelBase.h"

class ShaderWave : public ShaderPixelBase
{
public:
    ShaderWave(void);
    ~ShaderWave(void) override = default;

    void Initialize(const char* _shaderPath) override;

    void SetWaveParam(float _time, float _speed, float _force);

    void Draw(int _x, int _y, int _textureHandle, float _scale = 1.0f);

    float GetTime(void)  const { return time_; }
    float GetSpeed(void) const { return speed_; }
    float GetForce(void) const { return force_; }

private:
    float time_;
    float speed_;
    float force_;
};