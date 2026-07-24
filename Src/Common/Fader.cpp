#include <DxLib.h>
#include "../Application.h"
#include "Fader.h"
#include "../Manager/Generic/ResourceManager.h"

Fader::Fader(void)
	:
	state_(STATE::NONE),
	alpha_(0.0f),
	isPreEnd_(true),
	isEnd_(true),
	fadeImageHandle_(-1)
{
}

Fader::~Fader(void)
{
	if (fadeImageHandle_ != -1)
	{
		DeleteGraph(fadeImageHandle_);
	}
}

Fader::STATE Fader::GetState(void) const
{
	return state_;
}

bool Fader::IsEnd(void) const
{
	return isEnd_;
}

void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ != STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}
}

void Fader::Init(void)
{
}

void Fader::LoadFadeImage(void)
{

	fadeImageHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_AICON);
}

void Fader::Update(void)
{
	if (isEnd_)
	{
		// フェードが完全に終了した状態で画像が残っていれば自動で破棄する
		if (fadeImageHandle_ != -1)
		{
			fadeImageHandle_ = -1;
		}

		return;
	}

	switch (state_)
	{
	case STATE::NONE:
		return;

	case STATE::FADE_OUT:
		alpha_ += SPEED_ALPHA;
		if (alpha_ > 255.0f)
		{
			// フェード終了
			alpha_ = 255.0f;
			if (isPreEnd_)
			{
				// 1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}

		break;

	case STATE::FADE_IN:
		alpha_ -= SPEED_ALPHA;
		if (alpha_ < 0.0f)
		{
			// フェード終了
			alpha_ = 0.0f;
			if (isPreEnd_)
			{
				// 1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}
		break;

	default:
		return;
	}

}

void Fader::Draw(void)
{
	switch (state_)
	{
	case STATE::NONE:
		return;
	case STATE::FADE_OUT:
	case STATE::FADE_IN:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));

		if (fadeImageHandle_ != -1)
		{

			// 画像がない場合は通常の黒フェード
			DrawBox(
				0, 0,
				Application::SCREEN_SIZE_X,
				Application::SCREEN_SIZE_Y,
				0x000000, true);

			DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y,
				1.0f, 0.0f, fadeImageHandle_, true);
		}
		else
		{
			// 画像がない場合は通常の黒フェード
			DrawBox(
				0, 0,
				Application::SCREEN_SIZE_X,
				Application::SCREEN_SIZE_Y,
				0x000000, true);
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	}

}