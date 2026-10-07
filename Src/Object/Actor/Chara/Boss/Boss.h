#pragma once
#include <DxLib.h>
#include <memory>
#include <array>
#include <functional>
#include "../CharaBase.h"
#include "../../../Common/Transform.h";
#include "../../../../Net/NetStructures.h"

class WeaponMGL;
class WeaponMGR;
class WeaponCannon;
class WeaponMP;
class WeaponRG;
class BBulletWave;

class Boss : public CharaBase
{
public:
	///ステートパターン
	enum class STATE
	{
		IDLE,
		ATTACK,
		JUMP,
		JUMPBEFORE,
		ROADATTACK,
		LASER,
		END,
	};

	///攻撃パターン
	enum class ATTACK_TYPE
	{
		JUMP,
		MG,
		ROAD,
		CANNON,
		MISSILE,
		LASER,
		MAX,
	};

	///ウェポンの接続ボーンの名前
	enum class BONE_NAME
	{
		WEAPON_JOINT_MGL_L = 0,
		WEAPON_JOINT_MGL_R,
		WEAPON_JOINT_CANNON_L,
		WEAPON_JOINT_CANNON_R,
		WEAPON_JOINT_MP_L,
		WEAPON_JOINT_MP_R,
		WEAPON_JOINT_RG,
		MAX,
	};

	///アニメーションパターン
	enum class ANIM_TYPE
	{
		ATTACK,
		DIR,
		JUMP,
		JUMPBEFORE,
		MAX,
	};

	///各ボーンの情報
	struct Bone {
		int id = 0;
		Transform transform;

	};

	Boss(void);
	~Boss(void) override;

	// リソースロード

	/// @brief ボスのリソースをロードする
	void Load(void) override;

	// ゲット・セット

	/// @brief ボスの現在座標を取得する
	/// @return ボスの現在座標
	const VECTOR& GetBossPos(void) const { return transform_.pos; }

	/// @brief ボスの音が聞こえる範囲を取得する
	/// @return 音が聞こえる範囲
	const float& GetSoundRadius(void) const { return soundRadius_; }

	/// @brief 着地音を鳴らすかどうかを取得する
	/// @return 着地音を鳴らすかどうかのフラグ
	const bool& GetLandingFlag(void) const { return isLanging_; }

	/// @brief MGの発射音を鳴らすかどうかを取得する
	/// @return MGの発射音を鳴らすかどうかのフラグ
	const bool& GetMGFireFlag(void) const { return isMGFire_; }

	/// @brief 走行音を鳴らすかどうかを取得する
	/// @return 走行音を鳴らすかどうかのフラグ
	const bool& GetRoadFlag(void) const { return isRoadFire_; }


	// 各武器のダメージ受け取り用関数

	// ガトリング

	/// @brief 左ガトリングのダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetWeaponMGLDamage(int _damage);

	/// @brief 右ガトリングのダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetWeaponMGRDamage(int _damage);


	// ミサイルポッド

	/// @brief 左ミサイルポッドのダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetWeaponMPLDamage(int _damage);

	/// @brief 右ミサイルポッドのダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetWeaponMPRDamage(int _damage);


	// キャノン

	/// @brief 左キャノンのダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetWeaponCannonLDamage(int _damage);

	/// @brief 右キャノンのダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetWeaponCannonRDamage(int _damage);


	// レールガン

	/// @brief レールガンのダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetWeaponRGDamage(int _damage);


	// ボス本体へのダメージ受け取り用

	/// @brief ボス本体のダメージを設定する
	/// @param _damage 受けたダメージ量
	void SetBossDamage(int _damage);

	/// @brief ボスの現在HPを取得する
	/// @return ボスの現在HP
	int GetHP(void) const { return hp_; }

	/// @brief ボス本体のトランスフォームを取得する
	/// @return ボス本体のトランスフォーム
	Transform& GetBodyTransform(void) { return transformBody_; }


	// プレイヤー座標

	/// @brief プレイヤー1の座標を設定する
	/// @param _playerPos プレイヤー1の座標
	void SetPlayer1Pos(VECTOR _playerPos) { playerPos_[0] = _playerPos; }

	/// @brief プレイヤー2の座標を設定する
	/// @param _playerPos プレイヤー2の座標
	void SetPlayer2Pos(VECTOR _playerPos) { playerPos_[1] = _playerPos; }

