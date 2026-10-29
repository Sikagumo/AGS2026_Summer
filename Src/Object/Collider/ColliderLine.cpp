#include "ColliderLine.h"
#include "../Common/Transform.h"

ColliderLine::ColliderLine(TAG _collisionTag, const Transform* _followTarget,
	const VECTOR& _localStartPos, const VECTOR& _localEndPos)
	: ColliderBase(SHAPE::LINE, _collisionTag, _followTarget)
	, localStartPos_(_localStartPos)
	, localEndPos_(_localEndPos)
{
}

VECTOR ColliderLine::GetWorldStartPos(void) const
{
	// ローカル座標 → ワールド座標へ変換
	return TransformLocalToWorld(localStartPos_);
}

VECTOR ColliderLine::GetWorldEndPos(void) const
{
	// ローカル座標 → ワールド座標へ変換
	return TransformLocalToWorld(localEndPos_);
}

void ColliderLine::DrawDebug(int _debugColor) const
{
	const VECTOR WORLD_START_POS = GetWorldStartPos();
	const VECTOR WORLD_END_POS = GetWorldEndPos();

	// 線分を描画
	DrawLine3D(WORLD_START_POS, WORLD_END_POS, _debugColor);

	// 開始点と終了点に分かりやすく球体を描画
	DrawSphere3D(WORLD_START_POS, DEBUG_POINT_RADIUS, DEBUG_SEGMENT_COUNT, _debugColor, _debugColor, false);
	DrawSphere3D(WORLD_END_POS, DEBUG_POINT_RADIUS, DEBUG_SEGMENT_COUNT, _debugColor, _debugColor, false);
}