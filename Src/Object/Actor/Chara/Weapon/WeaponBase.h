#pragma once
#include <memory>
#include <array>
#include <map>
#include <functional>
#include "../../ActorBase.h"
#include "../../../Collider/ColliderBase.h"

/// @brief すべての武器（ウェポン）の基底クラス
class WeaponBase : public ActorBase
{
public:

	/// @brief ウェポンの状態（ステート）
	enum class STATE
	{
		IDLE,			// アイドル（待機）
		ATTACK,			// 攻撃
		END,			// 終了・破壊
		PREPARATION,	// 攻撃準備
	};

	/// @brief ボーン情報の受け取り構造体
	struct Bone {
		int id = 0;				// ボーンID
		Transform transform;	// ボーンを持つ対象のトランスフォーム
		VECTOR playerPos;		// プレイヤーの位置座標
	};

	WeaponBase(void);
	virtual ~WeaponBase(void) override = default;

	/// @brief 毎フレームの更新処理
	void Update(void) override final;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief ボーン情報と当たり判定タグの設定
	/// @param _id ボーンの番号
	/// @param _trans ボーンを持つ対象のトランスフォーム
	/// @param _tag 当たり判定用のタグ
	/// @param _playerPos ターゲット（プレイヤー）の座標
	virtual void SetBone(int _id, const Transform& _trans, ColliderBase::TAG _tag, const VECTOR& _playerPos) = 0;

	/// @brief 照準・ロックオン用の座標を取得する
	/// @return ワールド座標
	virtual const VECTOR GetPos(void) const = 0;

	/// @brief ウェポンのダメージ受け取り用
	/// @param _damage 実数ダメージ
	virtual void SetDamage(int _damage) = 0;

	/// @brief ウェポンの生存状態を取得する
	/// @return true: 生きている / false: 死亡
	virtual bool GetIsAlive(void) = 0;

	/// @brief ウェポンの現在HPを取得する
	/// @return 現在HP
	virtual int GetHp(void) = 0;

	/// @brief ウェポンのHPを設定する
	/// @param _hp 設定するHP値
	virtual void SetHp(int _hp) = 0;

	/// @brief 状態（ステート）を変更する
	/// @param _state 遷移先のステート
	virtual void ChangeState(STATE _state);

protected:

	static constexpr float MAX_FALL_SPEED = -30.0f;				// 最大落下速度
	static constexpr VECTOR WEAPON_SIZE = { 3.0f, 3.0f, 3.0f };	// 武器の基本スケール
	static constexpr float WEAPON_ROT = 180.0f;					// 武器の基本回転角度
	static constexpr float MOVE_SPEED = 5.0f;					// 吹っ飛び等の移動速度
	static constexpr float JUMP_POW = 10.0f;					// 吹っ飛び等のジャンプ初速

	int hp_;					// ウェポンのHP
	bool isAlive_;				// ウェポンの生存フラグ
	VECTOR movePow_;			// 重力・移動用ベクトル
	float jumpPow_;				// 吹っ飛び用の垂直移動量
	bool isJump_;				// 吹っ飛び（空中）状態フラグ
	VECTOR moveDir_;			// 移動方向ベクトル
	float speed_;				// 移動速度
	VECTOR localBackPos_;		// 当たり判定用の後方ローカル座標
	VECTOR localFrontPos_;		// 当たり判定用の前方ローカル座標
	VECTOR localPos_;			// カメラロックオン用の標準ローカル座標
	Bone bone_;					// 追従対象のボーン情報
	ColliderBase::TAG tag_;		// 当たり判定登録用のタグ

	/// @brief 前描画処理
	void DrawPre(void) override;

	/// @brief 固有の更新処理（メイン）
	virtual void UpdateProcess(void) = 0;

	/// @brief 固有の更新処理（後処理）
	virtual void UpdateProcessPost(void) = 0;

	/// @brief 重力加速度の計算
	void CalcGravityPow(void);

	/// @brief 衝突処理の取りまとめ
	void Collision(void);

	/// @brief 地形（ステージ）との着地判定
	void CollisionGravity(void);

	/// @brief 衝突予約処理
	virtual void CollisionReserve(void) {}

	/// @brief プレイヤーへ向く処理
	virtual void LookPlayer(void) {}

	STATE state_;				// 現在のステート（Enum）
	int stateBase_;				// 現在のステート（数値）

	// 状態遷移時の初期化関数マップ
	std::map<int, std::function<void(void)>> stateChanges_;

	/// @brief ステート変更処理（内部用）
	/// @param _state 遷移先のステート値
	virtual void ChangeState(int _state);

	/// @brief アイドル状態変更時の処理
	virtual void ChangeStateIdle(void);

	/// @brief 攻撃状態変更時の処理
	virtual void ChangeStateAttack(void);

	/// @brief 終了状態変更時の処理
	virtual void ChangeStateEnd(void);

	// ステート毎の更新関数ポインタ
	std::function<void(void)> stateUpdate_;

	/// @brief 攻撃状態の更新処理
	virtual void UpdateAttack(void);

	/// @brief アイドル状態の更新処理
	virtual void UpdateIdle(void);

	/// @brief 終了状態の更新処理
	virtual void UpdateEnd(void);
};