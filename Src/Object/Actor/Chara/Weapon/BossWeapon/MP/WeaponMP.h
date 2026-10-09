#pragma once
#include "../../WeaponBase.h"
#include <algorithm>
#include <vector>

class BBulletBase;


class WeaponMP : public WeaponBase
{
public:

	WeaponMP(void);
	~WeaponMP(void) override = default;

	/// @brief 武器モデル・SE等のリソース読み込み処理
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief ボーン情報およびターゲット情報の受け取り
	/// @param _id 接続ボーンの番号
	/// @param _trans 接続ボーンを持つ対象のトランスフォーム
	/// @param _tag 当たり判定登録用タグ
	/// @param _playerPos ターゲット（プレイヤー）の座標
	void SetBone(int _id, const Transform& _trans, ColliderBase::TAG _tag, const VECTOR& _playerPos) override;

	/// @brief 現在のワールド座標を取得する
	/// @return 現在のワールド座標
	const VECTOR GetPos(void) const override;

	/// @brief ダメージを受けてHPを減らす
	/// @param _damage 受けたダメージ量
	void SetDamage(int _damage) override { hp_ -= _damage; }

	/// @brief 生存状態を取得する
	/// @return true: 生きている / false: 死亡
	bool GetIsAlive(void) override { return isAlive_; }

	/// @brief 現在のHPを取得する
	/// @return 現在のHP
	int GetHp(void) override { return hp_; }

	/// @brief HPを設定する
	/// @param _hp 設定するHP量
	void SetHp(int _hp) override { hp_ = _hp; }

	/// @brief 状態（ステート）を変更する
	/// @param _state 遷移先のステート
	void ChangeState(STATE _state) override;

	/// @brief 右側か左側かを指定するフラグを設定する
	/// @param _isLR true: L（左） / false: R（右）
	void IsLR(bool _isLR) { isLR_ = _isLR; }

protected:

	/// @brief トランスフォーム（大きさ・回転・座標）の初期化
	void InitTransform(void) override;

	/// @brief 衝突判定の初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理（ステート設定等）
	void InitPost(void) override;

	/// @brief フレーム毎の更新処理
	void UpdateProcess(void) override;

	/// @brief 更新後の個別処理
	void UpdateProcessPost(void) override;

	/// @brief 描画前処理（発射中の弾の描画など）
	void DrawPre(void) override;

	/// @brief 衝突予約処理
	void CollisionReserve(void) override {};

	/// @brief プレイヤーの方向を向く処理
	void LookPlayer(void) override;

	/// @brief 弾を生成・発射する処理
	virtual void CreateBullets(void);

	/// @brief 使用可能な非アクティブ弾を取得（無ければ新規生成）
	/// @return 弾のスマートポインタ
	std::shared_ptr<BBulletBase> GetValidBullet(void);

	// ステート管理用
	std::function<void(void)> stateUpdate_;		// 現在ステートの更新関数ポインタ
	/// @param _state 遷移先のステート値
	void ChangeState(int _state) override;

	/// @brief 待機ステート開始処理
	void ChangeStateIdle(void) override;

	/// @brief 攻撃ステート開始処理
	void ChangeStateAttack(void) override;

	/// @brief 死亡・破壊ステート開始処理
	void ChangeStateEnd(void) override;

	/// @brief 待機ステートの毎フレーム更新
	void UpdateIdle(void) override;

	/// @brief 攻撃ステートの毎フレーム更新
	void UpdateAttack(void) override;

	/// @brief 死亡・破壊ステートの毎フレーム更新
	void UpdateEnd(void) override;

private:

	static constexpr VECTOR LINE_START_POS = { 0.0f, 0.0f, -40.0f };		// 線分コライダーの始点ローカル座標
	static constexpr VECTOR LINE_END_POS = { 0.0f, -10.0f, -40.0f };		// 線分コライダーの終点ローカル座標
	static constexpr VECTOR SPHERE_START_POS = { 0.0f, 0.0f, -40.0f };		// 球コライダーの始点ローカル座標
	static constexpr float SPHERE_RADIUS = 40.0f;							// 球コライダーの半径

	static constexpr int MUZZLE_MAX_COUNT = 6;								// 発射口の総数
	static constexpr int MAX_ATTACK_COUNT = 3;								// 1回の攻撃での発射回数
	static constexpr int ATTACK_DELAY = 5;									// 発射間隔のディレイフレーム数

	static constexpr float MIN_FALL_POS = 3000.0f;							// 弾が下降開始する最小高度
	static constexpr float UP_FALL_POS = 1000.0f;							// 銃口ごとの下降高度の加算値
	static constexpr float ATTACK_RADIUS = 400.0f;							// 弾の当たり判定サイズ

	static constexpr int HALF_MUZZLE_DIVISOR = 2;							// 銃口インデックス循環用の除数
	static constexpr int INDEX_STEP = 2;									// 高度計算時のステップ倍率
	static constexpr int EVEN_OFFSET = 0;									// 左（L）配置時のステップオフセット
	static constexpr int ODD_OFFSET = 1;									// 右（R）配置時のステップオフセット

	// 各発射口のローカルオフセット座標リスト
	static constexpr VECTOR MUZZLE_POS[MUZZLE_MAX_COUNT] = {
		{ 9.0f, 23.0f, -15.5f },
		{ 9.0f, 23.0f, -33.0f },
		{ 9.0f, 24.0f, -51.0f },
		{ -9.0f, 23.0f, -15.5f },
		{ -9.0f, 23.0f, -33.0f },
		{ -9.0f, 24.0f, -51.0f },
	};

	int attackCount_;														// 攻撃実行回数カウンタ
	int outCount_;															// 発射ディレイ用インターバルカウンタ
	VECTOR bulletDir_;														// 弾の発射方向ベクトル
	VECTOR muzzlePos_[MUZZLE_MAX_COUNT];									// 発射口のローカル座標配列
	int muzzleCount_;														// 現在選択中の発射口インデックス
	bool isLR_;																// 左右識別フラグ（true: L / false: R）

	std::vector<std::shared_ptr<BBulletBase>> bullets_;					// 弾オブジェクトの管理プール
};