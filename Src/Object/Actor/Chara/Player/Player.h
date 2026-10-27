#pragma once
#include "./PlayerBase.h"
#include "./PActionController.h"
#include <DxLib.h>
#include <array>

class Player : public PlayerBase
{
public:

	// 吹っ飛ばし量
	static constexpr float KNOCK_POW_Y = 3.75f;

	// 連射弾発射速度
	static constexpr float RAPID_SHOT_SPEED_XZ = 17.5f;
	static constexpr float RAPID_SHOT_SPEED_Y = 2.5f;

	// 拡散弾発射速度
	static constexpr float CLUSTER_SHOT_SPEED = 20.0f;

	/// @brief アニメーションの種類
	enum class ANIMATION_TYPE
	{
		NONE = -1, // 未割当値

		IDLE,        // 待機
		WALK,        // 歩行
		THROW_LEFT,  // 左投擲
		THROW_RIGHT, // 右投擲
		JUMP,        // ジャンプ
		DODGE,       // 回避
		DEFEAT,      // 撃破
	};

	/// @brief 行動の種類
	enum class ACTION_TYPE
	{
		NONE = -1, // 未割当値

		JUMP,           // ジャンプ
		DODGE,          // 回避
		DEFEAT,         // 撃破
		ATTACK_SPECIAL, // 特殊攻撃
		ATTACK,         // 通常攻撃
	};

	/// @brief コンストラクタ
	/// @param _playerNo プレイヤー番号
	/// @param _jobType 攻撃の種類
	/// @param _skinType 見た目の種類
	/// @param _startPos 開始位置
	Player(int _playerNo, JOB_TYPE _jobType, SKIN_TYPE _skinType, const VECTOR& _startPos);

	/// @brief デストラクタ
	~Player(void)override;

	/// @brief リソース読み込み処理
	void Load(void)override;

	/// @brief 吹っ飛ばし処理
	/// @param _knockDirXZ 横吹っ飛ばし方向
	/// @param _knockPowXZ 横吹っ飛ばし力
	/// @param _isStan スタンさせるか否か 
	/// @param _knockPowY 縦吹っ飛ばし力
	void SetKnock(const VECTOR& _knockDirXZ, float _knockPowXZ
		, bool _isStan, float _knockPowY = KNOCK_POW_Y);
	
	/// @brief リスポーンしたか否か
	bool IsRespawn(void)const;

	/// @brief 自分が操作するキャラクターかどうかを取得する
	bool GetHostControl(void) const { return isHostControl_; }

	/// @brief ネットワークから受け取った情報を強制的にセットする(ラジコン用)
	/// @param _pos 現在座標
	/// @param _rot 現在回転量
	/// @param _animationId 現在アニメーションID
	/// @param _isAttack 攻撃中かの判定
	/// @param _currentHp 現在HP
	void SetNetworkAction(const VECTOR& _pos, const Quaternion& _rot, int _animationId, bool _isAttack, int _currentHp);

	/// @brief 自分が操作するキャラクターか設定
	void SetHostControl(bool _isLocal) { isHostControl_ = _isLocal; };

	/// @brief キャラクターの通信キーを設定
	void SetNetKey(int _key) { netKey_ = _key; };

	/// @brief キャラクターの通信キーを取得
	int GetNetKey(void) const { return netKey_; }


protected:

	/// @brief 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	/// @brief 衝突判定の初期化
	void InitCollider(void) override;

	/// @brief アニメーションの初期化
	void InitAnimation(void) override;

	/// @brief 初期化後の個別処理
	void InitPost(void) override;

	/// @brief 更新処理
	void UpdateProcess(void);
	void UpdateProcessPost(void) {};

	/// @brief 描画処理
	void DrawPre(void)override;
	void DrawLate(void)override;

	/// @brief 各オブジェクトのメモリ解放
	void ReleasePost(void)override;

	/// @brief 位置に加算処理
	VECTOR CalcAddPosition(void) override;


private:

	// モデルのパラメータ
	static constexpr float MODEL_SCALE  = 0.625f; // スケール
	static constexpr float LOCAL_POS_Y  = 5.25f;  // ローカル縦位置
	static constexpr float LOCAL_ROT_Y  = 180.0f; // ローカルY軸回転
	static constexpr float SHADOW_SCALE = 30.0f;  // 影の半径

	// 連射パラメータ
	static constexpr float RAPID_SCALE      = 0.85f; // スケール
	static constexpr float RAPID_RADIUS     = 10.0f; // 当たり判定半径
	static constexpr int   RAPID_POWER      = 30;    // 攻撃力
	static constexpr float RAPID_ALIVE_TIME = 2.5f;  // 生存時間

