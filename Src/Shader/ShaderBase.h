#pragma once

#include <DxLib.h>

class ShaderBase
{
public:

	/// @brief コンストラクタ 
	ShaderBase(void) = default;

    /// @brief デストラクタ 
    virtual ~ShaderBase(void) = default;

	/// @brief シェーダをGPUに適用する 
	virtual void Apply(void) = 0;

	/// @brief シェーダの適用を解除する 
	virtual void UnApply(void) = 0;
};