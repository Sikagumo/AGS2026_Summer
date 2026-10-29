#include "AnimationController.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include "../../Manager/System/TimeManager.h"

namespace
{
	using Animation = AnimationController::Animation;
	using AnimationMap = std::unordered_map<int, Animation>;

	// ROOTフレーム番号
	constexpr int FRAME_ROOT_NUM = 1;

	/// @brief 登録済みアニメーションを検索する(operator[]と違い、要素を勝手に作らない)
	/// @return 見つかったアニメーション。無ければ、nullptr
	Animation* FindAnimation(AnimationMap& _animations, int _type)
	{
		auto it = _animations.find(_type);
		return (it != _animations.end()) ? &it->second : nullptr;
	}

	/// @brief アニメーションをモデルから外し、識別番号を未割当(-1)に戻す
	void DetachAnimation(int _modelId, Animation& _animation)
	{
		if (_animation.attachNo == -1) { return; }

		MV1DetachAnim(_modelId, _animation.attachNo);
		_animation.attachNo = -1;
	}

	/// @brief 2つの位置を線形補間する
	/// @param _rate 補間率(0.0f～1.0f)
	VECTOR LerpVector(const VECTOR& _start, const VECTOR& _end, float _rate)
	{
		return VGet(_start.x + (_end.x - _start.x) * _rate
			, _start.y + (_end.y - _start.y) * _rate
			, _start.z + (_end.z - _start.z) * _rate);
	}

	/// @brief 登録用のアニメーションデータを作成する
	Animation CreateAnimation(AnimationController::ANIMATION_TYPE _type, float _speed)
	{
		Animation animation = Animation();
		animation.type = _type;
		animation.speed = _speed;
		animation.step = 0.0f;
		animation.isInPlace = false;
		return animation;
	}

	/// @brief アニメーションを位置固定にする
	/// @param _start 固定位置の開始ローカル座標
	/// @param _end 固定位置の終了ローカル座標
	void ApplyInPlace(Animation& _animation, const VECTOR& _start, const VECTOR& _end)
	{
		_animation.isInPlace = true;
		_animation.inPlaceLocalPos = _start;
		_animation.inPlaceLocalPosEnd = _end;
	}
};

AnimationController::AnimationController(int _modelId)
	: modelId_(_modelId)
	, animations_{}
	, playType_(-1)
	, prePlayType_(-1)
	, blendTime_(0.0f)
	, blendRate_(0.0f)
	, curBlendTime_(0.0f)
	, isLoop_(false)
	, isStop_(false)
	, timeStop_(0.0f)
	, playSpeed_(0.0f)
	, preAnimationLocalPos_(UtilityMath::VECTOR_ZERO)
{
}

AnimationController::~AnimationController(void)
{
	Release();
}

void AnimationController::AddInternal(int _type, float _speed)
{
	/* 内部のアニメーションの追加 */
	Animation animation = CreateAnimation(ANIMATION_TYPE::INTERNAL, _speed);
	animation.animationIndex = _type;

	// アニメーション追加処理
	Add(_type, animation);
}

void AnimationController::AddInternal(int _type, const VECTOR& _localPos, float _speed)
{
    Animation animation = CreateAnimation(ANIMATION_TYPE::INTERNAL, _speed);
    animation.animationIndex = _type;
    ApplyInPlace(animation, _localPos, _localPos);   // if を外す
    Add(_type, animation);
}

void AnimationController::AddExternal(int _type, int _handle, float _speed)
{
	//* 外部のアニメーションの追加 */
	Animation animation = CreateAnimation(ANIMATION_TYPE::EXTERNAL, _speed);
	animation.modelId = _handle;
	
	// アニメーション追加処理
	Add(_type, animation);
}

void AnimationController::AddExternal(int _type, int _handle
									  , const VECTOR& _localPos, float _speed)
{
	/* 外部のアニメーションの追加(位置固定) */
	Animation animation = CreateAnimation(ANIMATION_TYPE::EXTERNAL, _speed);
	animation.modelId = _handle;

	if (!UtilityMath::EqualsVZero(_localPos))
	{
		ApplyInPlace(animation, _localPos, _localPos);
	}
	
	// アニメーション追加処理
	Add(_type, animation);
}

