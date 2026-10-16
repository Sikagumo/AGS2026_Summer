#pragma once
#include <memory>
#include <vector>
#include "MGBase.h"

class BBulletBase;


class WeaponMGL : public MGBase
{
public:

	WeaponMGL(void);
	~WeaponMGL(void) override = default;

	/// @brief 武器モデル・SE等のリソース読み込み処理
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

protected:

	/// @brief トランスフォーム（大きさ・回転・座標）の初期化
	void InitTransform(void) override;

	/// @brief 当たり判定（コライダー）の初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理（ステート設定等）
	void InitPost(void) override;

	/// @brief フレーム毎の更新処理
	void UpdateProcess(void) override;

	/// @brief 更新後の個別処理
	void UpdateProcessPost(void) override;

	/// @brief 描画前処理（発射中の弾の描画など）
	void DrawPre(void) override;

	/// @brief 衝突予約処理
	void CollisionReserve(void) override {};

private:

	static constexpr VECTOR LINE_START_POS = { -50.0f, 0.0f, 50.0f };		// 線分コライダーの始点ローカル座標
	static constexpr VECTOR LINE_END_POS = { -50.0f, -10.0f, 50.0f };		// 線分コライダーの終点ローカル座標
	static constexpr VECTOR CAPSULE_START_POS = { -50.0f, 0.0f, 140.0f };	// カプセルコライダーの始点ローカル座標
	static constexpr VECTOR CAPSULE_END_POS = { -50.0f, 0.0f, -40.0f };	// カプセルコライダーの終点ローカル座標

	// 各銃口のローカルオフセット座標リスト
	static constexpr VECTOR MUZZLE_POS[MUZZLE_MAX_COUNT] = {
		{ -52.0f, 4.0f, 150.0f },
		{ -47.0f, 1.0f, 150.0f },
		{ -47.0f, -5.0f, 150.0f },
		{ -52.0f, -8.0f, 150.0f },
		{ -57.0f, 1.0f, 150.0f },
		{ -57.0f, -5.0f, 150.0f },
	};
};