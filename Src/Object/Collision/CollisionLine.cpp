#include "CollisionLine.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "../Collision/CollisionController.h"
#include "../../Utility/UtilityMath.h"

bool CollisionLine::CheckLineVsModel(const ColliderBase* _lineCol,
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_lineCol || !_modelCol) { return false; }

	const auto* line =  static_cast<const ColliderLine*>(_lineCol);
	const auto* model =  static_cast<const ColliderModel*>(_modelCol);

	if (!line || !model) { return false; }

	int modelHandle = model->GetModelHandle();
	if (modelHandle == -1) { return false; }

	VECTOR startPos = line->GetWorldStartPos();
	VECTOR endPos = line->GetWorldEndPos();

	MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(modelHandle, -1, startPos, endPos);

	if (hitResult.HitFlag == 1)
	{
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