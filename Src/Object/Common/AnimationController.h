#pragma once
#include <unordered_map>
#include <string>
#include "../../Utility/UtilityMath.h"

class AnimationController
{
public:

	enum class ANIM_TYPE
	{
		NONE,
		INTERNAL, // 内部アニメーション
		EXTERNAL, // 外部アニメーション
	};

	// アニメーションデータ
	struct Animation
	{
		ANIM_TYPE type = ANIM_TYPE::NONE;
		int modelId		= -1; // アニメーションモデル
		int attachNo	= -1;
		int animIndex	= 0;	 // モデル内アニメーション番号
		float speed		= 0.0f;	 // 再生速度
		float totalTime = 0.0f;	 // 最大再生時間
		float step		= 0.0f;	 // 現在再生時間
		bool isLoadPath = false; // パスで読み込んでいるか否か
		bool isInPlace = false; // アニメーションの位置を固定するか否か
		VECTOR inPlaceLocalPos = UtilityMath::VECTOR_ZERO; // 固定するアニメーションローカル位置
	};


	/// @brief コンストラクタ
	/// @param _modelId アニメーション対象
	AnimationController(int _modelId);

	/// @brief デストラクタ
	~AnimationController(void);

	/// @brief 同じモデル内のアニメーションを準備
	/// @param type アニメーション種類
	/// @param speed アニメーション速度 
	void AddInternal(int _type, float _speed = 30.0f);

	/// @brief 別の読み込み済みアニメーションモデルから準備し、再生座標固定
	/// @see 詳細な説明
	/// @param _type アニメーション種類
	/// @param _speed アニメーション速度 
	/// @param _handle アニメーションのハンドル
	/// @param _placeLocalPos 固定するアニメーションローカル位置
	void AddExternal(int _type, float _speed, int _handle
					, bool _isPlace = false, const VECTOR& _localPos = UtilityMath::VECTOR_ZERO);


	/// @brief アニメーション再生
	/// @param _type アニメーションの種類
	/// @param _isLoop ループするか否か @hint default = true
	/// @param _playSpeed 再生速度 @hint default = initSpeed
	/// @param _blendTime アニメーション遷移時間
	void Play(int _type, bool _isLoop = true, float _playSpeed = -1.0f, float _blendTime = 0.175f);

	/// @brief 更新処理
	void Update(void);

	/// @brief デバッグ描画処理
	void DrawDebug(void);

	/// @brief メモリ解放処理
	void Release(void);


	/// @brief アニメーションが終了しているか否か
	bool IsEnd(void) const;

	/// @brief 一定の位置に到達したかの判定
	/// @param _pointStart 判定開始位置の割合(0.0f～1.0f)
	/// @param _pointEnd 判定終了位置の割合(0.0f～1.0f)
	bool IsEndPoint(float _pointStart, float _pointEnd = 1.0f);

	/// @brief 再生中のアニメーションの再生割合を取得
	/// @return 割合(0.0～1.0)
	float GetPlayPointRate(void);

	/// @brief 再生中のアニメーション番号取得
	int GetPlayType(void) { return playType_; };


	/// @brief アニメーション停止処理
	/// @param _stopTime 停止時間
	void Stop(float _stopTime);

	/// @brief 再生位置変更処理
	/// @param _step 再生する位置
	void SetAnimStep(float _step = 0.0f);

	/// @brief 停止しているか否か
	bool isStop(void) { return isStop_; };

	/// @brief 再生位置変更
	/// @param rate 再生位置の割合(0.0f～1.0f)
	void SetAnimStepRate(float rate);

	void SetModelId(int _modelId);

	/// @brief 再生中のアニメーションの現在時間を取得
	float GetPlayTime(void);

	/// @brief 再生中のアニメーションの総再生時間を取得
	float GetPlayTimeTotal(void);


private:

	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::unordered_map<int, Animation> animations_;

	// 再生中のアニメーション状態
	int playType_;

	// 前回のアニメーション状態
	int prePlayType_;

	// ブレンド時間
	float blendTime_;

	float playSpeed_;

	// ブレンドのカウンタタイマー
	float curBlendTime_;

	// ループするか否かの判定
	bool isLoop_;

	// アニメーションを停止するか否か
	bool isStop_;

	// 停止時間
	float timeStop_;
	
	float term;
	
	/// @brief 他アニメーションとのブレンドの影響を受けない単体の素のルート位置を取得
	/// @param _target 位置を取得したいアニメーション
	/// @param _other ブレンド対象の相方アニメーション(一時的にブレンド率0%にする)
	VECTOR GetRawAnimRootPos(Animation& _target, Animation& _other);
	

	/// @brief アニメーション追加処理
	/// @param _type アニメーションの種類
	/// @param _animIndex 格納するアニメーションリスト
	void Add(int _type, Animation& _animIndex);

	/// @brief アニメーションが格納されているか判定
	/// @param _type アニメーションの種類
	bool IsFindAnimation(int _type);

	/// @brief 固定アニメーション処理
	/// @param _prePlayAnim 再生中のアニメーション
	/// @param _curPlayAnim 再生中のアニメーション
	void AnimationInPlace(Animation& _prePlayAnim, Animation& _curPlayAnim, float _blendTime);
};