void AnimationController::AddExternal(int _type, int _handle
									  , const VECTOR& _localPosStart, const VECTOR& _localPosEnd, float _speed)
{
	/* 外部のアニメーションの追加(再生中に固定位置を移動) */
	Animation animation = CreateAnimation(ANIMATION_TYPE::EXTERNAL, _speed);
	animation.modelId = _handle;

	animation.isInPlace = true;
	ApplyInPlace(animation, _localPosStart, _localPosEnd);
	
	// アニメーション追加処理
	Add(_type, animation);
}

void AnimationController::Play(int _type, bool _isLoop, float _playSpeed, float _blendTime)
{
	// 同じアニメーション時、処理を終了
	if (playType_ == _type || _type == -1) { return; }

	// 未登録のアニメーションは再生しない
	Animation* playAnim = FindAnimation(animations_, _type);
	if (playAnim == nullptr)
	{
#ifdef _DEBUG
		OutputDebugString("\n未登録のアニメーションは再生できません。\n");
#endif
		return;
	}

	// ブレンド中の前アニメーションがあれば、モデルから外す
	if (Animation* preAnimation = FindAnimation(animations_, prePlayType_))
	{
		DetachAnimation(modelId_, *preAnimation);
	}

	prePlayType_ = -1;

	// 現在のアニメを前回に割り当て
	if (Animation* stateFromAnim = FindAnimation(animations_, playType_))
	{
		prePlayType_ = playType_;

		if (stateFromAnim->isInPlace)
		{
			// 遷移元が固定位置アニメの場合、固定値をそのまま使用
			preAnimationLocalPos_ = stateFromAnim->inPlaceLocalPos;
		}
		else
		{
			// 遷移元が非固定の場合、現在のルートフレームの実際のローカル位置を取得
			MATRIX curFrameMat = MV1GetFrameLocalMatrix(modelId_, FRAME_ROOT_NUM);
			preAnimationLocalPos_ = MGetTranslateElem(curFrameMat);
		}
	}

	// 再生状態の初期化
	isLoop_ = _isLoop;
	isStop_ = false;
	playType_ = _type;

	// ブレンド時間初期化
	curBlendTime_ = 0.0f;
	blendTime_ = _blendTime;

	playAnim->step = 0.0f;

	// 再生速度割り当て(負の値は登録時の速度を使用)
	playSpeed_ = ((_playSpeed >= 0.0f) ? _playSpeed : playAnim->speed);

	// モデルにアニメーションを付ける
	if (playAnim->type == ANIMATION_TYPE::INTERNAL)
	{
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim->attachNo = MV1AttachAnim(modelId_, playAnim->animationIndex);
	}
	else
	{
		// 別のモデルファイルからアニメーションをアタッチする
		// DxModelViewerを確認すること(大体0か1)
		constexpr int ANIM_INDEX_EXTERNAL = 0;
		playAnim->attachNo = MV1AttachAnim(modelId_, ANIM_INDEX_EXTERNAL, playAnim->modelId);
	}

	// アニメーション総時間の取得
	playAnim->totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim->attachNo);

	// 前回のアニメーションがある時はブレンド率0%から、無い時は100%にする
	float startRate = ((prePlayType_ == -1) ? 1.0f : 0.0f);
	MV1SetAttachAnimBlendRate(modelId_, playAnim->attachNo, startRate);
}

