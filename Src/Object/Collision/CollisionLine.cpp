#include "CollisionLine.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "../Collision/CollisionController.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// マジックナンバー排除用の定数
	constexpr int INVALID_HANDLE = -1; // 無効なモデルハンドルを示す値
	constexpr int HIT_FLAG_TRUE = 1;   // 衝突判定関数が真（ヒットした）を返す値
}

bool CollisionLine::CheckLineVsModel(const ColliderBase* _lineCol, const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_lineCol || !_modelCol)
	{
		return false;
	}

	const auto* line = static_cast<const ColliderLine*>(_lineCol);
	const auto* model = static_cast<const ColliderModel*>(_modelCol);

	if (!line || !model)
	{
		return false;
	}

	const int modelHandle = model->GetModelHandle();
	if (modelHandle == INVALID_HANDLE)
	{
		return false;
	}

	const VECTOR startPos = line->GetWorldStartPos();
	const VECTOR endPos = line->GetWorldEndPos();

	// 線分とモデルの衝突判定
	const MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(modelHandle, INVALID_HANDLE, startPos, endPos);

	if (hitResult.HitFlag == HIT_FLAG_TRUE)
	{
		// 判定から除外するフレームに当たった場合は無視する
		if (model->IsExcludedFrame(hitResult.FrameIndex))
		{
			return false;
		}

		_outInfo.myCollider = _lineCol;
		_outInfo.hitCollider = _modelCol;
		_outInfo.isActive = true;

		_outInfo.hitPosition = hitResult.HitPosition;
		_outInfo.hitNormal = hitResult.Normal;
		_outInfo.penetration = UtilityMath::MagnitudeF(VSub(endPos, hitResult.HitPosition));

		return true;
	}

	return false;
}