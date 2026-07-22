#pragma once
#include <DxLib.h>
#include "../Common/Quaternion.h"
#include "../Object/Actor/ActorBase.h"
#include <map>
#include <memory>
class Transform;

class Camera : public ActorBase
{

public:

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 10.0f;
	static constexpr float VIEW_FAR = 20000.0f;

	// 追従位置からカメラ位置までの相対座標
	static constexpr VECTOR FOLLOW_LOCAL_POS = { 25.0f, 50.0f, -150.0f };
	static constexpr VECTOR FOLLOW_LOCAL_POS_LOCKON = { 60.0f, 35.0f, -100.0f };

	// 追従位置から注視点までの相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 50.0f, 45.0f, 50.0f };


	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP = 40.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DOWN = 35.0f * (DX_PI_F / 180.0f);
	
	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,
		FREE,
		PLAYER_FOLLOW,
		BOSS_FOLLOW,
	};

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		SPHERE,
		MAX,
	};

	enum class LOCKON_TARGET
	{
		NONE = -1,
		BOSS_BODY,
		BOSS_WEAPON_MGL_L,
		BOSS_WEAPON_MGL_R,
		BOSS_WEAPON_CANNON_L,
		BOSS_WEAPON_CANNON_R,
		BOSS_WEAPON_MP_L,
		BOSS_WEAPON_MP_R,
		BOSS_WEAPON_RG,
		MAX,
	};


	// コンストラクタ
	Camera(void);

	// デストラクタ
	~Camera(void)override = default;

	/// @brief リソースロード
	void Load(void) override {}

	// 更新
	void Update(void)override;

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 座標の取得
	const VECTOR& GetPos(void) const { return transform_.pos; };

	// 角度の取得
	const VECTOR& GetAngles(void) const { return angles_;  };
		// X回転を抜いたY軸のみのカメラ角度
	const Quaternion& GetQuaRotY(void) const { return rotY_; };
	
	// 注視点の取得
	const VECTOR& GetTargetPos(void) const { return targetPos_;  };

	// カメラの前方方向
	VECTOR GetForward(void) const;

	// カメラモードの変更
	void ChangeMode(MODE _mode);

	// 追従対象の設定
	void SetFollow(const Transform* _follow) { followTransform_ = _follow; };

	//クオータニオン角度の取得
	void SetQuaternionRot(Quaternion _rot) { transform_.quaRot = _rot; }

	/// @brief 追従対象割り当て
	/// @param _target 対象の種類
	/// @param _targetPos 追従位置
	/// @param _targetHp 追従対象HP
	void SetLockOnTargets(LOCKON_TARGET _target,const VECTOR& _targetPos, int _targetHp);

	/// @brief 現在の追従対象割り当て
	LOCKON_TARGET GetLockOnTargetNum(void) { return lockOnTarget_; };

	/// @brief 追従対象選択処理
	void LockOnChoice(void);
	void FollowLockOnPosition(void);

	// ロックオンするか否かの設定
	void SetIsLockOn(bool _isLockOn);
	bool GetIsLockOn(void)const { return isLockOn_; };

	/// @brief ロックオン中の対象の位置を取得
	const VECTOR& GetLockOnPos(void)const { return lockOnParam_.pos; };
	int GetLockOnHp(void)const { return lockOnParam_.hp; };
	int GetLockOnMaxHp(void)const { return lockOnParam_.maxHp; };

	/// @brief イージング遷移中か否か
	bool IsEasingState(void) { return (easingTerm_ < 1.0f); };

	// 押し返し処理
	void ResolveCollision(void);


protected:

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override {}

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override {}

	// 初期化後の個別処理
	void InitPost(void) override;

private:

	//ボスの追従時の座標
	static constexpr VECTOR BOSS_CAMERA_POS = { 0,10,-500 };


	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION_CAMERA = 30;

	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 2.0f;

	// 衝突判定用球体半径
	static constexpr float COL_CAPSULE_SPHERE = 30.0f;

	// カメラの補間移動率
	static constexpr float LERP_RATE_MOVE = 0.1f;

	// 壁衝突時の追従距離の最小倍率
	static constexpr float FOLLOW_DIST_MIN_SCALE = 0.5f;

	// 追従距離の補間速度
	static constexpr float FOLLOW_DIST_LERP_SPEED = 5.0f;


	// カメラの更新前位置
	VECTOR prePos_;

	// カメラが追従対象とするTransform
	const Transform* followTransform_;

	// カメラモード
	MODE mode_;

	// カメラ角度(rad)
	VECTOR angles_;

	// カメラ角度(Y軸のみ)
	Quaternion rotY_;

	// 注視点
	VECTOR targetPos_;

	// ターゲット
	struct TargetParam
	{
		int hp = 0;
		int maxHp = 0;
		VECTOR pos;
	};
	std::map<LOCKON_TARGET, std::unique_ptr<TargetParam>> targetsParam_;
	bool isLockOn_;
	TargetParam lockOnParam_;
	LOCKON_TARGET lockOnTarget_;

	float easingTerm_;      // イージング経過値
	VECTOR easingFromPos_;    // カメラ位置イージング開始座標
	VECTOR easingFromTarget_; // 注視点イージング開始座標

	// 壁衝突時の追従距離倍率
	float followDistScale_;
	
	// カメラを初期位置に戻す
	void SetDefault(void);

	// 追従対象との位置同期を取る
	void SyncFollow(void);

	/// @brief アングルとY軸回転の同期処理
	void SyncAngleYFromRotY(void);

	// カメラ操作
	void ProcessRot(bool _isLimit);
	void ProcessMove(void);

	// カメラ回転
	void RotationKeyboard(bool _isLimit);
	void RotationMouse(bool _isLimit);
	void RotationGamePad(bool _isLimit);

	// モード別更新ステップ
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollowPlayer(void);
	void SetBeforeDrawFollowBoss(void);

	// 衝突判定
	void Collision(void);

	VECTOR EasingChangeTarget(const VECTOR& _fromVec, const VECTOR& _toVec, float _term);
};
