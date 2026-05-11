#pragma once

#include <unordered_set>
#include <string>

#include "ColliderBase.h"

/// @brief モデルベースのコライダクラス（フレーム単位で当たり判定を制御）
class ColliderModel : public ColliderBase
{
public:

    /// @brief コンストラクタ
    /// @param collisionTag 衝突種別
    /// @param followTarget 追従対象のTransform
    ColliderModel(TAG collisionTag, const Transform* followTarget);

    /// @brief デストラクタ
    ~ColliderModel(void) override = default;

    /// @brief 指定文字列を含むフレームを当たり判定から除外
    /// @param exclusionName 除外対象フレーム名に含まれる文字列
    void AddExcludeFrameIds(const std::string& exclusionName);

    /// @brief 除外フレームのクリア
    void ClearExcludedFrames(void);

    /// @brief フレームが除外対象か判定
    /// @param frameIndex フレーム番号
    /// @return 除外対象ならtrue
    bool IsExcludedFrame(int frameIndex) const;

protected:
    /// @brief デバッグ描画（モデルコライダは描画しない）
    void DrawDebug(int debugColor) const override {};

private:
    // 衝突判定から除外するフレーム番号
    std::unordered_set<int> excludedFrameIndices_;
};