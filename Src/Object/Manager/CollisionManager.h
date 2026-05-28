#pragma once
#include <vector>
#include <map>
#include <DxLib.h>
#include <set>

#include "../Collider/ColliderBase.h"

class ActorBase;

/// @brief 衝突結果を格納する構造体
struct CollisionInfo
{
	// 衝突したコライダー関連
	const ColliderBase* myCollider = nullptr;  // 自身のコライダーポインタ
	const ColliderBase* hitCollider = nullptr; // 相手のコライダーポインタ

	// 衝突座標関連
	VECTOR hitPosition = VGet(0.0f, 0.0f, 0.0f); // 衝突位置
	VECTOR hitNormal = VGet(0.0f, 1.0f, 0.0f);   // 衝突面における法線ベクトル
	float penetration = 0.0f;                    // 衝突の深さ（めり込み量）

	// 衝突情報の有効フラグ（true：衝突中）
	bool isActive = false;

};

/// @brief ゲーム内の衝突判定を一元管理するシングルトンクラス
class CollisionManager
{
public:

	/// @brief インスタンスを明示的に生成
	static void CreateInstance(void);

	/// @brief インスタンスの取得
	/// @return CollisionManagerの参照
	static CollisionManager& GetInstance(void);

	/// @brief インスタンスの破棄
	static void DestroyInstance(void);

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief 登録データのリセット
	void Clear(void);

	/// @brief 判定対象のアクターを登録
	/// @param actor 登録するアクターへのポインタ
	void RegisterActor(ActorBase* _actor);

	/// @brief 判定対象のアクターを解除
	/// @param actor 解除するアクターへのポインタ
	void UnregisterActor(ActorBase* _actor);

	/// @brief 二つのコライダー間の衝突判定を実行
	/// @param colliderA コライダーA
	/// @param colliderB コライダーB
	/// @param outInfo 衝突情報を格納する構造体
	/// @return 衝突していればtrue
	bool CheckCollision(const ColliderBase* _colliderA,
		const ColliderBase* _colliderB, CollisionInfo& _outInfo);

	/// @brief 指定したアクターが特定のタグを持つ相手と衝突しているか調べる
	/// @param actor 調査対象のアクター
	/// @param targetTag 確認したい相手の衝突タグ
	/// @return 衝突していればtrue
	bool IsActorCollidingWithTag(const ActorBase* _actor, ColliderBase::TAG _targetTag) const;

	/// @brief 特定のタグ同士が現在衝突しているか調べる
	/// @param _tagA 調査対象のタグA
	/// @param _tagB 調査対象のタグB
	/// @return 衝突していればtrue
	bool IsTagCollidingWithTag(ColliderBase::TAG _targetTagA, ColliderBase::TAG _targetTagB) const;
	
	/// @brief 指定した衝突タグのコライダーの有効・無効を一括で切り替える
	/// @param _targetActor 対象とするアクター
	/// @param _targetTag 対象とする衝突タグ
	/// @param _isActive 有効にする場合はtrue、無効にする場合はfalse
	void SetCollisionActive(ActorBase* _targetActor, ColliderBase::TAG _targetTag, const bool _isActive = true);

	/// @brief 指定したタグを持つコライダーの半径を一括で変更する
	/// @param _targetActor 変更したいアクター
	/// @param _targetTag 変更したいコライダーのタグ
	/// @param _radius 新しく設定する半径の値
	void SetActorColliderRadius(ActorBase* _targetActor, ColliderBase::TAG _targetTag, 
		float _radius);

	/// @brief 衝突したアクター同士の押し戻し処理を行う
	/// @param actorA 動かす対象のアクターA
	/// @param actorB 動かす対象のアクターB
	/// @param info 衝突判定の結果
	void ResolveCollision(ActorBase* actorA, ActorBase* actorB, const CollisionInfo& info);

	/// @brief 衝撃波の当たり判定を行う
	/// @param _hitCol 当たる側のコライダー
	/// @param _waveCol 衝撃波のコライダー
	/// @param _waveThickness 衝撃波の厚み
	/// @param _waveHeight 衝撃波の高さ
	/// @return 当たっていれば true
	bool CheckHitWave(const ColliderBase* _hitCapsuleCol, ColliderBase* _waveCol, float _waveThickness, float _waveHeight);

private:

