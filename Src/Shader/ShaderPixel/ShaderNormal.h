#pragma once

#include "ShaderPixelBase.h"

class ShaderNormal : public ShaderPixelBase
{
public:

    struct LightingParameter
    {
        float lightDirection[3] = { 0.0f, 0.0f, 1.0f };
        float specularShininess = 32.0f;
        float ambientIntensity = 0.2f;
        float padding[3] = { 0.0f, 0.0f, 0.0f };
    };

    ShaderNormal(void);

    ~ShaderNormal(void);
    
    void Initialize(const char* _shaderPath) override;

    // ï`âÊä÷êî
    void Draw(int _x, int _y, int _textureHandle, int _normalMapHandle, const LightingParameter& _param);
};

