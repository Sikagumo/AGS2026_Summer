#pragma once
#include "BBulletBase.h"

/// @brief ボス用キャノン弾クラス
class BBulletCannon : public BBulletBase
{
public:

	BBulletCannon(void);

	~BBulletCannon(void) override;

	/// @brief リソースの読み込み
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief 攻撃状態を設定する
	/// @param _isAttack true: 攻撃中 / false: 非攻撃中
	void SetIsAttack(bool _isAttack) override { isAttack_ = _isAttack; }

	/// @brief 座標を設定する
	/// @param _pos 設定座標
	void SetPos(VECTOR _pos) {};

	/// @brief 弾の生成と初期パラメータ設定
	/// @param _pos 発射座標
	/// @param _dir 発射方向
	/// @param _radius 当たり判定半径
	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius) override;

	/// @brief プレイヤー座標の設定（キャノン弾では未使用）
	/// @param _pos プレイヤー座標
	void SetPlayerPos(VECTOR _pos) override {}

	/// @brief 上限高度の設定（キャノン弾では未使用）
	/// @param _pos 上限高度
	void SetUpMaxPos(float _pos) override {}

	/// @brief 発射元トランスフォームの設定
	/// @param _trans 発射元のトランスフォーム
	void SetTransform(const Transform& _trans) override { weaponTrans_ = _trans; }

protected:

	/// @brief トランスフォームの初期化
	void InitTransform(void) override;

	/// @brief コライダーの初期化
	void InitCollider(void) override;

	/// @brief アニメーション・パラメータの初期化
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

	static constexpr float INIT_SPEED = 80.0f;								// 弾の初期移動速度
	static constexpr float MAX_ALIVE_TIME = 300.0f;							// 生存可能フレーム数（寿命）
	static constexpr VECTOR BULLET_SCALE = { 0.2f, 0.2f, 0.2f };			// 弾モデルの基本スケール
	static constexpr VECTOR COLLIDER_OFFSET = { 0.0f, 0.0f, 0.0f };			// 球コライダーのローカルオフセット座標

	int aliveTime_;															// 生存フレームカウンタ
};