#pragma once

class Transform;

/// @brief 2Dコライダーの共通親クラス
class Collider2DBase
{
	/// @brief 2D形状の種別
	enum class SHAPE_2D
	{
		CIRCLE,  // 円
		BOX,     // 矩形
	};

	/// @brief 衝突の識別タグ
	enum  class TAG_2D
	{
		UI_BUTTON,
		MOUSE_CURSOR,
	};

	/// @brief コンストラクタ
	/// @param _shapeType 2D形状の種類
	/// @param _collisionTag 衝突の2D識別タグ
	/// @param _followTarget 追従対象のTransform
	Collider2DBase(SHAPE_2D _shapeType, TAG_2D _collisionTag, const Transform* _followTarget);

	/// @brief デストラクタ
	virtual ~Collider2DBase(void) = default;

	/// @brief デバック描画
	/// @param _color 描画色
	virtual void DrawDebug(int _color) const = 0;

	/// @brief 2D形状タイプの取得
	/// @return 2D形状タイプ
	SHAPE_2D GetShapeType(void) const;

	/// @brief 2Dタグの取得 
	/// @return 衝突2Dタグ
	TAG_2D GetCollisionTag(void) const;

private:

	// 2D形状の種別
	SHAPE_2D shapeType_;

	// 衝突の2D識別タグ
	TAG_2D collisionTag_;

	// 追従対象のTransform



};