	/// @brief プレイヤー3の座標を設定する
	/// @param _playerPos プレイヤー3の座標
	void SetPlayer3Pos(VECTOR _playerPos) { playerPos_[2] = _playerPos; }

	/// @brief プレイヤー4の座標を設定する
	/// @param _playerPos プレイヤー4の座標
	void SetPlayer4Pos(VECTOR _playerPos) { playerPos_[3] = _playerPos; }

	/// @brief プレイヤーの人数を設定する
	/// @param _size プレイヤーの人数
	void SetPlayerSize(int _size) { playerSize_ = _size; }


	// ホスト制御

	/// @brief ホスト制御かどうかを設定する
	/// @param _isHostControl ホスト制御かどうかのフラグ
	void SetHostControl(bool _isHostControl);

	/// @brief ホスト制御かどうかを取得する
	/// @return ホスト制御かどうかのフラグ
	bool GetHostControl(void) const { return isHostControl_; }


	// ネットワーク

	/// @brief 現在のボスのネットワークアクション情報を取得する
	/// @return ネットワーク用ボスアクションデータ
	NET_BOSS_ACTION GetNetworkAction(void) const;

	/// @brief ネットワークからのアクション情報をボスに適用する
	/// @param _action 受信したネットワーク用ボスアクションデータ
	void SetNetworkAction(const NET_BOSS_ACTION& _action);


	// メインターゲット

	/// @brief メインターゲットの座標を取得する
	/// @return メインターゲットの座標
	VECTOR GetMainTargetPos(void) { return mainPos_; }

	/// @brief メインターゲットの座標を設定する
	/// @param _pos メインターゲットの座標
	void SetMainTargetPos(VECTOR _pos) { mainPos_ = _pos; }

private:
	//汎用
	static constexpr int HALF = 2;							//2分の1を表す値

	//bossの大きさ
	static constexpr VECTOR BOSS_SIZE = { 3.0f, 3.0f, 3.0f };				//ボス本体の大きさ
	static constexpr VECTOR BOSS_CAR_SIZE = { 5.0f, 5.0f, 5.0f };			//車形態の大きさ

	//bossの初期座標
	static constexpr VECTOR BOSS_INIT_POS = { 500.0f, 0.0f, 200.0f };		//ボスの初期位置

	//回転
	static constexpr float INIT_ROT = 180.0f;				//ボスの初期回転角度

	//攻撃
	static constexpr float FIRST_ATTACK_INTERVAL = 580;		//初回攻撃までの待機時間
	static constexpr float MAX_ATTACK_INTERVAL = 600;		//攻撃間隔の最大値
	static constexpr float DOUN_ATTACK_INTERVAL = 50;		//攻撃後の待機時間
	static constexpr int INTERVAL_SEC = 20;					//攻撃対象変更の間隔
	static constexpr int INTERVAL_SEC_MP = 4;				//MP攻撃の間隔
	static constexpr int INTERVAL_SEC_CANNON = 10;			//キャノン攻撃の間隔
	static constexpr float FIRST_LASER_ROT_SPEED = 5.0f;	//レーザー初回回転速度
	static constexpr float LASER_ROT_SPEED = 2.0f;			//レーザー回転速度
	static constexpr float LASER_END = 0.2f;				//レーザー終了判定値
	static constexpr float LASER_MAX_ROT = 360.0f;			//レーザーの最大回転角度

	//音
	static constexpr float SOUND_RADIUS = 2000.0f;			//音が聞こえる範囲

	//エフェクト
	static constexpr float EFFECT_PLAEY_DAMEGE = 5.0f;			//プレイヤーへのエフェクトダメージ
	static constexpr VECTOR EFFECT_SCL = { 17.5f,17.5f,17.5f };	//通常エフェクトの大きさ
	static constexpr VECTOR EFFECT_SCL_LASER = { 35,35,35 };	//レーザーエフェクトの大きさ
	static constexpr float EFFECT_PLAEY_SPEED = 1.0f;			//エフェクトの再生速度
	static constexpr VECTOR EFFECT_ROT = { 90.0f,0.0f,0.0f };	//エフェクトの回転
	static constexpr int EFFECT_NO_ZERO = 0;					//エフェクト番号0
	static constexpr int EFFECT_NO_ONE = 1;						//エフェクト番号1
	static constexpr int EFFECT_NO_TWO = 2;						//エフェクト番号2
	static constexpr int EFFECT_NO_THREE = 3;					//エフェクト番号3

