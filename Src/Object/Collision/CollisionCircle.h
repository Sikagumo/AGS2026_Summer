#pragma once

class Collider2DBase;

class CollisionCircle
{
public:
	/// @brief 円と円の衝突判定
	/// @param _circleA 1つ目の円コライダー
	/// @param _circleB 2つ目の円コライダー
	/// @return 衝突しているか
	static bool CheckCircleVsCircle(const Collider2DBase* _circleA,
		const Collider2DBase* _circleB);

	/// @brief 円と矩形の衝突判定
	/// @param _circle 円コライダー
	/// @param _box 矩形コライダー
	/// @return 衝突しているか
	static bool CheckCircleVsBox(const Collider2DBase* _circle, const Collider2DBase* _box);
};

