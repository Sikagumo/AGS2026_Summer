#include "AnimationController.h"
#include <DxLib.h>
#include <string>
#include <cassert>
#include <algorithm>
#include "../../Manager/Generic/SceneManager.h"

AnimationController::AnimationController(int _modelId)
	: animations_{}
	, modelId_(_modelId)
	, playType_(-1), prePlayType_(-1)
	, blendTime_(0.0f), curBlendTime_(0.0f)
	, isLoop_(false)
	, isStop_(false), timeStop_(0.0f)
	, playSpeed_(0.0f)
{
}


AnimationController::~AnimationController(void)
{
	Release();
}


void AnimationController::AddInternal(int _type, float _speed)
{
	/* 内部のアニメーションの追加 */
	Animation anim = Animation();

	anim.animIndex = _type;

	// アニメーション速度割り当て
	anim.speed = _speed;

	// アニメーション状態割り当て
	anim.type = ANIM_TYPE::INTERNAL;

	anim.step = 0.0f;

	// アニメーション追加処理
	Add(_type, anim);
}
void AnimationController::AddExternal(int _type, float _speed, const std::string _path)
{
	/* 外部のアニメーションの追加 */
	Animation anim = Animation();

	anim.modelId = MV1LoadModel(_path.c_str());

	// パス読み込み判定有効
	anim.isLoadPath = true;

	// アニメーション速度割り当て
	anim.speed = _speed;

	// アニメーション状態割り当て
	anim.type = ANIM_TYPE::EXTERNAL;

	anim.step = 0.0f;

	// アニメーション追加処理
	Add(_type, anim);
}
void AnimationController::AddExternal(int _type, float _speed, int _handle)
{
	/* 外部のアニメーションの追加 */
	Animation anim;

	anim.modelId = _handle;

	// アニメーション速度割り当て
	anim.speed = _speed;

	// アニメーション状態割り当て
	anim.type = ANIM_TYPE::EXTERNAL;

	anim.step = 0.0f;

	// アニメーション追加処理
	Add(_type, anim);
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
	}

	auto& playAnim = animations_[_type];

	// 初期化
	playAnim.step = 0.0f;

	// アニメーションループ
	isLoop_ = _isLoop;

	// 停止を解除
	isStop_ = false;

	// アニメーション種別を変更
	playType_ = _type;

	// ブレンド時間初期化
	curBlendTime_ = 0.0f;

	blendTime_ = _blendTime;

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
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
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

	if (prePlayType_ != -1)
	{
		// ブレンドタイマー増加
		curBlendTime_ += deltaTime;
		
		// ブレンド時間が割り当てているときは、現在時間と最大時間の割合を、それ以外はタイマーを終了
		float time = ((blendTime_ > 0.0f) ? (curBlendTime_ / blendTime_) : 1.0f);

		// 旧・新規アニメーションのブレンド率を割り当て
		MV1SetAttachAnimBlendRate(modelId_, preAnim.attachNo, (1.0f - time));
		MV1SetAttachAnimBlendRate(modelId_, curAnim.attachNo, time);

		// ブレンドアニメーション終了時
		if (time >= 1.0f)
		{
			// 前アニメーションをデタッチ
			MV1DetachAnim(modelId_, preAnim.attachNo);
			prePlayType_ = -1;

			// 新規アニメーションのブレンド率を100%にする
			MV1SetAttachAnimBlendRate(modelId_, curAnim.attachNo, 1.0f);
		}
	}
}

void AnimationController::DrawDebug(void)
{
#ifdef _DEBUG
	if (playType_ == -1) { return; }
	auto& anim = animations_.at(playType_);

	// アニメーションの描画
	DrawFormatString(0,64,0xFF0000,"animTime:%.2f",anim.step);
#endif // _DEBUG
}

void AnimationController::Release(void)
{
	if (animations_.empty()) return;

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

	Animation& anim = animations_.at(playType_);

	// 再生位置
	float start = std::clamp(_pointStart, 0.0f, 1.0f);

	// 終了位置
	float end = std::clamp(_pointEnd, 0.0f, 1.0f);

	// 再生位置の割合
	float curRate = (anim.step / anim.totalTime);

	// 再生位置が指定の割合になったときtrue
	return (curRate >= start && curRate < end);
}
float AnimationController::GetPlayPointRate(void)
{
	/* アニメーション再生割合を取得 */

	if (playType_ == -1) { return 0.0f; }

	Animation& anim = animations_.at(playType_);
	float ret = (anim.step / anim.totalTime);

	return ret;
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
	bool ret = false;

	auto it = animations_.find(_type);
	if (it != animations_.end())
	{
		// 発見
		ret = true;
	}
#ifdef _DEBUG
	else
	{
		OutputDebugString("\nアニメーションの情報がありません。\n");
	}
#endif

	return ret;
}