#pragma once

#include <vector>
#include <map>
#include <set>
#include <array>
#include <DxLib.h>

#include "../Collider/ColliderBase.h"
#include "../Collider2D/Collider2DBase.h"

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

	// 衝突情報の有効フラグ
	bool isActive = false;
};

/// @brief ゲーム内の衝突判定を一元管理するシングルトンクラス
class CollisionController
{
public:

	/// @brief インスタンスを明示的に生成
	static void CreateInstance(void);

	/// @brief インスタンスの取得
	/// @return CollisionControllerの参照
	static CollisionController& GetInstance(void);

	/// @brief インスタンスの破棄
	static void DestroyInstance(void);

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief 登録データのリセット
	void Clear(void);

	/// @brief 判定対象のアクターを登録
	/// @param _actor 登録するアクターへのポインタ
	void RegisterActor(ActorBase* _actor);

	/// @brief 判定対象のアクターを解除
	/// @param _actor 解除するアクターへのポインタ
	void UnregisterActor(ActorBase* _actor);

	/// @brief 二つのコライダー間の衝突判定を実行
	/// @param _colliderA コライダーA
	/// @param _colliderB コライダーB
	/// @param _outInfo 衝突情報を格納する構造体
	/// @return 衝突していればtrue
	bool CheckCollision(const ColliderBase* _colliderA,
		const ColliderBase* _colliderB, CollisionInfo& _outInfo);

	/// @brief 指定したアクターが特定のタグを持つ相手と衝突しているか調べる
	/// @param _actor 調査対象のアクター
	/// @param _targetTag 確認したい相手の衝突タグ
	/// @return 衝突していればtrue
	bool IsActorCollidingWithTag(const ActorBase* _actor, ColliderBase::TAG _targetTag) const;

	/// @brief 特定のタグ同士が現在衝突しているか調べる
	/// @param _targetTagA 調査対象のタグA
	/// @param _targetTagB 調査対象のタグB
	/// @return 衝突していればtrue
	bool IsTagCollidingWithTag(ColliderBase::TAG _targetTagA, ColliderBase::TAG _targetTagB) const;

	/// @brief 指定したアクターが特定のタグと衝突した際のヒット位置を取得する
	/// @param _actor 調査対象のアクター
	/// @param _targetTag 確認したい相手の衝突タグ
	/// @return 衝突位置
	VECTOR IsActorHitPosWithTag(const ActorBase* _actor, ColliderBase::TAG _targetTag) const;

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

	/// @brief 指定したタグの球コライダーのローカル座標を変更する
	/// @param _targetActor 変更したいアクター
	/// @param _targetTag 変更したいコライダータグ
	/// @param _localPosition 新しい座標
	void SetActorSphereLocalPos(ActorBase* _targetActor, ColliderBase::TAG _targetTag,
		const VECTOR& _localPosition);

	/// @brief 指定したタグのカプセルコライダーの始点・終点・半径を一括で変更する
	/// @param _targetActor 変更したいアクター
	/// @param _targetTag 変更したいコライダーのタグ
	/// @param _localStartPos 新しい始点
	/// @param _localEndPos 新しい終点
	/// @param _radius 新しい半径
	void SetActorCapsuleShape(ActorBase* _targetActor, ColliderBase::TAG _targetTag,
		const VECTOR& _localStartPos, const VECTOR& _localEndPos, float _radius);

	/// @brief 衝突したアクター同士の押し戻し処理を行う
	/// @param _actorA 動かす対象のアクターA
	/// @param _actorB 動かす対象のアクターB
	/// @param _info 衝突判定の結果
	void ResolveCollision(ActorBase* _actorA, ActorBase* _actorB, const CollisionInfo& _info);

	/// @brief 2Dコライダーの登録
	/// @param _collider 登録する2Dコライダーのポインタ
	void RegisterCollider2D(Collider2DBase* _collider);

	/// @brief 2Dコライダーの登録解除
	/// @param _collider 解除する2Dコライダーのポインタ
	void UnregisterCollider2D(Collider2DBase* _collider);

	/// @brief 全ての2Dコライダーをクリア
	void ClearColliders2D(void);

	/// @brief 特定の2Dタグ同士の衝突判定の有効・無効を設定する
	/// @param _tagA 対象の2DタグA
	/// @param _tagB 対象の2DタグB
	/// @param _isEnable 衝突を有効にするか
	void SetCollisionGroup2D(Collider2DBase::TAG_2D _tagA,
		Collider2DBase::TAG_2D _tagB, bool _isEnable);

	/// @brief コライダー同士の詳細な衝突チェック
	/// @param _colA 1つ目の2Dコライダー
	/// @param _colB 2つ目の2Dコライダー
	/// @return 衝突しているか
	bool CheckCollision2D(const Collider2DBase* _colA,
		const Collider2DBase* _colB) const;

	/// @brief 特定の2Dタグ同士が現在衝突しているか調べる
	/// @param _targetTagA 調査対象の2DタグA
	/// @param _targetTagB 調査対象の2DタグB
	/// @return 衝突していればtrue
	bool IsTagCollidingWithTag2D(Collider2DBase::TAG_2D _targetTagA,
		Collider2DBase::TAG_2D _targetTagB) const;

	/// @brief 2Dコライダーのデバッグ描画
	void DrawDebug2D(void);

private:

	// 定数関連
	static constexpr float DEFAULT_CULL_DIST = 5000.0f; // カリングを行う距離のデフォルト値
	static const size_t MATRIX_SIZE_2D = 32;            // 2D衝突マトリクスの配列サイズ

	// シングルトンインスタンス
	static CollisionController* instance_;

	// 判定対象のアクターリスト
	std::vector<ActorBase*> actors_;

	// 現在当たっているタグの組み合わせを保存する
	std::set<std::pair<ColliderBase::TAG, ColliderBase::TAG>> activeCollisions_;

	// 全衝突情報をアクターごとに保存する
	std::map<const ActorBase*, std::vector<CollisionInfo>> currentColInfos_;

	// 2Dコライダー管理
	std::vector<Collider2DBase*> colliders2D_;

	// 現在当たっている2Dタグの組み合わせを毎フレーム保存する
	std::set<std::pair<Collider2DBase::TAG_2D, Collider2DBase::TAG_2D>> activeCollisions2D_;

	// 2D衝突マトリクス
	std::array<std::array<bool, MATRIX_SIZE_2D>, MATRIX_SIZE_2D> collisionMatrix2D_;

	// カリング距離の2乗
	float cullingDistSquare_;

	/// @brief コンストラクタ
	CollisionController(void);

	/// @brief デストラクタ
	~CollisionController(void) = default;

	/// @brief コピー禁止
	CollisionController(const CollisionController&) = delete;

	/// @brief 代入禁止 
	CollisionController& operator=(const CollisionController&) = delete;

	/// @brief 全アクターのペアに対して衝突判定を更新
	void UpdateCollisionPars(void);

	/// @brief タグに基づいた衝突可能判定
	/// @param _tagA 判定Aのタグ
	/// @param _tagB 判定Bのタグ
	/// @return 衝突する組み合わせならtrue
	bool CanCollide(int _tagA, int _tagB) const;

	/// @brief 2Dコライダーの総当たり判定更新
	void UpdateCollision2D(void);

	/// @brief タグの組み合わせによる衝突判定可否の確認
	/// @param _tagA 1つ目の2Dタグ
	/// @param _tagB 2つ目の2Dタグ
	/// @return 衝突許可フラグ
	bool CanCollide2D(Collider2DBase::TAG_2D _tagA, Collider2DBase::TAG_2D _tagB) const;
};