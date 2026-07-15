#include "AnimationController.h"
#include <DxLib.h>
#include <string>
#include <cassert>
#include <algorithm>
#include "../../Manager/System/TimeManager.h"

// ROOTフレーム番号
constexpr int FRAME_ROOT_NUM = 1;


AnimationController::AnimationController(int _modelId)
	: animations_{}
	, modelId_(_modelId)
	, playType_(-1), prePlayType_(-1)
	, blendTime_(0.0f), curBlendTime_(0.0f)
	, isLoop_(false)
	, isStop_(false), timeStop_(0.0f)
	, playSpeed_(0.0f)
	, preAnimLocalPos_(UtilityMath::VECTOR_ZERO)
{
}


AnimationController::~AnimationController(void)
{
	Release();
}


void AnimationController::AddInternal(int _type, float _speed)
{
	/* 内部のアニメーションの追加 */
	Animation animation = Animation();

	animation.animIndex = _type;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIM_TYPE::INTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = false;

	// アニメーション追加処理
	Add(_type, animation);
}
void AnimationController::AddInternal(int _type, const VECTOR& _localPos, float _speed)
{
	/* 内部のアニメーションの追加 */
	Animation animation = Animation();

	animation.animIndex = _type;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIM_TYPE::INTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = true;

	if (!UtilityMath::EqualsVZero(_localPos))
	{
		animation.inPlaceLocalPos = _localPos;
	}

	// アニメーション追加処理
	Add(_type, animation);
}

void AnimationController::AddExternal(int _type, int _handle, float _speed)
{
	/* 外部のアニメーションの追加 */
	Animation animation = Animation();

	animation.modelId = _handle;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIM_TYPE::EXTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = false;
	
	// アニメーション追加処理
	Add(_type, animation);
}
void AnimationController::AddExternal(int _type, int _handle
									  , const VECTOR& _localPos, float _speed)
{
	/* 外部のアニメーションの追加 */
	Animation animation = Animation();

	animation.modelId = _handle;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIM_TYPE::EXTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = true;

	if (!UtilityMath::EqualsVZero(_localPos))
	{
		animation.inPlaceLocalPos = _localPos;
		animation.inPlaceLocalPosEnd = _localPos;
	}
	
	// アニメーション追加処理
	Add(_type, animation);
}
void AnimationController::AddExternal(int _type, int _handle
									  , const VECTOR& _localPos, const VECTOR& _localPosEnd, float _speed)
{
	/* 外部のアニメーションの追加 */
	Animation animation = Animation();

	animation.modelId = _handle;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIM_TYPE::EXTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = true;

	if (!UtilityMath::EqualsVZero(_localPos))
	{
		animation.inPlaceLocalPos = _localPos;
	}
	if (!UtilityMath::Equals(_localPos, _localPosEnd))
	{
		animation.inPlaceLocalPosEnd = _localPosEnd;
	}
	
	// アニメーション追加処理
	Add(_type, animation);
}


void AnimationController::Play(int _type, bool _isLoop, float _playSpeed, float _blendTime)
{
	// 同じアニメーション時、処理を終了
	if (playType_ == _type || _type == -1) { return; }

	if (prePlayType_ != -1)
	{
		// モデルからアニメーションを外す
		auto& preAnim = animations_[prePlayType_];
		MV1DetachAnim(modelId_, preAnim.attachNo);
		prePlayType_ = -1;
	}

	// 現在のアニメを前回に割り当て
	if (playType_ != -1)
	{
		prePlayType_ = playType_;
		
		auto& stateFromAnim = animations_[prePlayType_];
		if (stateFromAnim.isInPlace)
		{
			// 遷移元が固定位置アニメの場合、固定値をそのまま使用
			preAnimLocalPos_ = stateFromAnim.inPlaceLocalPos;
		}
		else
		{
			// 遷移元が非固定の場合、現在のルートフレームの実際のローカル位置を取得
			MATRIX curFrameMat = MV1GetFrameLocalMatrix(modelId_, FRAME_ROOT_NUM);
			preAnimLocalPos_ = MGetTranslateElem(curFrameMat);
		}
	}

	// アニメーションループ
	isLoop_ = _isLoop;

	// 停止を解除
	isStop_ = false;

	// アニメーション種別を変更
	playType_ = _type;

	// ブレンド時間初期化
	curBlendTime_ = 0.0f;

	blendTime_ = _blendTime;


	auto& playAnim = animations_[_type];

	// 初期化
	playAnim.step = 0.0f;

	// 再生速度割り当て
	playSpeed_ = ((_playSpeed >= 0.0f) ? _playSpeed : playAnim.speed);
	

	// モデルにアニメーションを付ける
	if (playAnim.type == ANIM_TYPE::INTERNAL)
	{
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim.attachNo = MV1AttachAnim(modelId_, playAnim.animIndex);
	}
	else
	{
		// 別のモデルファイルからアニメーションをアタッチする
		// DxModelViewerを確認すること(大体0か1)
		int animIdx = 0;
		playAnim.attachNo = MV1AttachAnim(modelId_, animIdx, playAnim.modelId);
	}

	// アニメーション総時間の取得
	playAnim.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim.attachNo);


	// 前回のアニメーションがある時、ブレンド率を1.0f(100%)にする
	float blendRate = ((prePlayType_ == -1) ? 1.0f : 0.0f);

	// ブレンドアニメーションの割合を割り当て
	MV1SetAttachAnimBlendRate(modelId_, playAnim.attachNo, blendRate);
}


