#include "ColliderModel.h"

ColliderModel::ColliderModel(TAG tag, const Transform* followTarget)
    : ColliderBase(SHAPE::MODEL, tag, followTarget)
{
}

int ColliderModel::GetModelHandle(void) const
{
    if (followTarget_ != nullptr && followTarget_->modelId != -1)
    {
        return followTarget_->modelId;
    }

    return -1;
}

void ColliderModel::AddExcludeFrameIds(const std::string& exclusionName)
{
    // モデル未設定なら処理しない
    if (!followTarget_ || followTarget_->modelId == -1) { return; }

    // モデルのフレーム総数を取得
    int num = MV1GetFrameNum(followTarget_->modelId);

    for (int i = 0; i < num; i++)
    {
        // フレーム名を取得
        const char* frameName = MV1GetFrameName(followTarget_->modelId, i);
        std::string frameNameStr = frameName;

        // 指定文字列を含むフレームか判定
        if (frameNameStr.find(exclusionName) != std::string::npos)
        {
            // 該当フレームを除外リストに登録
            excludedFrameIndices_.insert(i);
        }
    }
}

void ColliderModel::ClearExcludedFrames(void)
{
    // 除外リストを全削除
    excludedFrameIndices_.clear();
}

bool ColliderModel::IsExcludedFrame(int frameIdx) const
{
    // セットに存在するかで判定
    return excludedFrameIndices_.count(frameIdx) > 0;
}

void ColliderModel::DrawDebug(int debugColor) const
{
    int modelHandle = GetModelHandle();
    if (modelHandle == -1) return;

    // 半透明にして描画する
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 1); 

    MV1DrawModel(modelHandle);

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); 
}
