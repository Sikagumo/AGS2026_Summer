#pragma once
#include "Collider2DBase.h"
#include <DxLib.h>
#include "../../Common/Vector2.h"

/// @brief 2D円コライダーのクラス
class Collider2DCircle : public Collider2DBase
{
public:

	/// @brief コンストラクタ（座標直接指定）
	/// @param _centerPos 初期の中央座標
	/// @param _radius 円の半径
	/// @param _collisionTag 衝突の2D識別タグ
	Collider2DCircle(const Vector2F& _centerPos, float _radius, Collider2DBase::TAG_2D _collisionTag);

	/// @brief デストラクタ
	virtual ~Collider2DCircle(void) override = default;

	/// @brief 中心座標を直接変更する
	/// @param _centerPos 新しい中心座標
	void SetCenterPos(const Vector2F& _centerPos);

	/// @brief 半径の変更
	/// @param _radius 新しい半径
	void SetRadius(float _radius);

	/// @brief 中央座標を取得
	/// @return 中央座標
	Vector2F GetWorldCenterPos(void) const;

	/// @brief 半径の取得
	/// @return 半径
	float GetRadius(void) const;

protected:

	/// @brief デバッグ描画
	/// @param _color 描画色
	virtual void DrawDebug(int _color) const override;

private:

	// 円の設定関連
	float radius_;            // 円の半径

	// 座標関連
	Vector2F centerPos_;      // 中央座標
};