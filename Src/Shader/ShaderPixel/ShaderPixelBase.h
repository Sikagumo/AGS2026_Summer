#pragma once

#include <array>
#include <DxLib.h>

#include "../ShaderBase.h"


/// @brief ピクセルシェーダの基底クラス
class ShaderPixelBase : public ShaderBase
{
public:
    ShaderPixelBase(void);
    
    ~ShaderPixelBase(void) override;

    virtual void Initialize(const char* _shaderPath) override;

    void InitBaseVertexes(float _width, float _hight);

protected:

    std::array<VERTEX2DSHADER, 4> vertices_;
};