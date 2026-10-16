#pragma once
#include "BBulletBase.h"

/// @brief ボス用レーザー弾クラス
class BBulletLaser : public BBulletBase
{
public:

	BBulletLaser(void);

	~BBulletLaser(void) override;

	/// @brief リソースの読み込み
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief 攻撃状態を設定する
	/// @param _isAttack true: 攻撃中 / false: 非攻撃中
	void SetIsAttack(bool _isAttack) override { isAttack_ = _isAttack; }

	/// @brief 弾の生成と初期パラメータ設定
	/// @param _pos 発射座標
	/// @param _dir 発射方向
	/// @param _radius 当たり判定半径
	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius) override { transform_.pos = _pos; dir_ = _dir; radius_ = _radius; }

	/// @brief プレイヤー座標の設定（レーザーでは未使用）
	/// @param _pos プレイヤー座標
	void SetPlayerPos(VECTOR _pos) override {}

	/// @brief 上限高度の設定（レーザーでは未使用）
	/// @param _pos 上限高度
	void SetUpMaxPos(float _pos) override {}

	/// @brief 発射元トランスフォームの設定
	/// @param _trans 発射元のトランスフォーム
	void SetTransform(const Transform& _trans) override { weaponTrans_ = _trans; }

	/// @brief 照射実行（拡張用）
	void Shot(void);

protected:

	/// @brief トランスフォームの初期化
	void InitTransform(void) override;

	/// @brief コライダーの初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理
	void InitPost(void) override;

	/// @brief フレーム毎の更新処理
	void UpdateProcess(void) override;

	/// @brief 更新処理後の個別処理
	void UpdateProcessPost(void) override;

	/// @brief 描画処理
	void DrawPre(void) override;

private:

	static constexpr VECTOR CAPSULE_START_POS = { 0.0f, 0.0f, 0.0f };		// カプセルコライダーの始点ローカル座標
	static constexpr VECTOR CAPSULE_END_POS = { 0.0f, 8000.0f, 0.0f };		// カプセルコライダーの終点ローカル座標
	static constexpr float CAPSULE_RADIUS = 300.0f;							// カプセルコライダーの半径
};