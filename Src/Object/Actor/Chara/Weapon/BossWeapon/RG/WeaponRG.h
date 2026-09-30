#pragma once
#include <memory>
#include "../../WeaponBase.h"

class BBulletLaser;


class WeaponRG : public WeaponBase
{
public:

	WeaponRG(void);

	~WeaponRG(void) override = default;

	/// @brief 武器モデル等のリソース読み込み処理
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief ボーン情報およびターゲット情報の受け取り
	/// @param _id 接続ボーンの番号
	/// @param _trans 接続ボーンを持つ対象のトランスフォーム
	/// @param _tag 当たり判定登録用タグ
	/// @param _playerPos ターゲット（プレイヤー）の座標
	void SetBone(int _id, const Transform& _trans, ColliderBase::TAG _tag, const VECTOR& _playerPos) override;

	/// @brief 現在のワールド座標を取得する
	/// @return 現在のワールド座標
	const VECTOR GetPos(void) const override;

	/// @brief ダメージを受けてHPを減らす
	/// @param _damage 受けたダメージ量
	void SetDamage(int _damage) override { hp_ -= _damage; }

	/// @brief 生存状態を取得する
	/// @return true: 生きている / false: 死亡
	bool GetIsAlive(void) override { return isAlive_; }

	/// @brief 現在のHPを取得する
	/// @return 現在のHP
	int GetHp(void) override { return hp_; }

	/// @brief HPを設定する
	/// @param _hp 設定するHP量
	void SetHp(int _hp) override { hp_ = _hp; }

	/// @brief 状態（ステート）を変更する
	/// @param _state 遷移先のステート
	void ChangeState(STATE _state) override;

	/// @brief 攻撃中かどうかを取得する
	/// @return true: 攻撃中 / false: 非攻撃中
	bool GetIsAttack(void) { return isAttack_; }

protected:

	/// @brief トランスフォーム（大きさ・回転・座標）の初期化
	void InitTransform(void) override;

	/// @brief 衝突判定の初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理（ステート設定等）
	void InitPost(void) override;

	/// @brief フレーム毎の更新処理
	void UpdateProcess(void) override;

	/// @brief 更新後の個別処理
	void UpdateProcessPost(void) override;

	/// @brief 描画前処理（レーザー描画など）
	void DrawPre(void) override;

	/// @brief 衝突予約処理
	void CollisionReserve(void) override {};

	/// @brief プレイヤーの方向を向く処理
	void LookPlayer(void) override;

	// ステート管理用
	std::function<void(void)> stateUpdate_;		// 現在ステートの更新関数ポインタ
	/// @brief 内部ステートの遷移処理（int値版）
	/// @param _state 遷移先のステート値
	void ChangeState(int _state) override;

	/// @brief 待機ステートの開始処理
	void ChangeStateIdle(void) override;

	/// @brief 攻撃準備（チャージ）ステートの開始処理
	void ChangePreparation(void);

	/// @brief 攻撃ステートの開始処理
	void ChangeStateAttack(void) override;

	/// @brief 死亡・破壊ステートの開始処理
	void ChangeStateEnd(void) override;

	/// @brief 攻撃準備（チャージ）ステートの毎フレーム更新
	void UpdatePreparation(void);

	/// @brief 攻撃ステートの毎フレーム更新
	void UpdateAttack(void) override;

	/// @brief 待機ステートの毎フレーム更新
	void UpdateIdle(void) override;

	/// @brief 死亡・破壊ステートの毎フレーム更新
	void UpdateEnd(void) override;

private:

	static constexpr VECTOR LINE_START_POS = { 0.0f, -10.0f, -60.0f };			// 線分コライダーの始点ローカル座標
	static constexpr VECTOR LINE_END_POS = { 0.0f, -20.0f, -60.0f };			// 線分コライダーの終点ローカル座標
	static constexpr VECTOR CAPSULE_START_POS = { 0.0f, -100.0f, -60.0f };		// カプセルコライダーの始点ローカル座標
	static constexpr VECTOR CAPSULE_END_POS = { 0.0f, 80.0f, -60.0f };			// カプセルコライダーの終点ローカル座標
	static constexpr float CAPSULE_RADIUS = 30.0f;								// カプセルコライダーの半径

	static constexpr float UP_ROT = 0.3f;										// 毎フレームの仰角上昇量（度）
	static constexpr float MAX_UP_ROT = 90.0f;									// 最大仰角（度）
	static constexpr int MAX_CHARGE_COUNT = 120;								// 攻撃開始までのチャージフレーム数

	static constexpr VECTOR LASER_EMIT_OFFSET = { 0.0f, 400.0f, -60.0f };		// レーザー発射口のローカルオフセット座標
	static constexpr VECTOR EFFECT_SCALE = { 300.0f, 300.0f, 300.0f };			// レーザーエフェクトのスケール
	static constexpr VECTOR ZERO_VECTOR = { 0.0f, 0.0f, 0.0f };					// ゼロベクトル定数
	static constexpr float PITCH_ANGLE_90 = 90.0f;								// X軸回転の90度オフセット

	std::unique_ptr<BBulletLaser> bulletLaser_;									// レーザー弾管理オブジェクト
	float localUpRot_;															// 砲身のカレント仰角（度）
	int chargeCount_;															// チャージ状態のフレームカウント
	bool isAttack_;																// 攻撃中フラグ
};