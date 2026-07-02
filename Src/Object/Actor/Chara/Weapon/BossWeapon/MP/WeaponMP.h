#pragma once
#include "../../WeaponBase.h"
#include <algorithm>
#include <vector>

class BBulletBase;

class WeaponMP :
	public WeaponBase
{
public:

	WeaponMP();

	~WeaponMP(void)override = default;

	// リソースロード
	void Load(void) override;

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

	/// <summary>
	/// 右か左かを指定するフラグ
	/// </summary>
	/// <param name="_isLR">TURE＝L,FALSE=R</param>
	void IsLR(bool _isLR) { isLR_ = _isLR; }
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
	void ChangeStateAttack(void) override;
	void ChangeStateEnd(void) override;

	void UpdateAttack(void) override;
	void UpdateIdle(void) override;
	void UpdateEnd(void) override;


private:

	static constexpr VECTOR LINE_START_POS = { 0.0f,0.0f,-40.0f };
	static constexpr VECTOR LINE_END_POS = { 0.0f,-10.0f,-40.0f };
	static constexpr VECTOR SPHERE_START_POS = { 0.0f,0.0f,-40.0f };
	static constexpr float SPHERE_RADIUS = 40.0f;
	//発射位置
	static constexpr int MUZZLE_MAX_COUNT = 6;

	//最大攻撃回数
	static constexpr int MAX_ATTACK_COUNT = 3;
	//発射ディレイ
	static constexpr int ATTACK_DELAY = 5;

	//落下し始め最小値
	static constexpr float MIN_FALL_POS = 3000.0f;
	//落下し始め上昇値
	static constexpr float UP_FALL_POS = 1000.0f;

	//攻撃半径
	static constexpr float ATTACK_RADIUS = 400.0f;

	const VECTOR MUZZLE_POS[MUZZLE_MAX_COUNT] = {
		{9.0f,23.0f,-15.5f},
		{9.0f,23.0f,-33.0f},
		{9.0f,24.0f,-51.0f},
		{-9.0f,23.0f,-15.5f},
		{-9.0f,23.0f,-33.0f},
		{-9.0f,24.0f,-51.0f},
	};

	int attackCount_;
	int outCount_;
	VECTOR bulletDir_;
	VECTOR muzzlePos_[MUZZLE_MAX_COUNT];
	int muzzleCount_;
	bool isLR_;

	std::vector<std::shared_ptr<BBulletBase>> bullets_;
	// 弾を発射する共通の仕組み（中身は現在のMGL/MGRのものと同じ）
	virtual void CreateBullets(void);
	std::shared_ptr<BBulletBase> GetValidBullet(void);

};

