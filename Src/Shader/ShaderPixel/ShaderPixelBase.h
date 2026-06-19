#pragma once
#include "../ShaderBase.h"

/// @brief ピクセルシェーダの基底クラス
class ShaderPixelBase : public ShaderBase
{
public:
    ShaderPixelBase(void);
    virtual ~ShaderPixelBase(void);

    virtual void Initialize(const char* shaderPath) override;

    /// @brief 定数バッファの値を更新してGPUに送る（テンプレート版を使用）
    /// @tparam T 送信する構造体の型
    /// @param constantData 定数バッファに書き込むデータ構造体
    template <typename T>
    void UpdateConstantBuffer(const T& constantData)
    {
        // 親クラス（ShaderBase）の汎用テンプレートメソッドを呼び出す
        // ピクセルシェーダーはスロット4から使うルールを継承
        ShaderBase::UpdateConstantBuffer(constantData, DX_SHADERTYPE_PIXEL, CONSTANT_BUFFER_SLOT_BEGIN_PIXEL);
    }

private:
    // ピクセルシェーダ用オリジナル定数バッファの使用開始スロット
    static constexpr int CONSTANT_BUFFER_SLOT_BEGIN_PIXEL = 4;
};