	// 拡散弾パラメータ
	static constexpr float CLUSTER_SCALE      = 0.25f; // スケール
	static constexpr float CLUSTER_RADIUS     = 5.0f;  // 当たり判定半径
	static constexpr int   CLUSTER_POWER      = 30;    // 攻撃力
	static constexpr float CLUSTER_ALIVE_TIME = 0.25f; // 生存時間

	// ジャンプ力
	static constexpr float JUMP_POW = 5.0f;

	// 回避力
	static constexpr float DODGE_POW = 10.0f;

	// 回避時間
	static constexpr float TIME_DODGE = 0.5f;

	// 回避クールタイム
	static constexpr float TIME_WAIT_DODGE = 1.5f;

	// 移動速度
	static constexpr float MOVE_SPEED = 8.5f;

	// 攻撃中の移動速度
	static constexpr float MOVE_SPEED_SHOT = (MOVE_SPEED * 0.3f);

	// 連射の発射間隔
	static constexpr float SHOT_RAPID_TERM = 0.025f;

	// アニメーション速度
	static constexpr float ANIMATION_SPEED_IDLE   = 30.0f; // 待機
	static constexpr float ANIMATION_SPEED_WALK   = 55.0f; // 移動
	static constexpr float ANIMATION_SPEED_JUMP   = 50.0f; // ジャンプ
	static constexpr float ANIMATION_SPEED_DODGE  = 55.0f; // 回避
	static constexpr float ANIMATION_SPEED_DEFEAT = 30.0f; // 撃破
	static constexpr float ANIMATION_SPEED_THROW_BIG   = 20.0f; // 巨大弾発射
	static constexpr float ANIMATION_SPEED_THROW_RAPID = 75.0f; // 連射弾発射
	static constexpr float ANIMATION_SPEED_THROW_BOMB  = 35.0f; // 爆発弾発射

	// 行動有効の時間
	static constexpr float JUMP_TOTAL_ACTIVE         = 0.2f;  // ジャンプ
	static constexpr float DODGE_TOTAL_ACTIVE        = 0.5f;  // 回避
	static constexpr float DEFEAT_TOTAL_ACTIVE       = 1.0f;  // 撃墜
	static constexpr float SHOT_BOMB_TOTAL_ACTIVE    = 1.0f;  // 爆発弾発射
	static constexpr float SHOT_CANNON_TOTAL_ACTIVE  = 1.26f; // 巨大弾発射
	static constexpr float SHOT_RAPID_TOTAL_ACTIVE   = 0.5f;  // 連射弾発射
	static constexpr float SHOT_CLUSTER_TOTAL_ACTIVE = 0.5f;  // 拡散弾発射
	static constexpr float SHOT_SUPPORT_TOTAL_ACTIVE = 1.0f;  // サポート職の発射

	// 行動有効のタイミング
	static constexpr float JUMP_TIMING_ACTION         = 0.1f;  // ジャンプ
	static constexpr float DODGE_TIMING_ACTION        = 0.1f;  // 回避
	static constexpr float DEFEAT_TIMING_ACTION       = 0.01f; // 撃墜
	static constexpr float SHOT_BOMB_TIMING_ACTION    = 0.75f; // 爆発弾発射
	static constexpr float SHOT_CANNON_TIMING_ACTION  = 1.25f;  // 巨大弾発射
	static constexpr float SHOT_RAPID_TIMING_ACTIVE   = 0.375f; // 連射弾発射
	static constexpr float SHOT_CLUSTER_TIMING_ACTIVE = 0.4f;   // 拡散弾発射
	static constexpr float SHOT_SUPPORT_TIMING_ACTIVE = 0.75f;  // サポート職の発射

	// 行動終了後の時間
	static constexpr float JUMP_TOTAL_END         = 0.25f; // ジャンプ
	static constexpr float DODGE_TOTAL_END        = 0.0f;  // 回避
	static constexpr float DEFEAT_TOTAL_END       = 5.0f;  // 撃墜
	static constexpr float SHOT_BOMB_TOTAL_END    = 0.25f; // 爆発弾発射
	static constexpr float SHOT_CANNON_TOTAL_END  = 0.25f; // 巨大弾発射
	static constexpr float SHOT_RAPID_TOTAL_END   = 0.1f;  // 連射弾発射
	static constexpr float SHOT_CLUSTER_TOTAL_END = 0.0f;  // 拡散弾発射
	static constexpr float SHOT_SUPPORT_TOTAL_END = 0.25f; // サポート職の発射

	// 行動の停止時間
	static constexpr float JUMP_TOTAL_STOP        = 0.05f;  // ジャンプ
	static constexpr float DODGE_TOTAL_STOP       = 0.025f; // 回避
	static constexpr float DEFEAT_TOTAL_STOP      = 2.5f;   // 撃墜
	static constexpr float SHOT_CANNON_TOTAL_STOP = 1.85f;  // 巨大弾

