#pragma once

#include <DxLib.h>

class ShaderBase
{
public:

	ShaderBase(void);

	virtual void Initialize(const char* shaderPath) = 0;
	
	virtual void Release(void);

protected:

	// シェーダーのハンドル
	int shaderHandle_;

	// 定数バッファのハンドル
	int constantBuffer_;

    /// @brief 定数バッファの値を更新してGPUに送る
    /// @param T 送信する構造体の型
    /// @param constantData 定数バッファに書き込むデータ構造体
    /// @param shaderType DxLibのシェーダータイプ（DX_SHADERTYPE_PIXELなど）
    /// @param slotIndex 定数バッファのスロット番号
    template <typename T>
    void UpdateConstantBuffer(const T& _constantData, int _shaderType, int _slotIndex)
    {
        if (constantBuffer_ == -1) return;

        // バッファのメモリ領域を直接取得してコピー
        T* bufferPointer = static_cast<T*>(GetBufferShaderConstantBuffer(constantBuffer_));
        *bufferPointer = _constantData;

        // GPUに転送してスロットにバインド
        UpdateShaderConstantBuffer(constantBuffer_);
        SetShaderConstantBuffer(constantBuffer_, _shaderType, _slotIndex);
    }
};