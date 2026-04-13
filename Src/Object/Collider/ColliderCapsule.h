#pragma once
#include <DxLib.h>
#include "ColliderBase.h"
class Transform;
class ColliderModel;

class ColliderCapsule : public ColliderBase
{
public:

	// コンストラクタ
	ColliderCapsule(TAG tag, const Transform* follow,
					const VECTOR& localPosTop, const VECTOR& localPosDown, float radius);

	// デストラクタ
	~ColliderCapsule(void) = default;


	// 親Transformからの相対位置を取得
	const VECTOR& GetLocalPosTop(void) const { return localPosTop_; };
	const VECTOR& GetLocalPosDown(void) const { return localPosDown_; };

	// 親Transformからの相対位置をセット
	void SetLocalPosTop(const VECTOR& pos) { localPosTop_ = pos; };
	void SetLocalPosDown(const VECTOR& pos) { localPosDown_ = pos; };

	// ワールド座標を取得
	VECTOR GetPosTop(void) const;
	VECTOR GetPosDown(void) const;

	// 半径
	float GetRadius(void) const { return radius_; };
	void SetRadius(float _radius) { radius_ = _radius; };

	// 高さ
	float GetHeight(void) const { return localPosTop_.y; };

	// カプセルの中心座標
	VECTOR GetCenter(void) const;

	// 指定された回数と距離で三角形の法線方向に押し戻した座標を取得
	VECTOR GetPosPushBackAlongNormal(
		const MV1_COLL_RESULT_POLY& hitColPoly,
		int maxTryCnt,
		float pushDistance) const;

	// 指定された回数と距離で三角形の法線方向に押し戻す
	void PushBackAlongNormal(
		const ColliderModel* _colliderModel, Transform* _transform,
		int _maxTryCnt, float _pushDistance,
		bool isExclude = false, bool isTarget = false) const;
	
	// モデルと衝突しているか否か
	bool IsHit(const ColliderModel* colliderModel,
				bool isExcude = false, bool isTarget = false) const;

protected:

	// デバッグ用描画
	void DrawDebug(int color) override;

private:

	// 親Transformからの相対位置(上側)
	VECTOR localPosTop_;

	// 親Transformからの相対位置(下側)
	VECTOR localPosDown_;

	// 半径
	float radius_;
};
