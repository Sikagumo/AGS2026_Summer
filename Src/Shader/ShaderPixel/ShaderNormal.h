#pragma once

#include "ShaderPixelBase.h"

class ShaderNormal : public ShaderPixelBase
{
public:
    ShaderNormal(void);
    ~ShaderNormal(void) override = default;

    void Initialize(const char* _shaderPath) override;

    void SetLightDirection(float _x, float _y, float _z);
    void SetAmbient(float _ambient);

    void Draw(int _x, int _y, int _textureHandle, int _normalMapHandle, float _scale);

    float GetLightX(void)  const { return lightX_; }
    float GetLightY(void)  const { return lightY_; }
    float GetLightZ(void)  const { return lightZ_; }
    float GetAmbient(void) const { return ambient_; }

private:

    float lightX_;
    float lightY_;
    float lightZ_;
    float ambient_;
};