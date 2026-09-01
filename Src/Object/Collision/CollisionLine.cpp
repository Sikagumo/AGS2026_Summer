#include "CollisionLine.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "../Collision/CollisionController.h"
#include "../../Utility/UtilityMath.h"

bool CollisionLine::CheckLineVsModel(const ColliderBase* _lineCol,
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_lineCol || !_modelCol)
	{
		return false;
	}

	const auto* LINE = static_cast<const ColliderLine*>(_lineCol);    
	const auto* MODEL = static_cast<const ColliderModel*>(_modelCol); 

	if (!LINE || !MODEL)
	{
		return false;
	}

	const int MODEL_HANDLE = MODEL->GetModelHandle(); 
	if (MODEL_HANDLE == -1)
	{
		return false;
	}

	const VECTOR START_POS = LINE->GetWorldStartPos(); 
	const VECTOR END_POS = LINE->GetWorldEndPos();     

	// 線分とモデルの衝突判定
	const MV1_COLL_RESULT_POLY HIT_RESULT = MV1CollCheck_Line(MODEL_HANDLE, -1, START_POS, END_POS);

	if (HIT_RESULT.HitFlag == 1)
	{
		// 判定から除外するフレームに当たった場合は無視する
		if (MODEL->IsExcludedFrame(HIT_RESULT.FrameIndex))
		{
			return false;
		}

		_outInfo.myCollider = _lineCol;
		_outInfo.hitCollider = _modelCol;
		_outInfo.isActive = true;

		_outInfo.hitPosition = HIT_RESULT.HitPosition;
		_outInfo.hitNormal = HIT_RESULT.Normal;
		_outInfo.penetration = UtilityMath::MagnitudeF(VSub(END_POS, HIT_RESULT.HitPosition));

		return true;
	}

	return false;
}