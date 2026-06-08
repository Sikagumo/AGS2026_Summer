#pragma once

class ColliderBase;
struct CollisionInfo;

class CollisionSphere
{
public:
	/// @brief 球体 vs 球体の衝突判定
	/// @param colliderA 判定対象A（球体）
	/// @param colliderB 判定対象B（球体）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckSphereVsSphere(const ColliderBase* _colliderA, const ColliderBase* _colliderB,
		CollisionInfo& _outInfo);

	/// @brief 球体 vs カプセルの衝突判定
	/// @param sphereCol 判定対象（球体）
	/// @param capsuleCol 判定対象（カプセル）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckSphereVsCapsule(const ColliderBase* _sphereCol, const ColliderBase* _capsuleCol,
		CollisionInfo& _outInfo);

	/// @brief 球体 vs モデルの衝突判定
	/// @param _sphereCol 判定対象 (球体)
	/// @param _modelCol 判定対象 (3Dモデル/メッシュ)
	/// @param _outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckSphereVsModel(const ColliderBase* _sphereCol, const ColliderBase* _modelCol,
		CollisionInfo& _outInfo);

	/// @brief 衝撃波の当たり判定を行う
	/// @param _hitCol 当たる側のコライダー
	/// @param _waveCol 衝撃波のコライダー
	/// @param _waveThickness 衝撃波の厚み
	/// @param _waveHeight 衝撃波の高さ
	/// @return 当たっていれば true
	static bool CheckHitWave(const ColliderBase* _hitCapsuleCol, ColliderBase* _waveCol);

private:

	// 衝撃波の定数関連
	static constexpr float HIT_WAVE_THICKNESS = 2.0f; // 衝撃波の厚み
	static constexpr float HIT_WAVE_HEIGHT = 75.0f;    // 衝撃波の高さ
};