	//アニメーション
	static constexpr float ANIM_SPEED = 20.0f;				//アニメーション再生速度

	//HP
	static constexpr int MAX_HP = 2000;						//ボスの最大HP
	static constexpr float MAX_HP_HALF = MAX_HP / 2.0f;		//ボスHPの半分
	static constexpr float WEAPON_HP_CANNON = 0.5f;			//キャノンのダメージ倍率
	static constexpr float WEAPON_HP_MP = 0.7f;				//MPのダメージ倍率
	static constexpr float WEAPON_HP_MG = 0.8f;				//MGのダメージ倍率

	//ジャンプ力
	static constexpr float POW_JUMP_INIT = 3000.0f;					//ジャンプ初速
	static constexpr float JUMP_MAX_UP = POW_JUMP_INIT + 500.0f;	//ジャンプ上昇力の最大値
	static constexpr float MOVE_SPEED_INIT = 20.0f;					//ジャンプ時の初期移動速度
	static constexpr float POW_JUMP_DOUN = -50.0f;					//落下時の加速度
	static constexpr VECTOR WAVE_SCL = { 1.0f,50.0f,1.0f };			//ジャンプ波エフェクトの大きさ
	static constexpr VECTOR WAVE_SCL_UP = { 4.0f,0.0f,4.0f };		//上昇波エフェクトの大きさ
	static constexpr VECTOR LANDING_SCL = { 100.0f,50.0f,100.0f };	//着地エフェクトの大きさ

	//ロードアッタク
	static constexpr float WHEEL_ROT = 10.0f;				//突進時の車輪回転速度
	static constexpr float MOVE_SPEED_ROAD = 20.0f;			//突進時の移動速度
	static constexpr int MAX_ROAD_ATTACK_TIME = 80;			//突進攻撃の最大時間
	static constexpr int MAX_ROAD_LOCK_TIME = 30;			//突進時のロックオン時間
	static constexpr int MAX_ROAD_COUNT = 3;				//突進攻撃の最大回数

	//当たり判定の座標
	//ライン
	static constexpr VECTOR COL_LINE_START_POS = { 0.0f,60.0f,0.0f };	//ライン判定の始点
	static constexpr VECTOR COL_LINE_END_POS = { 0.0f,-1.0f,0.0f };		//ライン判定の終点

