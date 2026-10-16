#include "Fader.h"

#include <DxLib.h>

#include "../Application.h"
#include "../Manager/Generic/ResourceManager.h"

namespace
{
    // アルファ値定義関連
    constexpr float ALPHA_MAX = 255.0f; // 透明度の最大値
    constexpr float ALPHA_MIN = 0.0f;   // 透明度の最小値
}

Fader::Fader(void)
    : state_(STATE::NONE)
    , alpha_(ALPHA_MIN)
    , isPreEnd_(true)
    , isEnd_(true)
    , fadeImageHandle_(-1)
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

void Fader::SetFade(STATE _state)
{
    state_ = _state;
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
    fadeImageHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_ICON);
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
    {
        return;
    }
    case STATE::FADE_OUT:
    {
        alpha_ += SPEED_ALPHA;
        if (alpha_ > ALPHA_MAX)
        {
            // フェード終了
            alpha_ = ALPHA_MAX;
            if (isPreEnd_)
            {
                // 1フレーム後に終了とする
                isEnd_ = true;
            }
            isPreEnd_ = true;
        }

        break;
    }
    case STATE::FADE_IN:
    {
        alpha_ -= SPEED_ALPHA;
        if (alpha_ < ALPHA_MIN)
        {
            // フェード終了
            alpha_ = ALPHA_MIN;
            if (isPreEnd_)
            {
                // 1フレーム後に終了とする
                isEnd_ = true;
            }
            isPreEnd_ = true;
        }
        break;
    }
    default:
    {
        return;
    }
    }
}

void Fader::Draw(void)
{

    switch (state_)
    {
    case STATE::NONE:
    {
        return;
    }
    case STATE::FADE_OUT:
    case STATE::FADE_IN:
    {
        // 描画関連
        constexpr unsigned int COLOR_BLACK = 0x000000;  // 黒色
        constexpr float IMAGE_SCALE = 1.0f;             // 画像の描画スケール
        constexpr float IMAGE_ROTATION = 0.0f;          // 画像の回転角
        constexpr int DRAW_POSITION_X = 0;              // 描画開始X座標
        constexpr int DRAW_POSITION_Y = 0;              // 描画開始Y座標
        constexpr int SCREEN_HALF_DIVISOR = 2;          // 画面中央を求めるための除数
        constexpr int BLEND_NO_BLEND_PARAM = 0;         // ノーブレンド時のパラメータ

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));

        if (fadeImageHandle_ != -1)
        {
            DrawBox(
                DRAW_POSITION_X,
                DRAW_POSITION_Y,
                Application::SCREEN_SIZE_X,
                Application::SCREEN_SIZE_Y,
                COLOR_BLACK,
                true
            );

            int centerX = Application::SCREEN_SIZE_X / SCREEN_HALF_DIVISOR;
            int centerY = Application::SCREEN_SIZE_Y / SCREEN_HALF_DIVISOR;

            DrawRotaGraph(
                centerX,
                centerY,
                IMAGE_SCALE,
                IMAGE_ROTATION,
                fadeImageHandle_,
                true
            );
        }
        else
        {
            // 画像がない場合は通常の黒フェード
            DrawBox(
                0, 0,
                Application::SCREEN_SIZE_X,
                Application::SCREEN_SIZE_Y,
                COLOR_BLACK, true);
        }

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, BLEND_NO_BLEND_PARAM);
        break;
    }
    }
}