void AnimationController::Update(void)
{
	// 経過時間の取得
	float deltaTime = TimeManager::GetInstance().GetDeltaTime();
	auto& curAnim = animations_[playType_];
	auto& preAnim = animations_[prePlayType_];


	// 停止時に処理終了
	if (isStop_ && timeStop_ > 0.0f)
	{
		timeStop_ -= deltaTime;
		if (timeStop_ <= 0.0f) { isStop_ = false; }

		return;
	}

	// 再生中のアニメーション
	if (playType_ != -1)
	{
		// アニメーション進行処理
		curAnim.step += (deltaTime * playSpeed_);

		if (curAnim.step >= curAnim.totalTime && isLoop_)
		{
			// 再生がループ状態で終了時、初期位置に戻す
			curAnim.step = 0.0f;
		}

		// アニメーション更新
		MV1SetAttachAnimTime(modelId_, curAnim.attachNo, curAnim.step);
	}


	// ブレンド時間が割り当てているときは、現在時間と最大時間の割合を、それ以外はタイマーを終了
	term = ((blendTime_ > 0.0f) ? (curBlendTime_ / blendTime_) : 1.0f);

	if (prePlayType_ != -1)
	{
		// ブレンドタイマー増加
		curBlendTime_ += deltaTime;



		// 旧・新規アニメーションのブレンド率を割り当て
		MV1SetAttachAnimBlendRate(modelId_, preAnim.attachNo, (1.0f - term));
		MV1SetAttachAnimBlendRate(modelId_, curAnim.attachNo, term);

		// ブレンドアニメーション終了時
		if (term >= 1.0f)
		{
			// 前アニメーションをデタッチ
			MV1DetachAnim(modelId_, preAnim.attachNo);
			prePlayType_ = -1;

			// 新規アニメーションのブレンド率を100%にする
			MV1SetAttachAnimBlendRate(modelId_, curAnim.attachNo, 1.0f);
		}
	}

	// 対象フレームのローカル行列を初期値にリセットする
	MV1ResetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM);

	
	if (preAnim.isInPlace || curAnim.isInPlace)
	{
		// 遷移前/遷移後アニメーションが固定の場合、アニメーション位置固定処理
		AnimationInPlace(preAnim, curAnim, term);
	}
}
void AnimationController::AnimationInPlace(Animation& _prePlayAnim, Animation& _curPlayAnim, float _rate)
{
	// 対象フレームのローカル行列を初期値にリセットす
	MV1ResetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM);
	
	// アニメーションブレンド率
	float rate = std::clamp(_rate, 0.0f, 1.0f);

	// 遷移の位置
	VECTOR preBase = preAnimLocalPos_;
	VECTOR curBase = UtilityMath::VECTOR_ZERO;
	
	if (_curPlayAnim.isInPlace)
	{
		// 固定位置アニメーションの場合、固定値を使用
		curBase = _curPlayAnim.inPlaceLocalPos;
	}
	else
	{
		curBase = GetRawAnimRootPos(_curPlayAnim, _prePlayAnim);
		
		// ブレンド率に戻す
		if (_curPlayAnim.attachNo != -1)
			{ MV1SetAttachAnimBlendRate(modelId_, _curPlayAnim.attachNo, rate); }
			
		if (_prePlayAnim.attachNo != -1)
			{ MV1SetAttachAnimBlendRate(modelId_, _prePlayAnim.attachNo, (1.0f - rate)); }
	}
		
	// アニメーション位置変更の線形補間用座標
	VECTOR localPos = UtilityMath::VECTOR_ZERO;
	localPos.x = (preBase.x + (curBase.x - preBase.x) * rate);
	localPos.y = (preBase.y + (curBase.y - preBase.y) * rate);
	localPos.z = (preBase.z + (curBase.z - preBase.z) * rate);

	// 対象フレームのローカル行列(大きさ、回転)はモデル本来の値を使用
	MATRIX mat = MV1GetFrameLocalMatrix(modelId_, FRAME_ROOT_NUM);
	VECTOR scl = MGetSize(mat);
	MATRIX rot = MGetRotElem(mat);

	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(scl));
	mix = MMult(mix, rot);
	mix = MMult(mix, MGetTranslate(localPos));

	// 対象フレームにセットし直し、アニメーションの移動値を無効化
	MV1SetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM, mix);
}
VECTOR AnimationController::GetRawAnimRootPos(Animation& _target, Animation& _other)
{
	if (_target.attachNo != -1)
	{
		MV1SetAttachAnimBlendRate(modelId_, _target.attachNo, 1.0f);
	}
	if (_other.attachNo != -1)
	{
		MV1SetAttachAnimBlendRate(modelId_, _other.attachNo, 0.0f);
	}

	MATRIX mat = MV1GetFrameLocalMatrix(modelId_, FRAME_ROOT_NUM);
	return MGetTranslateElem(mat);
}


