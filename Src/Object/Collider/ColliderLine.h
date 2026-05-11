#pragma once

#include <DxLib.h>

#include "ColliderBase.h"

class Transform;

/// @brief 線分コライダクラス
class ColliderLine : public ColliderBase
{
public:

	/// @brief コンストラクタ
	/// @param collisionTag 衝突種別
	/// @param followTarget 追従対象のTransform
	/// @param localStartPos ローカル空間での開始位置
	/// @param localEndPos ローカル空間での終了位置
	ColliderLine(TAG  collisionTag, const Transform* followTarget, const VECTOR& localStartPos, const VECTOR& localEndPos);

	/// @brief デストラクタ
	~ColliderLine(void) override = default;

	/// @brief ローカル開始位置の設定
	/// @param position ローカル空間での開始位置
	void SetLocalStartPos(const VECTOR& position);

	/// @brief ローカル終了位置の設定
	/// @param position ローカル空間での終了位置
	void SetLocalEndPos(const VECTOR& position);

	/// @brief ローカル開始位置の取得
	/// @return ローカル空間での開始位置
	const VECTOR& GetLocalStartPos(void) const;

	/// @brief ローカル終了位置の取得
	/// @return ローカル空間での終了位置
	const VECTOR& GetLocalEndPos(void) const;

	/// @brief ワールド開始位置の取得
	/// @return ワールド空間での開始位置
	VECTOR GetWorldStartPos(void) const;

	/// @brief ワールド終了位置の取得
	/// @return ワールド空間での終了位置
	VECTOR GetWorldEndPos(void) const;

protected:

	/// @brief デバッグ描画
	/// @param debugColor 描画色
	void DrawDebug(int debugColor) const override;

private:

	// デバック表示の球体半径
	static constexpr float DEBUG_POINT_RADIUS = 5.0f;

	// デバック表示の球体ポリゴン分割数
	static constexpr int DEBUG_SEGMENT_COUNT = 6;

	// 線分の開始地点座標(ローカル)
	VECTOR localStartPos_;

	// 線分の終了座標(ローカル)
	VECTOR localEndPos_;

};
