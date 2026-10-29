#pragma once

#include <DxLib.h>
#include "ColliderBase.h"
#include <algorithm>

class Transform;

/// @brief 球体コライダクラス
class ColliderSphere : public ColliderBase
{
public:

	/// @brief コンストラクタ
	/// @param _collisionTag 衝突種別
	/// @param _followTarget 追従対象のTransform
	/// @param _localPos ローカル空間での中心位置
	/// @param _radius 球体の半径
	ColliderSphere(TAG _collisionTag, const Transform* _followTarget, const VECTOR& _localPos, float _radius);

	/// @brief デストラクタ
	~ColliderSphere(void) override = default;

	/// @brief ローカル座標の設定
	/// @param _position ローカル空間での中心位置
	void SetLocalPosition(const VECTOR& _position) { localPos_ = _position; }

	/// @brief 半径の設定（負にならないように制限）
	/// @param _radius 球体の半径
	void SetRadius(float _radius);

	/// @brief ローカル座標の取得
	/// @return ローカル空間での中心位置
	const VECTOR& GetLocalPosition(void) const { return localPos_; }

	/// @brief ワールド座標の取得
	/// @return ワールド空間での中心位置
	VECTOR GetWorldPosition(void) const;

	/// @brief 半径の取得
	/// @return 球体の半径
	float GetRadius(void) const { return radius_; }

protected:

	/// @brief デバッグ描画
	/// @param _debugColor 描画色
	void DrawDebug(int _debugColor) const override;

private:

	// デバッグ表示の球体ポリゴン分割数
	static constexpr int DEBUG_SEGMENT_COUNT = 16;

	// コライダーの形状パラメータ関連
	VECTOR localPos_; // 球体の中心座標
	float radius_;    // 球体の半径
};