void AnimationController::Update(void)
{
	// 経過時間の取得
	float deltaTime = TimeManager::GetInstance().GetDeltaTime();

	// 停止時に処理終了
	if (isStop_ && timeStop_ > 0.0f)
	{
		timeStop_ -= deltaTime;
		
		// 停止時間が終了したらフラグを無効化
		isStop_ = ((timeStop_ <= 0.0f) ? false : isStop_);

		return;
	}

	// 再生中のアニメーションがない時は処理しない
	Animation* curAnimation = FindAnimation(animations_, playType_);
	if (curAnimation == nullptr) { return; }

	// ブレンド中の前アニメーション(無ければ、nullptr)
	Animation* preAnimation = FindAnimation(animations_, prePlayType_);


	// アニメーション進行処理
	curAnimation->step += (deltaTime * playSpeed_);

	if (curAnimation->step >= curAnimation->totalTime && isLoop_)
	{
		// 再生がループ状態で終了時、初期位置に戻す
		curAnimation->step = 0.0f;
	}

	// アニメーション更新
	MV1SetAttachAnimTime(modelId_, curAnimation->attachNo, curAnimation->step);


	if (preAnimation != nullptr)
	{
		// ブレンド率(ブレンド時間が0以下の時は即時100%)
		blendRate_ = ((blendTime_ > 0.0f) ? (curBlendTime_ / blendTime_) : 1.0f);
		blendRate_ = std::clamp(blendRate_, 0.0f, 1.0f);

		// ブレンドタイマー増加
		curBlendTime_ += deltaTime;

		// 旧・新規アニメーションのブレンド率を割り当て
		MV1SetAttachAnimBlendRate(modelId_, preAnimation->attachNo, (1.0f - blendRate_));
		MV1SetAttachAnimBlendRate(modelId_, curAnimation->attachNo, blendRate_);

		// ブレンドアニメーション終了時
		if (blendRate_ >= 1.0f)
		{
			// 前アニメーションをデタッチ
			DetachAnimation(modelId_, *preAnimation);
			prePlayType_ = -1;
			preAnimation = nullptr;

			// 新規アニメーションのブレンド率を100%にする
			MV1SetAttachAnimBlendRate(modelId_, curAnimation->attachNo, 1.0f);
		}
	}
	else
	{
		// 前アニメーションがない時は常に100%
		blendRate_ = 1.0f;
	}

	// 対象フレームのローカル行列を初期値にリセットする
	MV1ResetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM);

	
	if (IsPreInPlace(preAnimation, curAnimation))
	{
		// 遷移前・遷移後アニメーションが固定の場合、アニメーション位置固定処理
		Animation emptyAnimation = Animation();
		AnimationInPlace((preAnimation != nullptr ? *preAnimation : emptyAnimation), *curAnimation, blendRate_);
	}
}

bool AnimationController::IsPreInPlace(const Animation* _preAnimation, const Animation* _curAnimation) const
{
	return ((_preAnimation != nullptr && _preAnimation->isInPlace)
		|| _curAnimation->isInPlace);
}

void AnimationController::AnimationInPlace(Animation& _prePlayAnimation, Animation& _curPlayAnimation, float _rate)
{
	// 対象フレームのローカル行列を初期値にリセットす
	MV1ResetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM);
	
	// アニメーションブレンド率
	float blendRate = std::clamp(_rate, 0.0f, 1.0f);

	// 遷移の位置
	VECTOR preBase = preAnimationLocalPos_;
	VECTOR curBase = UtilityMath::VECTOR_ZERO;
	
	if (_curPlayAnimation.isInPlace)
	{
		// 固定位置アニメーションの場合、自身の再生進行度(step/totalTime)に応じて
		// inPlaceLocalPos → inPlaceLocalPosEnd へ徐々に移動した目標位置を使用
		curBase = GetInPlaceProgressPos(_curPlayAnimation);
	}
	else
	{
		curBase = GetRawAnimationRootPos(_curPlayAnimation, _prePlayAnimation);
		
		// ブレンド率に戻す
		if (_curPlayAnimation.attachNo != -1)
		{
			MV1SetAttachAnimBlendRate(modelId_, _curPlayAnimation.attachNo, blendRate);
		}
		if (_prePlayAnimation.attachNo != -1)
		{
			MV1SetAttachAnimBlendRate(modelId_, _prePlayAnimation.attachNo, (1.0f - blendRate));
		}
	}
		
	// アニメーション位置変更の線形補間用座標
	VECTOR localPos = UtilityMath::VECTOR_ZERO;
	localPos = LerpVector(preBase, curBase, blendRate);

	// 対象フレームのローカル行列(大きさ、回転)はモデル本来の値を使用し補間した行列を合成
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