	//カプセル
	static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f,130.0f,0.0f };	//カプセル判定の始点
	static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f,80.0f,0.0f };		//カプセル判定の終点
	static constexpr float COL_CAPSULE_END_RADIUS = 80.0f;					//カプセル判定の半径

	//ボーンの番号
	static constexpr int JOINT_FEET_BODY = 12;								//足本体のボーン番号
	static constexpr int JOINT_CAR_BODY = 4;								//車体のボーン番号
	static constexpr int JOINT_CAR_WHEEL_FRONT_L = 6;						//前輪左のボーン番号
	static constexpr int JOINT_CAR_WHEEL_FRONT_R = JOINT_FEET_BODY;			//前輪右のボーン番号
	static constexpr int JOINT_CAR_WHEEL_BACK_FRONT_L = 8;					//後方前側左のボーン番号
	static constexpr int JOINT_CAR_WHEEL_BACK_FRONT_R = 16;					//後方前側右のボーン番号
	static constexpr int JOINT_CAR_WHEEL_BACK_L = 10;						//後輪左のボーン番号
	static constexpr int JOINT_CAR_WHEEL_BACK_R = 14;						//後輪右のボーン番号
	static constexpr int JOINT_WAEAPON_MG_L = JOINT_CAR_BODY;				//左MGのボーン番号
	static constexpr int JOINT_WAEAPON_MG_R = 10;							//右MGのボーン番号
	static constexpr int JOINT_WAEAPON_CANNON_L = 6;						//左キャノンのボーン番号
	static constexpr int JOINT_WAEAPON_CANNON_R = JOINT_FEET_BODY;			//右キャノンのボーン番号
	static constexpr int JOINT_WAEAPON_MP_L = JOINT_CAR_WHEEL_BACK_FRONT_L;	//左MPのボーン番号
	static constexpr int JOINT_WAEAPON_MP_R = JOINT_CAR_WHEEL_BACK_R;		//右MPのボーン番号
	static constexpr int JOINT_WAEAPON_RG = JOINT_CAR_WHEEL_BACK_FRONT_R;	//RGのボーン番号

	//復帰
	static constexpr int DOWU_POS = -50;						//落下時の復帰判定位置
	static constexpr VECTOR POP_POS = { 0, 2000,0 };			//復帰時の位置

	//死亡時
	static constexpr int END_MAX_COUNT = 4;					//死亡演出の最大カウント
	static constexpr int END_COUNT = 3;						//死亡演出の終了カウント
	static constexpr int MOVE_SPEED = 30;					//死亡時の移動速度


	//ボス本体の各トランスフォーム
	Transform transformFeet_;					//足部分のトランスフォーム
	Transform transformBody_;					//体部分のトランスフォーム
	Transform transformFeetCar_;				//車体のトランスフォーム
	Transform transformWheelBackL_;				//後輪左のトランスフォーム
	Transform transformWheelBackFrontL_;		//後方前側左輪のトランスフォーム
	Transform transformWheelFrontL_;			//前輪左のトランスフォーム
	Transform transformWheelBackR_;				//後輪右のトランスフォーム
	Transform transformWheelBackFrontR_;		//後方前側右輪のトランスフォーム
	Transform transformWheelFrontR_;			//前輪右のトランスフォーム

	//ステータス
	int hp_;								//HP
	std::array<Bone, 7> boneId_;			//各ボーン
	BONE_NAME boneName_;					//ボーンの名前
	VECTOR wallStopPos_;					//壁に接触した位置
	VECTOR jumpDir_;						//ジャンプ中の移動方向
	VECTOR roadDir_;						//体当たり中の移動方向
	float speed_;							//移動スピード
	int roadCount_;							//体当たりの回数
	int roadAttackTime_;					//体当たりの突進時間
	int roadLockTime_;						//体当たりのロックオン時間
	bool roadIsAttack_;						//体当たり攻撃中かのフラグ
	float soundRadius_;						//音の聞こえる範囲
	bool isLanging_;						//着地音を鳴らすかのフラグ
	bool isMGFire_;							//MG発射音を鳴らすかのフラグ
	bool isRoadFire_;						//走行音を鳴らすかのフラグ

	//攻撃関連
	int jumpCount_;							//ジャンプ回数
	int attackCount_;						//攻撃回数
	int attackInterval_;					//攻撃間隔
	VECTOR currentWaveScl;					//現在の波エフェクトサイズ
	float laserAttackRot_;					//レーザー攻撃の回転角度
	float laserShotHp_;						//レーザー発射時のHP
	float laserRotSpeed_;					//レーザーの回転速度
	ATTACK_TYPE lastAttackType_;			//前回の攻撃種類
	ATTACK_TYPE attackSelect_;				//現在選択中の攻撃種類

	//攻撃対象情報
	VECTOR mainPos_;						//メインターゲットの位置
	int mainIdx_;							//メインターゲットの番号
	int nextChangeMainTime_;				//メインターゲット変更までの時間
	VECTOR mpPos_;							//MPターゲットの位置
	int mpIdx_;								//MPターゲットの番号
	int nextChangeMpTime_;					//MPターゲット変更までの時間
	VECTOR CannonPos_;						//キャノンターゲットの位置
	int cannonIdx_;							//キャノンターゲットの番号
	int nextChangeCannonTime_;				//キャノンターゲット変更までの時間
	VECTOR playerPos_[4];					//各プレイヤーの位置
	int playerSize_;						//プレイヤー人数

	//武器のポインター宣言
	std::unique_ptr<WeaponMGL> weaponMGL_;			//左MGの武器ポインター
	std::unique_ptr<WeaponMGR> weaponMGR_;			//右MGの武器ポインター
	std::unique_ptr<WeaponCannon> weaponCannonL_;	//左キャノンの武器ポインター
	std::unique_ptr<WeaponCannon> weaponCannonR_;	//右キャノンの武器ポインター
	std::unique_ptr<WeaponMP> weaponMPL_;			//左MPの武器ポインター
	std::unique_ptr<WeaponMP> weaponMPR_;			//右MPの武器ポインター
	std::unique_ptr<WeaponRG> weaponRG_;			//RGの武器ポインター
	std::unique_ptr<BBulletWave> wave_;				//波攻撃のポインター

	// ホストかどうか	
	bool isHostControl_;							//ホストが操作を担当するかのフラグ

	//死亡時
	VECTOR bodyDir_;								//死亡時の移動方向
	float moveSpeed_;								//死亡時の移動速度
	int endCount_;									//死亡演出のカウント
	VECTOR cameraPos_;								//死亡演出時のカメラ位置

	//ボーン初期化
	void BoneParam(void);							//ボーン情報を初期化

	//ボーンアプデ
	void BossTransformUpdate(void);					//ボーンのトランスフォームを更新

	// 状態
	STATE state_;									//現在の状態
	// 状態管理
	int stateBase_;									//状態の基準値

	// 状態管理
	std::map<int, std::function<void(void)>> stateChanges_;	//状態遷移時の初期処理を管理する

	/// @brief ボスの状態を変更する
	/// @param _state 変更する状態
	void ChangeState(STATE _state);

	/// @brief ボスの状態を変更する
	/// @param state 変更する状態番号
	void ChangeState(int state);

	/// @brief 待機状態へ遷移する
	void ChangeStateIdle(void);

	/// @brief 攻撃状態へ遷移する
	void ChangeStateAttack(void);

	/// @brief ジャンプ状態へ遷移する
	void ChangeStateJump(void);

	/// @brief ジャンプ前状態へ遷移する
	void ChangeStateJumpBefore(void);

	/// @brief 突進攻撃状態へ遷移する
	void ChangeStateRoadAttack(void);

	/// @brief レーザー攻撃状態へ遷移する
	void ChangeStateLaserAttack(void);

	/// @brief 終了状態へ遷移する
	void ChangeStateEnd(void);


	// 更新系
	// 状態管理
	std::function<void(void)> stateUpdate_;	//現在の状態の更新処理を管理する

	/// @brief 待機状態を更新する
	void UpdateIdle(void);

	/// @brief 攻撃状態を更新する
	void UpdateAttack(void);

	/// @brief ジャンプ状態を更新する
	void UpdateJump(void);

	/// @brief ジャンプ前状態を更新する
	void UpdateJumpBefore(void);

	/// @brief 突進攻撃状態を更新する
	void UpdateRoadAttack(void);

	/// @brief レーザー攻撃状態を更新する
	void UpdateStateLaserAttack(void);

	/// @brief 終了状態を更新する
	void UpdateEnd(void);

	/// @brief エフェクトを更新する
	void UpdateEffect(void);

	/// @brief タイヤの情報を更新する
	void UpdateWheel(void);

	/// @brief サウンドの情報を更新する
	void UpdateSound(void);


	// 機能関数
	/// @brief プレイヤーの方向を向く
	void LookPlayer(void);
	/// @brief ターゲットの選定
	void SelectTarget(void);



	// Effect
	/// @brief エフェクトを再生する
	void PlayEffect(void);

	



