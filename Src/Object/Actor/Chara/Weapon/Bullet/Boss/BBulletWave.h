#pragma once
#include "BBulletBase.h"

/// @brief ボス用衝撃波（ウェーブ）弾クラス
class BBulletWave : public BBulletBase
{
public:

	/// @brief コンストラクタ
	/// @param _transform ボスのトランスフォーム参照
	BBulletWave(const Transform& _transform);

	~BBulletWave(void) override;

	/// @brief リソースロード
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief 攻撃状態を設定する
	/// @param _isAttack true: 攻撃中 / false: 非攻撃中
	void SetIsAttack(bool _isAttack) override { isAttack_ = _isAttack; }

	/// @brief 座標を設定する
	/// @param _pos 設定座標
	void SetPos(VECTOR _pos);

	/// @brief 弾の生成と初期パラメータ設定（ウェーブでは未使用）
	/// @param _pos 発射座標
	/// @param _dir 発射方向
	/// @param _radius 当たり判定半径
	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius) override {}

	/// @brief プレイヤー座標の設定（ウェーブでは未使用）
	/// @param _pos プレイヤー座標
	void SetPlayerPos(VECTOR _pos) override {}

	/// @brief 上限高度の設定（ウェーブでは未使用）
	/// @param _pos 上限高度
	void SetUpMaxPos(float _pos) override {}

	/// @brief 発射元トランスフォームの設定（ウェーブでは未使用）
	/// @param _trans 発射元のトランスフォーム
	void SetTransform(const Transform& _trans) override {}

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

	static constexpr float INIT_RADIUS = 4.0f;								// 衝撃波の初期半径
	static constexpr float INCREASE_RADIUS = 4.0f;							// 1フレームあたりの半径増加量
	static constexpr float MAX_RADIUS = 1000.0f;							// 衝撃波の最大到達半径
	static constexpr VECTOR COLLIDER_OFFSET = { 0.0f, 0.0f, 0.0f };			// コライダーのローカルオフセット

	Transform bossTransform_;												// ボスのトランスフォームデータ
};