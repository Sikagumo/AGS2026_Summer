#include "AnimationController.h"
#include <DxLib.h>
#include <string>
#include <cassert>
#include <algorithm>
#include "../../Manager/SceneManager.h"

AnimationController::AnimationController(int _modelId)
{
	animations_.clear();

	modelId_ = _modelId;

	playType_ = prePlayType_ = -1;

	blendTime_ = curBlendTime_ = 0.0f;

	isLoop_	  = false;
}

AnimationController::~AnimationController(void)
{
	Release();
}


void AnimationController::AddInternal(int _type, float _speed)
{
	/* 内部のアニメーションの追加 */
	Animation anim;

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
	Animation anim;

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

void AnimationController::Play(int _type, bool _isLoop, float _blendTime)
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
	if (isStop_) return;

	// 再生中のアニメーション
	if (playType_ != -1)
	{
		// アニメーション進行処理
		curAnim.step += (deltaTime * curAnim.speed);
		
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
	auto& anim = animations_.at(playType_);

	// 再生位置
	float start = std::clamp(_pointStart, 0.0f, 1.0f);

	// 終了位置
	float end = std::clamp(_pointEnd, 0.0f, 1.0f);

	// 再生位置の割合
	float curRate = (anim.step / anim.totalTime);

	// 再生位置が指定の割合になったときtrue
	return (curRate >= start && curRate < end);
}

void AnimationController::SetAnimStep(float step)
{
	auto& anim = animations_.at(playType_);
	// 再生位置がマイナス時、０にする
	step = ((step < 0.0f) ? 0.0f : step);

	// 再生位置が最大時間を超えたとき、最大時間にする
	step = ((step > anim.totalTime) ? anim.totalTime : step);

	if (playType_ != -1)
	{
		// 再生位置割り当て
		anim.step = step;
	}
}

void AnimationController::SetAnimStepRate(float _rate)
{
	auto& anim = animations_.at(playType_);

	float step = _rate;

	// 再生位置の上限制限
	step = ((step < 0.0f) ? 0.0f : step);
	step = ((step > 1.0f) ? 1.0f : step);

	// 割合の値
	float rate = (1.0f / anim.totalTime);

	if (playType_ != -1)
	{
		// 再生位置割り当て
		anim.step = (rate * step);
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