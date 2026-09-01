#pragma once

#include <unordered_set>
#include <string>

#include "ColliderBase.h"

/// @brief モデルベースのコライダクラス
class ColliderModel : public ColliderBase
{
public:

    /// @brief コンストラクタ
    /// @param _collisionTag 衝突種別
    /// @param _followTarget 追従対象のTransform
    ColliderModel(TAG _collisionTag, const Transform* _followTarget);

    /// @brief デストラクタ
    ~ColliderModel(void) override = default;

    /// @brief モデルハンドルの取得
    /// @return モデルハンドル
    int GetModelHandle(void) const;

    /// @brief 指定文字列を含むフレームを当たり判定から除外
    /// @param _exclusionName 除外対象フレーム名に含まれる文字列
    void AddExcludeFrameIds(const std::string& _exclusionName);

    /// @brief 除外フレームのクリア
    void ClearExcludedFrames(void);

    /// @brief フレームが除外対象か判定
    /// @param _frameIndex フレーム番号
    /// @return 除外対象ならtrue
    bool IsExcludedFrame(int _frameIndex) const;

protected:
    /// @brief デバッグ描画
    /// @param _debugColor 描画色
    void DrawDebug(int _debugColor) const override;

private:
    // 衝突判定から除外するフレーム番号
    std::unordered_set<int> excludedFrameIndices_;
};