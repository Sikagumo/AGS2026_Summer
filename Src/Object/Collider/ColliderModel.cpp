#include "ColliderModel.h"

namespace
{
	// 無効なモデルID
	constexpr int INVALID_MODEL_ID = -1; 

	// ループの初期インデックス
	constexpr int INITIAL_LOOP_INDEX = 0; 
}

ColliderModel::ColliderModel(TAG _collisionTag, const Transform* _followTarget)
	: ColliderBase(SHAPE::MODEL, _collisionTag, _followTarget)
{
}

int ColliderModel::GetModelHandle(void) const
{
	// 早期リターン
	if (followTarget_ == nullptr || followTarget_->modelId == INVALID_MODEL_ID)
	{
		return INVALID_MODEL_ID;
	}

	return followTarget_->modelId;
}

void ColliderModel::AddExcludeFrameIds(const std::string& _exclusionName)
{
	// モデル未設定なら早期リターン
	if (followTarget_ == nullptr || followTarget_->modelId == INVALID_MODEL_ID)
	{
		return;
	}

	// モデルのフレーム総数を取得
	int frameNum = MV1GetFrameNum(followTarget_->modelId);

	for (int i = INITIAL_LOOP_INDEX; i < frameNum; i++)
	{
		// フレーム名を取得
		const char* frameName = MV1GetFrameName(followTarget_->modelId, i);
		std::string frameNameStr = frameName;

		// 指定文字列を含むフレームか判定
		if (frameNameStr.find(_exclusionName) != std::string::npos)
		{
			// 該当フレームを除外リストに登録
			excludedFrameIndices_.insert(i);
		}
	}
}

void ColliderModel::DrawDebug(int _debugColor) const
{
	int modelHandle = GetModelHandle();

	// モデルが無効なら早期リターン
	if (modelHandle == INVALID_MODEL_ID)
	{
		return;
	}

	constexpr int ALPHA_BLEND_PARAM = 1; // 半透明のブレンドパラメータ
	constexpr int NO_BLEND_PARAM = 0;    // ブレンドなしのパラメータ

	// 半透明にして描画する
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALPHA_BLEND_PARAM);

	MV1DrawModel(modelHandle);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NO_BLEND_PARAM);
}