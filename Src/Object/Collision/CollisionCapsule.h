#pragma once

class ColliderBase;
struct CollisionInfo;

class CollisionCapsule
{
public:
	/// @brief カプセル vs カプセルの衝突判定
	/// @param colliderA 判定対象A（カプセル）
	/// @param colliderB 判定対象B（カプセル）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckCapsuleVsCapsule(const ColliderBase* _colliderA, const ColliderBase* _colliderB,
		CollisionInfo& _outInfo);

	/// @brief カプセル vs 球体の衝突判定
	/// @param _capsuleCol 判定対象（カプセル）
	/// @param _sphereCol 判定対象（球体）
	/// @param _outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckCapsuleVsSphere(const ColliderBase* _capsuleCol, const ColliderBase* _sphereCol,
		CollisionInfo& _outInfo);

	/// @brief カプセル vs モデル（地形）の衝突判定
	/// @param capsuleCol 判定対象（カプセル）
	/// @param modelCol 判定対象（3Dモデル/メッシュ）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckCapsuleVsModel(const ColliderBase* _capsuleCol, const ColliderBase* _modelCol,
		CollisionInfo& _outInfo);
};