	// 行動の停止タイミング
	static constexpr float JUMP_TIMING_STOP        = 0.175f; // ジャンプ
	static constexpr float DODGE_TIMING_STOP       = 0.15f;  // 回避
	static constexpr float DEFEAT_TIMING_STOP      = 0.0f;   // 撃墜
	static constexpr float SHOT_CANNON_TIMING_STOP = 1.15f;  // 巨大弾

	// 巨大弾 行動パラメータ
	static constexpr float SHOT_CANNON_TOTAL_ACTIVE_INCREMENT = 0.2f;  // 行動有効の間隔上昇値
	static constexpr float SHOT_CANNON_TOTAL_END_INCREMENT    = 0.55f; // 行動終了の間隔上昇値
	static constexpr float SHOT_CANNON_TOTAL_INPUT            = 0.2f;  // 連続入力が有効になる時間

	// Y軸制限位置
	static constexpr float LIMIT_POS_Y = -1500.0f;

	// 着弾地点画像
	int imageBulletHitPoint_;

	// 攻撃回数
	int attackNumMax_;
	int curAttackCount_;

	// 発射している弾の識別番号
	int shotIndex_;

	// 再生中のアニメーションの種類
	ANIMATION_TYPE animationType_;

	// 発射Z位置
	VECTOR throwPos_;

	// 発射方向
	VECTOR throwDir_;

	// 移動速度
	float moveSpeed_;

	// 吹っ飛ばし量
	Vector2F knockPowXZ_;

	// 回避の横移動量
	Vector2F dodgePowXZ_;

	// カメラに応じた回転をするか否か
	bool isCameraRotActive_;

	// 回避クールタイム
	float curTimeWaitDodge_;

	// 撃破時間
	float curTimeDefeat_;

	// 行動コントローラ
	std::unique_ptr<PActionController> actionController_;

	// 発射間隔
	float shotTerm_;

	// 自分が操作するかどうか
	bool isHostControl_;

	// 誰のキャラクターかの識別キー
	int netKey_;

	// 攻撃したかどうか
	bool isNetAttack_;

	// 攻撃をホスト側に送信するか否か
	bool isAttackSend_;

	// 着弾地点
	std::array<VERTEX3D, 4> shotPointVertex_;

	/// @brief 行動初期化処理
	void InitActions(void);

	/// @brief 移動処理
	void ProcessMove(void);

	/// @brief ジャンプ処理
	void ProcessJump(void);
	void Jump(void);

	/// @brief 回避処理
	void ProcessDodge(void);
	void Dodge(void);

	/// @brief 撃墜処理
	void ProcessDefeat(void);

	/// @brief 吹っ飛ばし処理
	void ProcessKnockback(void);

	/// @brief 攻撃処理
	void ProcessAttack(void);

	/// @brief 回復処理
	void ProcessRecovery(void);

	/// @brief 復活処理
	void SetRespawn(void);

	/// @brief アニメーション再生
	/// @param _type アニメーションの種類
	/// @param _isLoop ループ再生するか否か
	/// @param _animationSpeed 再生速度指定(任意)
	void PlayAnimation(ANIMATION_TYPE _type, bool _isLoop = true, bool _isAnimationBlend = true, float _animSpeed = -1.0f);

	/// @brief 弾丸生成
	void CreateBullet(void);

	/// @brief 弾丸発射
	void ShotBullet(void);

	/// @brief 弾丸更新
	void UpdateBullets(void);

	/// @brief 拡散弾生成
	void CreateCluster(void);

	/// @brief 拡散弾生成処理
	/// @param _throwDir 発射方向
	/// @return 拡散弾オブジェクト
	std::unique_ptr<PBulletNormal> CreateClusterBullet(const VECTOR& _throwDir);

	/// @brief 拡散弾発射
	void ShotCluster(void);

	/// @brief 移動方向に応じた遅延回転
	void DelayRotate(void)override;

	/// @brief 弾の軌道を描画
	void DrawShotOrbit(void);

	/// @brief 弾の着弾地点
	void DrawShotOrbitPoint(const VECTOR& _shotPos);

	/// @brief 発射方向取得
	VECTOR CalcShotDirection(void);
	
	/// @brief 通常弾更新
	void ProcShotNormal(void);

	/// @brief 特殊弾更新
	void ProcShotSpecial(void);

	/// @brief 攻撃の行動中か否か
	bool IsActionAttack(void)const;

	/// @brief 攻撃回数をリセットするか否か
	/// @param _type 再生するアニメーションの種類
	bool IsResetAttackCount(ANIMATION_TYPE _type)const;

	/// @brief 自分の最新状態をパケット用に変換して NetManager に送る
	void SendMyActionToNetManager(void);
};