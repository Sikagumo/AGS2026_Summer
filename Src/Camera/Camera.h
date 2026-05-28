#pragma once
#include <DxLib.h>
#include "../Common/Quaternion.h"
#include "../Object/Actor/ActorBase.h"
#include <vector>
class Transform;
class InputManager;

class Camera : public ActorBase
{

public:

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 10.0f;
	static constexpr float VIEW_FAR = 20000.0f;

	// 追従位置からカメラ位置までの相対座標
	static constexpr VECTOR FOLLOW_LOCAL_POS = { 25.0f, 50.0f, -150.0f };
	static constexpr VECTOR FOLLOW_LOCAL_POS_LOCKON = { 60.0f, 45.0f, -75.0f };

	// 追従位置から注視点までの相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 50.0f, 50.0f, 50.0f };


	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP = 40.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DOWN = 35.0f * (DX_PI_F / 180.0f);
	
	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,
		FREE,
		FOLLOW,
	};

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		SPHERE,
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

	void SetLockOnPosition(const VECTOR& _pos);

	// ロックオンするか否かの設定
	void SetIsLockOn(bool _isLockOn) { isLockOn_ = _isLockOn; };
	bool GetIsLockOn(void)const { return isLockOn_; };


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

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION_CAMERA = 30;

	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 2.0f;

	// 衝突判定用球体半径
	static constexpr float COL_CAPSULE_SPHERE = 50.0f;

	// カメラの補間移動率
	static constexpr float LERP_RATE_MOVE = 0.1f;

	InputManager& inputManager_;

	// カメラの視野角割合
	float fovRate_;

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
	bool isLockOn_;
	VECTOR lockOnPos_;
	VECTOR lockOnPrePos_;


	
	// カメラを初期位置に戻す
	void SetDefault(void);

	// 追従対象との位置同期を取る
	void SyncFollow(void);

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
	void SetBeforeDrawFollow(void);

	// 衝突判定
	void Collision(void);

};