	// カリングを行う距離のデフォルト値
	static constexpr float DEFAULT_CULL_DIST = 1500.0f;

	// 更新のインターバル時間
	static constexpr float UPDATE_INTERVAL = 0.016f;

	// 衝撃波の定数関連
	static constexpr float HIT_WAVE_THICKNESS = 3.0f; // 衝撃波の厚み
	static constexpr float HIT_WAVE_HEIGHT = 15.0f;    // 衝撃波の高さ

	// シングルトンインスタンス
	static CollisionManager* instance_;

	// 判定対象のアクターリスト
	std::vector<ActorBase*> actors_;

	// 現在当たっているタグの組み合わせを保存する
	std::set<std::pair<ColliderBase::TAG, ColliderBase::TAG>> activeCollisions_;

	// 内部パラメータ関連
	float cullingDistSquare_;           // カリング距離の2乗（計算高速化用）
	float updateTimer_;                 // 更新タイマー

	/// @brief コンストラクタ
	CollisionManager(void);

	/// @brief デストラクタ
	~CollisionManager(void) = default;

	/// @brief コピー禁止
	CollisionManager(const CollisionManager&) = delete;

	/// @brief 代入禁止 
	CollisionManager& operator=(const CollisionManager&) = delete;

	/// @brief 全アクターのペアに対して衝突判定を更新
	void UpdateCollisionPars(void);

	/// @brief タグに基づいた衝突可能判定
	/// @param tagA 判定Aのタグ
	/// @param tagB 判定Bのタグ
	/// @return 衝突する組み合わせならtrue
	bool CanCollide(int _tagA, int _tagB) const;

	/// @brief 球体 vs 球体の衝突判定
	/// @param colliderA 判定対象A（球体）
    /// @param colliderB 判定対象B（球体）
    /// @param outInfo 衝突時の詳細データを格納する構造体
    /// @return 衝突している場合はtrue
	bool CheckSphereVsSphere(const ColliderBase* _colliderA, const ColliderBase* _colliderB,
		CollisionInfo& _outInfo);

	/// @brief 球体 vs カプセルの衝突判定
    /// @param sphereCol 判定対象（球体）
    /// @param capsuleCol 判定対象（カプセル）
    /// @param outInfo 衝突時の詳細データを格納する構造体
    /// @return 衝突している場合はtrue
	bool CheckSphereVsCapsule(const ColliderBase* _sphereCol, const ColliderBase* _capsuleCol,
		CollisionInfo& _outInfo);

	/// @brief カプセル vs モデル（地形）の衝突判定
    /// @param capsuleCol 判定対象（カプセル）
    /// @param modelCol 判定対象（3Dモデル/メッシュ）
    /// @param outInfo 衝突時の詳細データを格納する構造体
    /// @return 衝突している場合はtrue
	bool CheckCapsuleVsModel(const ColliderBase* _capsuleCol, const ColliderBase* _modelCol,
		CollisionInfo& _outInfo);

	/// @brief 線分 vs モデル（地形）の衝突判定
    /// @param lineCol 判定対象（線分/レイ）
    /// @param modelCol 判定対象（3Dモデル/メッシュ）
    /// @param outInfo 衝突時の詳細データを格納する構造体
    /// @return 衝突している場合はtrue
	bool CheckLineVsModel(const ColliderBase* _lineCol, const ColliderBase* _modelCol,
		CollisionInfo& _outInfo);

	/// @brief 線分上の最もターゲットに近い座標を算出
	/// @param startPos 線分の開始点
	/// @param endPos 線分の終了点
	/// @param targetPos ターゲット座標
	/// @return 線分上の最近接座標
	VECTOR GetNearestPointOnSegment(const VECTOR& _startPos,
		const VECTOR& _endPos, const VECTOR& _targetPos);

	/// @brief カプセル vs カプセルの衝突判定
	/// @param colliderA 判定対象A（カプセル）
	/// @param colliderB 判定対象B（カプセル）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	bool CheckCapsuleVsCapsule(const ColliderBase* _colliderA, const ColliderBase* _colliderB,
		CollisionInfo& _outInfo);
};

