#pragma once
#include "BBulletBase.h"

/// @brief ボス用ミサイル弾クラス
class BBulletMissile : public BBulletBase
{
public:

	BBulletMissile(void);

	~BBulletMissile(void) override;

	/// @brief リソースの読み込み
	void Load(void) override;

	/// @brief 解放後の後処理
	void ReleasePost(void) override;

	/// @brief 攻撃状態を設定する
	/// @param _isAttack true: 攻撃中 / false: 非攻撃中
	void SetIsAttack(bool _isAttack) override { isAttack_ = _isAttack; }

	/// @brief 座標を設定する
	/// @param _pos 設定座標
	void SetPos(VECTOR _pos) {};

	/// @brief 弾の生成と初期パラメータ設定
	/// @param _pos 発射座標
	/// @param _dir 発射方向
	/// @param _radius 当たり判定半径
	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius) override { transform_.pos = _pos; dir_ = _dir; radius_ = _radius; }

	/// @brief 発射元トランスフォームの設定
	/// @param _trans 発射元のトランスフォーム
	void SetTransform(const Transform& _trans) override { weaponTrans_ = _trans; }

	/// @brief プレイヤー座標の設定
	/// @param _pos プレイヤー座標
	void SetPlayerPos(VECTOR _pos) override { playerPos_ = _pos; }

	/// @brief 上限高度の設定
	/// @param _pos 上限高度
	void SetUpMaxPos(float _pos) override { maxPos_ = _pos; }

protected:

	/// @brief トランスフォームの初期化
	void InitTransform(void) override;

	/// @brief コライダーの初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理
	void InitPost(void) override;

	/// @brief フレーム毎の更新処理
	void UpdateProcess(void) override;

	/// @brief 更新処理後の個別処理
	void UpdateProcessPost(void) override;

	/// @brief 描画処理
	void DrawPre(void) override;

private:

	// 影・警告範囲に関する定数
	static constexpr float SHADOW_POS_Y = -7.0f + 0.5f;						// 影の描画高さ座標
	static constexpr int MAX_SHADOW_COL = 255;								// 影の濃さ最大値
	static constexpr float SHADOW_FADE_HEIGHT = 3000.0f;					// 影を完全に消す高度
	static constexpr int ALERT_ALPHA = 125;									// 警告エリア描画の不透明度（0～255）

	// 移動・攻撃制御に関する定数
	static constexpr float MOVE_UP_SPEED = 20.0f;							// 上昇速度
	static constexpr float MOVE_DOWN_SPEED = 30.0f;							// 降下速度
	static constexpr float SPAWN_HEIGHT_Y = 3000.0f;						// プレイヤー頭上にワープした際のアプローチ高度
	static constexpr float GROUND_POS_Y = -7.0f;							// 着地地上高度
	static constexpr int MAX_ATTACK_COUNT = 40;								// 攻撃判定持続フレーム数
	static constexpr VECTOR MISSILE_SCALE = { 0.07f, 0.07f, 0.07f };		// ミサイルモデルのスケール
	static constexpr VECTOR COLLIDER_OFFSET = { 0.0f, 0.0f, 0.0f };		// コライダーのローカルオフセット

	// エフェクト演出に関する定数
	static constexpr VECTOR EFFECT_ZERO_ROT = { 0.0f, 0.0f, 0.0f };			// エフェクトの回転オフセット無し
	static constexpr VECTOR MISSILE_EFFECT_SCALE = { 90.0f, 90.0f, 90.0f };// 着弾エフェクトのスケール
	static constexpr float MISSILE_EFFECT_SPEED = 20.0f;					// 着弾エフェクトの再生速度

	// コライダーの半径倍率
	static constexpr float ATTACK_COLLIDER_RADIUS_RATE = 0.8f;				// 攻撃判定コライダー半径の倍率
	static constexpr float PUSH_COLLIDER_RADIUS_RATE = 1.1f;				// 押し出し判定コライダー半径の倍率

	// 頂点番号の定数
	static constexpr int LEFT_BACK = 0;										// 左後頂点インデックス
	static constexpr int LEFT_FORWARD = 1;									// 左前頂点インデックス
	static constexpr int RIGHT_BACK = 2;									// 右後頂点インデックス
	static constexpr int RIGHT_FORWARD = 3;									// 右前頂点インデックス

	VERTEX3D imageVertex_[4];												// ポリゴン描画用頂点配列
	int fallingHandle_;														// 落下地点警告画像ハンドル
	bool isUp_;																// 上昇中フラグ
	bool isAttack_;															// 攻撃判定発生中フラグ
	VECTOR playerPos_;														// プレイヤー座標
	int attackCount_;														// 攻撃持続フレームカウンタ
	float maxPos_;															// 上昇限界高度

	/// @brief ミサイルの上昇移動処理
	void MoveUp(void);

	/// @brief ミサイルの降下移動処理
	void MoveDown(void);

	/// @brief 攻撃判定持続処理
	void Attack(void);

	/// @brief 落下予定地点の警告エリア（影）描画
	void DrawAreaAlert(void);
};