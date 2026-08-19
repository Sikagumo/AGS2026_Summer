#pragma once

#include <DxLib.h>
#include <unordered_map>
#include <array>
#include "RenderCommand.h"

/// @brief シェーダの描画クラス
class ShaderRenderer
{
public:

    // 定数バッファのスロット関連
    static constexpr int CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER = 7; // 頂点シェーダ用オリジナル定数バッファの使用開始スロット
    static constexpr int CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER = 4;  // ピクセルシェーダ用オリジナル定数バッファの使用開始スロット

    /// @brief コンストラクタ
    ShaderRenderer(void);

    /// @brief デストラクタ 
    ~ShaderRenderer(void);

    /// @brief 初期化 
    void Initialize(void);

    /// @brief 描画命令の実行
    /// @param _renderCommand 実行する描画命令
    void ExecuteCommand(const RenderCommand& _renderCommand);

    /// @brief 解放処理 
    void Release(void);

private:

    /// @brief 定数バッファを更新してシェーダに設定
    /// @param _parameterPointer パラメータデータへのポインタ
    /// @param _parameterSize パラメータのサイズ
    /// @param _shaderType シェーダのタイプ
    /// @param _slotBegin スロットの開始番号
    /// @return 使用した定数バッファのハンドル
    int UpdateAndSetConstantBuffer(const void* _parameterPointer, int _parameterSize, int _shaderType, int _slotBegin);

    /// @brief 2D描画用の頂点データを設定する
    /// @param _vertices 頂点データの配列
    /// @param _width 描画する幅
    /// @param _height 描画する高さ
    void ApplyVertices(std::array<VERTEX2DSHADER, 4>& _vertices, float _width, float _height) const;

    // 定数バッファ関連
    std::unordered_map<int, int> constantBufferMap_; // パラメータサイズをキーとした定数バッファハンドル
};