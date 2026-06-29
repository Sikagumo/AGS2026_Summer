#pragma once

#include <DxLib.h>

class ShaderBase
{
public:

	ShaderBase(void);

    virtual ~ShaderBase(void) = default;

	virtual void Initialize(const char* _shaderPath) = 0;
	
	virtual void Release(void);

    int GetShaderHandle() const { return shaderHandle_; }

protected:

	// シェーダーのハンドル
	int shaderHandle_;
};