protected:
	// 初期化
	/// @brief 大きさ、回転、座標を初期化する
	void InitTransform(void) override;

	/// @brief 衝突判定を初期化する
	void InitCollider(void) override;

	/// @brief アニメーションを初期化する
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理を行う
	void InitPost(void) override;

	// 更新
	/// @brief ボスの更新処理を行う
	void UpdateProcess(void) override;

	/// @brief 更新処理後の個別処理を行う
	void UpdateProcessPost(void) override;

	// 描画
	/// @brief 描画前の処理を行う
	void DrawPre(void) override;

	// 解放
	/// @brief リソース解放後の個別処理を行う
	void ReleasePost(void) override;

	// 衝突判定
	/// @brief 衝突判定の予約処理を行う
	void CollisionReserve(void) override {};

	// 武器関連
	/// @brief 武器のセット処理をまとめて呼び出す
	void WeaponSet(void);

	/// @brief 武器のロード処理をまとめて呼び出す
	void WeaponLoad(void);

	/// @brief 武器の初期化処理をまとめて呼び出す
	void WeaponInit(void);

	/// @brief 武器の更新処理をまとめて呼び出す
	void WeaponUpdate(void);

	/// @brief 武器の描画処理をまとめて呼び出す
	void WeaponDraw(void);
};

