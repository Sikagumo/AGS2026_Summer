#pragma once

#include <DxLib.h>

#include "ColliderBase.h"

class Transform;

// @brief 球体コライダクラス
class ColliderSphere : public ColliderBase
{
public:

    // @brief コンストラクタ
    // @param collisionTag 衝突種別
    // @param followTarget 追従対象のTransform
    // @param localPosition ローカル空間での中心位置
    // @param radius 球体の半径
    ColliderSphere(TAG collisionTag, const Transform* followTarget, const VECTOR& localPosition, float radius);

    // @brief デストラクタ
    ~ColliderSphere(void) override = default;

    // @brief ローカル座標の設定
    // @param position ローカル空間での中心位置
    void SetLocalPosition(const VECTOR& position);

    // @brief 半径の設定
    // @param radius 球体の半径
    void SetRadius(float radius);

    // @brief ローカル座標の取得
    // @return ローカル空間での中心位置
    const VECTOR& GetLocalPosition(void) const;

    // @brief ワールド座標の取得
    // @return ワールド空間での中心位置
    VECTOR GetWorldPosition(void) const;

    // @brief 半径の取得
    // @return 球体の半径
    float GetRadius(void) const;

protected:

    // @brief デバッグ描画
    // @param color 描画色
    void DrawDebug(int color) const override;

private:

    // デバッグ表示の球体ポリゴン分割数
    static constexpr int DEBUG_SEGMENT_COUNT = 16;

    // 球体の中心座標(ローカル)
    VECTOR localPosition_;

    // 球体の半径
    float radius_;
};