VECTOR AnimationController::GetRawAnimationRootPos(Animation& _target, Animation& _other)const
{
	// 対象のみ100%、相方を0%にして素のルート位置を取得する
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

VECTOR AnimationController::GetInPlaceProgressPos(const Animation& _animation) const
{
	if (_animation.totalTime <= 0.0f)
	{
		// 総再生時間が未取得(0以下)の間は開始位置をそのまま返す
		return _animation.inPlaceLocalPos;
	}

	// アニメーション自身の再生進行度(0.0～1.0)
	float animRate = std::clamp((_animation.step / _animation.totalTime), 0.0f, 1.0f);

	// 開始位置 → 終了位置へ進行度で線形補間
	return LerpVector(_animation.inPlaceLocalPos, _animation.inPlaceLocalPosEnd, animRate);
}

void AnimationController::DrawDebug(void)
{
#ifdef _DEBUG
	if (playType_ == -1) { return; }

	auto& animation = animations_.at(playType_);

	// アニメーションの描画
	DrawFormatString(0,64,0xFF0000,"animationTime:%.2f, term : %.2f",animation.step, blendRate_);
#endif // _DEBUG
}

void AnimationController::Release(void)
{
	if (animations_.empty()) { return; }

	// ロードしたアニメーションを解放
	for (auto& [type, animation] : animations_)
	{
		// アニメーションをリセット
		MV1DetachAnim(modelId_, animation.attachNo);

		// パス読み込みでの外部アニメーション時
		if (animation.type == ANIMATION_TYPE::EXTERNAL &&
			animation.isLoadPath)
		{
			// アニメーション解放
			MV1DeleteModel(animation.modelId);
		}
	}

	// リスト解放
	animations_.clear();

	// 再生状態を未再生に戻す
	playType_ = -1;
	prePlayType_ = -1;
}

bool AnimationController::IsEnd(void) const
{
	// アニメーションが再生されていない・ループアニメーション時、false
	if (playType_ == -1 || isLoop_)
	{
		return false;
	}

	auto animation = animations_.find(playType_);

	if (animation == animations_.end())
	{
		return false;
	}

	// 再生時間が最大再生時間を超えたら、true
	return (animation->second.step >= animation->second.totalTime);
}

bool AnimationController::IsEndPoint(float _pointStart, float _pointEnd)
{
	if (playType_ == -1)
	{
		return false;
	}
 
	// 判定開始位置
	float start = std::clamp(_pointStart, 0.0f, 1.0f);
 
	// 判定終了位置
	float end = std::clamp(_pointEnd, 0.0f, 1.0f);
 
	// 再生位置の割合
	float curRate = GetPlayPointRate();
 
	// 再生位置が指定の範囲内になったとき、true
	return (curRate >= start && curRate < end);
}

float AnimationController::GetPlayPointRate(void)
{
	/* アニメーション再生割合を取得 */
	Animation* animation = FindAnimation(animations_, playType_);
 
	// 未再生、または総再生時間が未取得の時は0
	if (animation == nullptr || animation->totalTime <= 0.0f)
	{
		return 0.0f;
	}
 
	return (animation->step / animation->totalTime);
}

void AnimationController::Stop(float _stopTime)
{
	// 停止処理
	isStop_ = true;
	timeStop_ = _stopTime;
}

void AnimationController::SetAnimationStep(float _step)
{
	Animation* animation = FindAnimation(animations_, playType_);
	if (animation == nullptr) { return; }

	// 再生位置を範囲内に制限して割り当て
	animation->step = std::clamp(_step, 0.0f, animation->totalTime);
}

void AnimationController::SetAnimationStepRate(float _rate)
{
	Animation* animation = FindAnimation(animations_, playType_);
	if (animation == nullptr) { return; }

	// 割合を範囲内に制限
	float rate = std::clamp(_rate, 0.0f, 1.0f);

	// 割合から再生位置を割り当て
	animation->step = (animation->totalTime * rate);
}

void AnimationController::Add(int _type, Animation& _animation)
{
	// 同じ種類が登録済みの場合は追加されない
	animations_.emplace(_type, _animation);
}