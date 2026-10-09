#pragma once
#include "../../WeaponBase.h"
#include <algorithm>
#include <memory>
#include <vector>

class BBulletBase;


class WeaponCannon : public WeaponBase
{
public:
	WeaponCannon();
	~WeaponCannon(void) override = default;

	/// @brief 描画用リソースのロード処理
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief 接続ボーン情報およびターゲット情報の受け取り
	/// @param _id 接続ボーンの番号
	/// @param _trans 接続ボーンを持つ対象のトランスフォーム
	/// @param _tag 当たり判定登録用タグ
	/// @param _playerPos ターゲット（プレイヤー）の座標
	void SetBone(int _id, const Transform& _trans, ColliderBase::TAG _tag, const VECTOR& _playerPos) override;

	/// @brief 武装の現在ワールド座標を取得する
	/// @return ワールド座標
	const VECTOR GetPos(void) const override;

	/// @brief ダメージ処理
	/// @param _damage 受け取った実数ダメージ
	void SetDamage(int _damage) override { hp_ -= _damage; }

	/// @brief 生存状態の取得
	/// @return true: 生存 / false: 死亡
	bool GetIsAlive(void) override { return isAlive_; }

	/// @brief 現在HPの取得
	/// @return 現在HP
	int GetHp(void) override { return hp_; }

	/// @brief HPの設定
	/// @param _hp 設定するHP値
	void SetHp(int _hp) override { hp_ = _hp; }

	/// @brief ステートの変更処理
	/// @param _state 遷移先のステート
	void ChangeState(STATE _state) override;

protected:

	/// @brief トランスフォームの初期化
	void InitTransform(void) override;

	/// @brief 衝突判定の初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理
	void InitPost(void) override;

	/// @brief メイン更新処理
	void UpdateProcess(void) override;

	/// @brief 更新後の後処理
	void UpdateProcessPost(void) override;

	/// @brief プレイヤー方向へ向く処理
	void LookPlayer(void) override;

	/// @brief 前描画処理
	void DrawPre(void) override;

	/// @brief 衝突予約処理
	void CollisionReserve(void) override {}

	/// @brief ステート変更（数値インデックス）
	/// @param _state 遷移先のステート値
	void ChangeState(int _state) override;

	/// @brief アイドル状態変更時の処理
	void ChangeStateIdle(void) override;

	/// @brief 攻撃状態変更時の処理
	void ChangeStateAttack(void) override;

	/// @brief 終了（破壊）状態変更時の処理
	void ChangeStateEnd(void) override;

	/// @brief 攻撃状態の更新処理
	void UpdateAttack(void) override;

	/// @brief アイドル状態の更新処理
	void UpdateIdle(void) override;

	/// @brief 終了状態の更新処理
	void UpdateEnd(void) override;

private:

	static constexpr VECTOR LINE_START_POS = { 0.0f, 50.0f, 60.0f };		// 線分当たり判定の開始ローカル座標
	static constexpr VECTOR LINE_END_POS = { 0.0f, 40.0f, 60.0f };			// 線分当たり判定の終了ローカル座標
	static constexpr VECTOR CAPSULE_START_POS = { 0.0f, 50.0f, 160.0f };	// カプセル当たり判定の開始ローカル座標
	static constexpr VECTOR CAPSULE_END_POS = { 0.0f, 50.0f, -40.0f };		// カプセル当たり判定の終了ローカル座標

	static constexpr float LIMIT_MIN_ANGLE = -10.0f;	// X軸回転の最小制限角度（度）
	static constexpr float LIMIT_MAX_ANGLE = 10.0f;		// X軸回転の最大制限角度（度）

	static constexpr float CAPSULE_RADIUS = 20.0f;		// カプセル当たり判定の半径
	static constexpr float DOWN_LOCK = -45.0f;			// 照準調整用のY軸オフセット（タイポ修正：DOUN_ROCK -> DOWN_LOCK）
	static constexpr float BULLET_HIT_SIZE = 18.0f;		// 弾の当たり判定サイズ

	bool isAttack_;											// 攻撃中フラグ
	VECTOR bulletDir_;										// 弾の発射方向ベクトル
	std::vector<std::shared_ptr<BBulletBase>> bullets_;	// 弾のオブジェクトプール
	int look_;												// 視線・向き管理用ID
	int count_;												// 汎用フレームカウンタ

	/// @brief 弾の生成処理
	virtual void CreateBullets(void);

	/// @brief 利用可能な弾を取得（プールからの再利用または新規生成）
	/// @return 弾のスマートポインタ
	std::shared_ptr<BBulletBase> GetValidBullet(void);
};