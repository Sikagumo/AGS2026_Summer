#include "ColliderLine.h"
#include "../Common/Transform.h"

ColliderLine::ColliderLine(TAG collisionTag, const Transform* followTarget, const VECTOR& localStartPos, const VECTOR& localEndPos)
    : ColliderBase(SHAPE::LINE, collisionTag, followTarget)
    , localStartPos_(localStartPos)
    , localEndPos_(localEndPos)
{
}

void ColliderLine::SetLocalStartPos(const VECTOR& position)
{
    localStartPos_ = position;
}

void ColliderLine::SetLocalEndPos(const VECTOR& position)
{
    localEndPos_ = position;
}

const VECTOR& ColliderLine::GetLocalStartPos(void) const
{
    return localStartPos_;
}

const VECTOR& ColliderLine::GetLocalEndPos(void) const
{
    return localEndPos_;
}

VECTOR ColliderLine::GetWorldStartPos(void) const
{
    // ローカル座標 → ワールド座標へ変換（開始点）
    return TransformLocalToWorld(localStartPos_);
}

VECTOR ColliderLine::GetWorldEndPos(void) const
{
    // ローカル座標 → ワールド座標へ変換（終了点）
    return TransformLocalToWorld(localEndPos_);
}

void ColliderLine::DrawDebug(int debugColor) const
{
    const VECTOR start = GetWorldStartPos();
    const VECTOR end = GetWorldEndPos();

    // 線分を描画
    DrawLine3D(start, end, debugColor);

    // 始点・終点を球体で補助表示
    DrawSphere3D(start, DEBUG_POINT_RADIUS, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
    DrawSphere3D(end, DEBUG_POINT_RADIUS, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
}