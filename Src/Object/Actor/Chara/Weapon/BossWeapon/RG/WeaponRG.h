#pragma once
#include "../../WeaponBase.h"

/// <summary>
/// レールガンクラス
/// </summary>

class WeaponRG :
	public WeaponBase
{
public:

	WeaponRG();

	~WeaponRG(void)override = default;

	// リソースロード
	void Load(void) override;

	//リリース
	void ReleasePost(void)override;

	/// <summary>
	/// ボーン情報の受け取り用
	/// </summary>
	/// <param name="_id">接続ボーンの番号</param>
	/// <param name="_trans">接続ボーンを持つ対象のトランスフォーム</param>
	/// <param name="_tag">当たり判定登録よタグ</param>
	void SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos) override;

	///現在の座標
	const VECTOR GetPos(void) const override;
	/// <summary>
	/// ウェポンのダメージ受け取り用
	/// </summary>
	/// <param name="_damage">実数ダメージ</param>
	void SetDamage(int _damage)override { hp_ -= _damage; }
	/// <summary>
	/// ウェポンの生存状態を渡すよう
	/// </summary>
	/// <param name=""></param>
	/// <returns>true=生きている</returns>
	bool GetIsAlive(void)override { return isAlive_; }
	/// <summary>
	/// ウェポンの現在HPを渡すよう
	/// </summary>
	/// <param name=""></param>
	/// <returns>現在HP</returns>
	int GetHp(void)override { return hp_; }

	// 状態遷移
	void ChangeState(STATE _state)override;

	

protected:

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

	void UpdateProcess(void) override;
	void UpdateProcessPost(void) override;

	// 前描画
	void DrawPre(void) override;

	void CollisionReserve(void) override {};


	void LookPlayer(void) override {};





	// 状態（ステート）管理用の関数ポインタと関数群
	std::function<void(void)> stateUpdate_;
	void ChangeState(int state) override;
	void ChangeStateIdle(void) override;
	void ChangePreparation(void);
	void ChangeStateAttack(void) override;
	void ChangeStateEnd(void) override;

	void UpdatePreparation(void);
	void UpdateAttack(void) override;
	void UpdateIdle(void) override;
	void UpdateEnd(void) override;

private:

	static constexpr VECTOR LINE_START_POS = { 0.0f,-10.0f,-60.0f };
	static constexpr VECTOR LINE_END_POS = { 0.0f,-20.0f,-60.0f };
	static constexpr VECTOR CAPSULE_START_POS = { 0.0f,-100.0f,-60.0f };
	static constexpr VECTOR CAPSULE_END_POS = { 0.0f,80.0f,-60.0f };
	static constexpr float CAPSULE_RADIUS = 30.0f;
	static constexpr float UP_ROT = 3.0f;
	static constexpr float MAX_UP_COUNT = 30.0f;
	static constexpr float MAX_CHARGE_COUNT = 60.0f;


	int upCount_ = 0;		//上昇回数

	int ChargeCount_ = 0;	//チャージ回数


};

