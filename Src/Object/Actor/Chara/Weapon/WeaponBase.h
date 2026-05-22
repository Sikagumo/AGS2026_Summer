
#pragma once

#include <memory>
#include "../../ActorBase.h"
#include "../../../Collider/ColliderBase.h"

class WeaponBase :
    public ActorBase
{
public:
	struct Bone {
		int id = 0;
		Transform transform;

	};


	WeaponBase(void);
	virtual ~WeaponBase(void)override = default;


	void Update(void)override final;

	

	void Release(void)override;
	/// <summary>
	/// ボーン情報とタグの取得
	/// </summary>
	/// <param name="_id">ボーンのナンバー</param>
	/// <param name="_trans">ボーンを持つ相手のトランスフォーム</param>
	/// <param name="_tag">当たり判定用のタグ</param>
	virtual void SetBone(int _id, Transform _trans,ColliderBase::TAG _tag)=0;
	/// <summary>
	/// ダメージの受け渡し
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual int GetDamage(void) { return attackDamage_; }
	/// <summary>
	/// ダメージの受け取り
	/// </summary>
	/// <param name="_damege">ダメージの数値</param>
	virtual void SetDamage(int _damege) { hp_ -= _damege; }
	/// <summary>
	/// ロックオン用の座標
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual VECTOR GetPos(void) const = 0;

private:


protected:

	

	static constexpr VECTOR WEAPON_SIZE = { 3.0f,3.0f,3.0f };
	static constexpr float WEAPON_ROT = 180.0f;



	int hp_;
	int attackDamage_;	
	VECTOR localBackPos_;	//当たり判定用の後方の座標
	VECTOR localFrontPos_;	//当たり判定用の前方の座標
	VECTOR localPos_;		//カメラのロックオン用の中央座標
	Bone bone_;
	ColliderBase::TAG tag_;

	virtual void DrawPre(void)override;

	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	//受けたダメージをボスに伝えるための関数
	virtual float Damage(void)=0;


	virtual void CollisionReserve(void) {};
};

