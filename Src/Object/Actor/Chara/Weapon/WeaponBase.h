
#pragma once

#include <memory>
#include "../../ActorBase.h"
#include "../../../Collider/ColliderBase.h"

class WeaponBase :
    public ActorBase
{
public:
	//ボーン情報の受け取り用
	struct Bone {
		int id = 0;
		Transform transform;
		VECTOR playerPos;
	};


	WeaponBase(void);
	virtual ~WeaponBase(void)override = default;


	void Update(void)override final;

	

	void ReleasePost(void)override;
	/// <summary>
	/// ボーン情報とタグの取得
	/// </summary>
	/// <param name="_id">ボーンのナンバー</param>
	/// <param name="_trans">ボーンを持つ相手のトランスフォーム</param>
	/// <param name="_tag">当たり判定用のタグ</param>
	virtual void SetBone(int _id, Transform _trans,ColliderBase::TAG _tag, VECTOR _playerPos)=0;
	
	/// <summary>
	/// ロックオン用の座標
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual const VECTOR GetPos(void) const = 0;

	/// <summary>
	/// ウェポンのダメージ受け取り用
	/// </summary>
	/// <param name="_damage">実数ダメージ</param>
	virtual void SetDamage(int _damage) = 0;
	/// <summary>
	/// ウェポンの生存状態を渡すよう
	/// </summary>
	/// <param name=""></param>
	/// <returns>true=生きている</returns>
	virtual bool GetIsAlive(void)=0;
	/// <summary>
	/// ウェポンの現在HPを渡すよう
	/// </summary>
	/// <param name=""></param>
	/// <returns>現在HP</returns>
	virtual int GetHp(void) = 0;
private:


protected:

	// 最大落下速度
	static constexpr float MAX_FALL_SPEED = -30.0f;

	static constexpr VECTOR WEAPON_SIZE = { 3.0f,3.0f,3.0f };
	static constexpr float WEAPON_ROT = 180.0f;



	int hp_=500;			//ウェポンのHP
	bool isAlive_;			//ウェポンの生存フラグ
	VECTOR movePow_;		//重力用移動量
	VECTOR jumpPow_;		//吹っ飛び用の移動量
	bool isJump_;			//吹っ飛び中かのフラグ
	VECTOR localBackPos_;	//当たり判定用の後方の座標
	VECTOR localFrontPos_;	//当たり判定用の前方の座標
	VECTOR localPos_;		//カメラのロックオン用の中央座標
	Bone bone_;				//ボーン情報
	ColliderBase::TAG tag_;	//当たり判定登録用タグの保管

	//ドロー処理
	virtual void DrawPre(void)override;

	//アップデート処理
	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	//重力
	void CalcGravityPow(void);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	virtual void CollisionReserve(void) {};

	virtual void LookPlayer(void) {};
};

