#pragma once

#include <array>
#include <DxLib.h>

#include "../ShaderBase.h"


/// @brief ピクセルシェーダの基底クラス
class ShaderPixelBase : public ShaderBase
{
public:
    ShaderPixelBase(void);
    virtual ~ShaderPixelBase(void);

    virtual void Initialize(const char* shaderPath) override;

    void InitBaseVertexes(float w, float h);

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

    int GetShaderHandle(void) const { return shaderHandle_; }

protected:

    std::array<VERTEX2DSHADER, 4> vertices_;

private:
    // ピクセルシェーダ用オリジナル定数バッファの使用開始スロット
    static constexpr int CONSTANT_BUFFER_SLOT_BEGIN_PIXEL = 4;
};