void AnimationController::DrawDebug(void)
{
#ifdef _DEBUG
	if (playType_ == -1) { return; }
	auto& anim = animations_.at(playType_);

	// アニメーションの描画
	DrawFormatString(0,64,0xFF0000,"animTime:%.2f, term : %.2f",anim.step, term);
#endif // _DEBUG
}

void AnimationController::Release(void)
{
	if (animations_.empty()) { return; }

	// ロードしたアニメーションを解放
	for (auto& [type, anim] : animations_)
	{
		// アニメーションをリセット
		MV1DetachAnim(modelId_, anim.attachNo);

		// パス読み込みでの外部アニメーション時
		if (anim.type == ANIM_TYPE::EXTERNAL &&
			anim.isLoadPath)
		{
			// アニメーション解放
			MV1DeleteModel(anim.modelId);
		}
	}

	// リスト解放
	animations_.clear();
}

bool AnimationController::IsEnd(void) const
{
	// アニメーションが再生されていない・ループアニメーション時、false
	if (playType_ == -1 || isLoop_) { return false; }

	auto& anim = animations_.at(playType_);

	// 再生時間が最大再生時間を超えたら、true
	return (anim.step >= anim.totalTime);

	/*
	// アニメーションが終了しているか
	if (playAnim_.step >= playAnim_.totalTime)
	{
		// アニメーションが終了している
		return true;
	}

	if (isLoop_)
	{
		// ループ時は終了していない判定
		return false;
	}

	return false;
	*/
}

bool AnimationController::IsEndPoint(float _pointStart, float _pointEnd)
{
	if (playType_ == -1) { return false; }

	Animation& animation = animations_.at(playType_);

	// 再生位置
	float start = std::clamp(_pointStart, 0.0f, 1.0f);

	// 終了位置
	float end = std::clamp(_pointEnd, 0.0f, 1.0f);

	// 再生位置の割合
	float curRate = (animation.step / animation.totalTime);

	// 再生位置が指定の割合になったときtrue
	return (curRate >= start && curRate < end);
}
float AnimationController::GetPlayPointRate(void)
{
	/* アニメーション再生割合を取得 */

	if (playType_ == -1) { return 0.0f; }

	Animation& anim = animations_.at(playType_);

	return (anim.step / anim.totalTime);
}

void AnimationController::Stop(float _stopTime)
{
	isStop_ = true;
	timeStop_ = _stopTime;
}

void AnimationController::SetAnimStep(float _step)
{
	if (playType_ == -1) { return; }

	auto& anim = animations_.at(playType_);

	// 再生位置の制限
	float step = std::clamp(_step, 0.0f, anim.totalTime);

	// 再生位置割り当て
	anim.step = _step;
}

void AnimationController::SetAnimStepRate(float _rate)
{
	if (playType_ == -1) { return; }

	auto& anim = animations_.at(playType_);

	float step = std::clamp(_rate, 0.0f, 1.0f);

	// 再生位置割り当て
	float rate = (1.0f / anim.totalTime);
	anim.step = (rate * step);
}

void AnimationController::SetModelId(int _modelId)
{
	modelId_ = _modelId;
	for (auto& anim : animations_)
	{
		anim.second.modelId = _modelId;
	}
}

float AnimationController::GetPlayTime(void)
{
	auto& anim = animations_.at(playType_);
	float time = -1;
	if (playType_ != -1)
	{
		time = anim.step;
	}

#ifdef _DEBUG
	else
	{
		OutputDebugString("\nアニメーションが割り当てられていないため、再生時間が取得出来ませんでした；；\n");
		assert(false); // 例外スロー
	}
#endif

	return time;
}

float AnimationController::GetPlayTimeTotal(void)
{
	auto& anim = animations_.at(playType_);
	float time = -1;
	if (playType_ != -1)
	{
		time = anim.totalTime;
	}

#ifdef _DEBUG
	else
	{
		OutputDebugString("\nアニメーションが割り当てられていないため、総再生時間が取得出来ませんでした；；\n");
		assert(false); // 例外スロー
	}
#endif

	return time;
}


void AnimationController::Add(int _type, Animation& _animation)
{
	if (animations_.count(_type) == 0)
	{
		// 動的配列に追加
		animations_.emplace(_type, _animation);
	}
}

bool AnimationController::IsFindAnimation(int _type)
{
	auto it = animations_.find(_type);
	if (it != animations_.end())
	{
		// 発見
		return true;
	}
#ifdef _DEBUG
	else
	{
		OutputDebugString("\nアニメーションの情報がありません。\n");
	}
#endif

	return false;
}