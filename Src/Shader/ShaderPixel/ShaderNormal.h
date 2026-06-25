#pragma once

#include "ShaderPixelBase.h"

class ShaderNormal : public ShaderPixelBase
{
public:


    ShaderNormal(void);

    ~ShaderNormal(void);
    
    void Initialize(const char* _shaderPath) override;

    void SetLightDirection(float _x, float _y, float _z);

    // ï`âÊä÷êî
    void Draw(int _x, int _y, int _textureHandle, int _normalMapHandle, float _scale);

private:

    struct alignas(16) LightingParameter
    {
        float lightX = 0.6f;
        float lightY = 0.6f;
        float lightZ = 1.0f;
        float padding = 0.0f; 
    };

    LightingParameter param_;
};

