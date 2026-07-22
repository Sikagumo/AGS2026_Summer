#pragma once
#include "../../WeaponBase.h"
#include <algorithm>
#include <vector>


class BBulletBase;


class WeaponCannon :
	public WeaponBase
{
public:
	WeaponCannon();

	~WeaponCannon(void)override = default;

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

	void SetHp(int _hp)override { hp_ = _hp; }
	

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

	
	void LookPlayer(void) override ;

	// 前描画
	void DrawPre(void) override;


	void CollisionReserve(void) override {};


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

	static constexpr VECTOR LINE_START_POS = { 0.0f,50.0f,60.0f };
	static constexpr VECTOR LINE_END_POS = { 0.0f,40.0f,60.0f };
	static constexpr VECTOR CAPSULE_START_POS = { 0.0f,50.0f,160.0f };
	static constexpr VECTOR CAPSULE_END_POS = { 0.0f,50.0f,-40.0f };

	static constexpr float LIMIT_MIN_ANGLE = -10.0f;
	static constexpr float LIMIT_MAX_ANGLE = 10.0f;

	static constexpr float CAPSULE_RADIUS = 20.0f;

	static constexpr float DOUN_ROCK = -45.0f;

	static constexpr float BULLET_HIT_SIZE = 18.0f;


	bool isAttack_;
	VECTOR bulletDir_;
	std::vector<std::shared_ptr<BBulletBase>> bullets_;

	int look;

	
	int count_;
	

	// 弾を発射する共通の仕組み（中身は現在のMGL/MGRのものと同じ）
	virtual void CreateBullets(void);
	std::shared_ptr<BBulletBase> GetValidBullet(void);

};

