#pragma once

#include <DxLib.h>

#include "ColliderBase.h"

class Transform;

/// @brief カプセル形状のコライダクラス
class ColliderCapsule : public ColliderBase
{
public:

    /// @brief コンストラクタ
    /// @param collisionTag 衝突種別（プレイヤー・敵など）
    /// @param followTarget 追従対象のTransform
    /// @param localStartPos ローカル空間での開始位置
    /// @param localEndPos ローカル空間での終了位置
    /// @param radius カプセルの半径
    ColliderCapsule(TAG _collisionTag, const Transform* _followTarget, const VECTOR& _localStartPos, const VECTOR& _localEndPos, float _radius);

    /// @brief デストラクタ
    ~ColliderCapsule(void) override = default;

    /// @brief ローカル開始位置の設定
    /// @param pos ローカル空間での開始位置
    void SetLocalStartPos(const VECTOR& _position);

    /// @brief ローカル終了位置の設定
    /// @param pos ローカル空間での終了位置
    void SetLocalEndPos(const VECTOR& _position);

    /// @brief 半径の設定
    /// @param radius カプセルの半径
    void SetRadius(float _radius) override;

    /// @brief ローカル開始位置の取得
    /// @return ローカル空間での開始位置
    const VECTOR& GetLocalStartPos(void) const;

    /// @brief ローカル終了位置の取得
    /// @return ローカル空間での終了位置
    const VECTOR& GetLocalEndPos(void) const;

    /// @brief ワールド開始位置の取得
    /// @return ワールド空間での開始位置
    VECTOR GetWorldStartPos(void) const;

    /// @brief ワールド開始位置の取得
    /// @return ワールド空間での開始位置
    VECTOR GetWorldEndPos(void) const;

    /// @brief 半径の取得
   /// @return カプセルの半径
    float GetRadius(void) const;

    /// @brief 始点、終点、半径をまとめて設定する
    /// @param _localStartPos 新しい始点 (ローカル座標)
    /// @param _localEndPos 新しい終点 (ローカル座標)
    /// @param _radius 新しい半径
    void SetShape(const VECTOR& _localStartPos, const VECTOR& _localEndPos, float _radius);

protected:
    /// @brief デバッグ描画
    /// @param debugColor 描画色
    void DrawDebug(int debugColor) const override;

private:
    // デバッグ表示の球体・カプセルポリゴン分割数
    static constexpr int DEBUG_SEGMENT_COUNT = 8;

    // カプセルの開始座標(ローカル)
    VECTOR localStartPos_;

    // カプセルの終了座標(ローカル)
    VECTOR localEndPos_;

    // カプセルの半径
    float radius_;
};