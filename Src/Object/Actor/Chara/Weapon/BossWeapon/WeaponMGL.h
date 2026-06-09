#pragma once
#include <memory>
#include <vector>
#include "../WeaponBase.h"

class BBulletBase;

class WeaponMGL :
	public WeaponBase
{
public:
	//発射位置
	//{ -55.0f,5.0f,140.0f }
	//



	WeaponMGL(void);

	~WeaponMGL(void)override = default;

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

	void LookPlayer(void) override;

	
	// 状態遷移
	void ChangeState(int state)override;
	void ChangeStateIdle(void)override;
	void ChangeStateAttack(void)override;
	void ChangeStateEnd(void)override;

	void UpdateAttack(void)override;
	void UpdateIdle(void)override;
	void UpdateEnd(void)override;

private:

	static constexpr VECTOR LINE_START_POS = { -50.0f,0.0f,50.0f };
	static constexpr VECTOR LINE_END_POS = { -50.0f,-10.0f,50.0f };
	static constexpr VECTOR CAPSULE_START_POS = { -50.0f,0.0f,140.0f };
	static constexpr VECTOR CAPSULE_END_POS = { -50.0f,0.0f,-40.0f };
	static constexpr float CAPSULE_RADIUS = 20.0f;
	static constexpr int MAX_BULLET_COUNT = 200;

	//発射位置
	static constexpr int MUZZLE_MAX_COUNT = 6;

	const VECTOR MUZZLE_POS[MUZZLE_MAX_COUNT] = {
		{ -52.0f,4.0f,150.0f },
		{ -47.0f,1.0f,150.0f },
		{ -47.0f,-5.0f,150.0f },
		{ -52.0f,-8.0f,150.0f },
		{ -57.0f,1.0f,150.0f },
		{-57.0f,-5.0f,150.0f },
	};


	


	std::vector<std::shared_ptr<BBulletBase>> bullets_;
	VECTOR bulletDir_;

	int bulletCount_;

	VECTOR muzzlePos_[MUZZLE_MAX_COUNT];
	int muzzleCount_;


	std::shared_ptr<BBulletBase> GetValidBullet(void);
	void CreateBullets(void);
};

