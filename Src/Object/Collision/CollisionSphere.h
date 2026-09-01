#pragma once

#include <DxLib.h>

class ColliderBase;
struct CollisionInfo;

/// @brief 球体の衝突判定を管理するクラス
class CollisionSphere
{
public:
	/// @brief 球体 vs 球体の衝突判定
	/// @param _colliderA 判定対象A（球体）
	/// @param _colliderB 判定対象B（球体）
	/// @param _outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckSphereVsSphere(const ColliderBase* _colliderA, const ColliderBase* _colliderB,
		CollisionInfo& _outInfo);

	/// @brief 球体 vs カプセルの衝突判定
	/// @param _sphereCol 判定対象（球体）
	/// @param _capsuleCol 判定対象（カプセル）
	/// @param _outInfo 衝突時の詳細データを格納する構造体
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
	/// @param _hitCapsuleCol 当たる側のコライダー（カプセル）
	/// @param _waveCol 衝撃波のコライダー（球体）
	/// @return 当たっていれば true
	static bool CheckHitWave(const ColliderBase* _hitCapsuleCol, ColliderBase* _waveCol);

	/// @brief 球とモデルの当たり判定の実処理。中心座標・半径を直接指定
	/// @param _centerPos 判定したい球の中心座標
	/// @param _radius 判定したい球の半径
	/// @param _modelCol 判定対象のモデルコライダー
	/// @param _outInfo 衝突情報の出力先
	/// @return 衝突していればtrue
	static bool CheckSphereVsModelCore(const VECTOR& _centerPos, float _radius,
		const ColliderBase* _modelCol, CollisionInfo* _outInfo);

private:

	// 衝撃波の定数関連
	static constexpr float HIT_WAVE_THICKNESS = 2.0f; // 衝撃波の厚み
	static constexpr float HIT_WAVE_HEIGHT = 75.0f;   // 衝撃波の高さ
};