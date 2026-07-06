#pragma once

#include <DxLib.h>

#include "../Common/Transform.h"


/// @brief コライダーの基底クラス
/// @details
/// 各種コライダーの共通インターフェースを定義する
class ColliderBase
{
public:

	// 形状
	enum class SHAPE
	{
		NONE,        //　無効
		LINE,        //  線分
		SPHERE,      //  球
		CAPSULE,     //  カプセル
		MODEL,       //  モデル
	};

	// 衝突種別
	enum class TAG
	{
		STAGE,			// ステージ
		PLAYER,			// プレイヤー
		PLAYER_BULLET,	// プレイヤー弾爆弾
		PLAYER_BLAST,	// プレイヤー
		ENEMY,          // 敵
		BOSS,			// ボス
		WEAPON_MG_L,	//マシンガン（左
		WEAPON_MG_R,	//マシンガン（右
		WEAPON_CANNON_L,//キャノン（左
		WEAPON_CANNON_R,//キャノン（右
		WEAPON_MP_L,	//ミサイルポッド（左
		WEAPON_MP_R,	//ミサイルポッド（右
		WEAPON_RG,		//レールガン
		CAMERA,         // カメラ
		WALL,           // 壁
		HIT_WAVE,		//衝撃波
		MG_BULLET,		//マシンガンの弾
		ROAD_ATTACK,	//体当たり
		MISSILE_ATTACK,	//ミサイル攻撃
		MISSILE_PUSH,	//ミサイル吹き飛ばして
	};

	/// @brief コンストラクタ
	/// @param shape コライダー形状
	/// @param tag 衝突種別
	/// @param follow 追従対象Transform
	ColliderBase(SHAPE shapeType, TAG collisionTag, const Transform* followTarget);

	/// @brief デストラクタ
	virtual ~ColliderBase(void) = default;

	/// @brief デバッグ描画を行う
	void Draw(void) const;

	/// @brief 追従先を取得する
	/// @return Transformポインタ
	const Transform* GetFollowTarget(void) const;

	/// @brief 追従先を設定する
	/// @param followTarget 新しいTransform
	void SetFollowTarget(Transform* followTarget);

	/// @brief コライダー形状を取得する
	/// @return 形状種別
	SHAPE GetShapeType(void) const;

	/// @brief 衝突タグを取得する
	/// @return タグ種別
	TAG GetCollisionTag(void) const;

	/// @brief 有効状態を取得する
	/// @return true: 有効 / false: 無効
	bool IsActive(void) const;

	/// @brief 有効状態を設定する
	/// @param isActive trueで有効化
	void SetActive(bool isActive);

	/// @brief 押し出し判定するかどうか
	/// @return trueなら押し出す
	bool IsTrigger(void) const { return isTrigger_; }

	/// @brief 押し出し判定の設定
	/// @param isTrigger trueなら押し出す、falseなら押し出さない
	void SetTriger(bool isTrigger);

	/// @brief 半径を設定する
	/// @param _radius 新しい半径の値
	virtual void SetRadius(float _radius) {}
protected:

	// デバック表示の色
	static constexpr int DEBUG_COLOR_ACTIVE = 0xff0000;      // 有効なコライダーは赤色
	static constexpr int DEBUG_COLOR_INACTIVE = 0xaaaaaa;    // 無効なコライダーは灰色

	// 形状
	SHAPE shapeType_;

	// 衝突種別
	TAG collisionTag_;

	// 追従先
	const Transform* followTarget_;

	// 有効フラグ
	bool isActive_;

	// 押し出しするかどうかのフラグ
	bool isTrigger_;

	/// @brief ローカル座標をワールド座標に変換する
	/// @param localPos ローカル座標
	/// @return ワールド座標
	VECTOR TransformLocalToWorld(const VECTOR& localPos) const;

	/// @brief デバッグ描画（派生クラスで実装）
	/// @param color 描画色
	virtual void DrawDebug(int color) const = 0;
};

