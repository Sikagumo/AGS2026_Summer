#include "GameTimer.h"
#include <DxLib.h>
#include <algorithm>
#include "../Application.h"
#include "../Manager/System/TimeManager.h"
#include "../Manager/Generic/ResourceManager.h"

constexpr int TIME_POS_X = (Application::SCREEN_HALF_X - 350);
constexpr int TIME_POS_Y = (Application::SCREEN_HALF_Y + 10);

GameTimer::GameTimer(float _gameTime)
	: curTimer_(_gameTime)
	, isTimeActive_(false)
{
	std::fill(timeText_.begin(), timeText_.end(), -1);

	// 複数画像割り当て
	ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_TIMER, timeText_.data());
}

void GameTimer::Update(void)
{
	/* 更新処理 */
	if (isTimeActive_)
	{
		// 時間減少
		curTimer_ -= TimeManager::GetInstance().GetDeltaTime();

		// 時間が0の時、false
		isTimeActive_ = (curTimer_ > 0.0f);
	}
}
void GameTimer::Draw(void)
{
	/* 描画処理 */
	constexpr float UI_TEXT_SCALE = 0.5f;
	constexpr int UI_TEXT_SIZE = static_cast<int>((72.5f * UI_TEXT_SCALE));
	constexpr int TIME_OFFSET = 150;

	constexpr float MIN_TIME = 0.01f;
	float time = ((curTimer_ > 0.0f) ? curTimer_ : MIN_TIME) + 1;

	// 描画する数の位
	int arrayNum = 0;
	int x = TIME_POS_Y;

	// 小数点以下の数値
	float frac = (time - std::floor(time));

	// 100の位
	x += TIME_OFFSET;
	arrayNum = static_cast<int>(time / 100.0f);
	if (arrayNum > 0)
	{
		x += UI_TEXT_SIZE;
		DrawRotaGraph(x, UI_TEXT_SIZE,
			UI_TEXT_SCALE, 0.0, timeText_.at(arrayNum), true);
	}

	// 10の位
	x += UI_TEXT_SIZE;
	arrayNum = static_cast<int>(time / 10) % 10;
	DrawRotaGraph(x, UI_TEXT_SIZE,
		UI_TEXT_SCALE, 0.0, timeText_.at(arrayNum), true);

	// 1の位
	x += UI_TEXT_SIZE;
	arrayNum = static_cast<int>(time) % 10;
	DrawRotaGraph(x, UI_TEXT_SIZE,
		UI_TEXT_SCALE, 0.0, timeText_.at(arrayNum), true);

	// 単位
	x += UI_TEXT_SIZE;
	arrayNum = static_cast<int>(time) % 10;
	DrawRotaGraph(x, UI_TEXT_SIZE,
		UI_TEXT_SCALE, 0.0, timeText_.at(STORAGE_NUM_SECOND), true);
}

void GameTimer::DrawCountDown(float _time, int _uiHandle)
{
	/* カウントダウン描画処理 */

	// 画像スケール(カウントが０の場合は2倍、[げーむすたーと]描画時、1倍)
	const double SCALE = ((_time > 0.0f) ? 2.0 : 1.0);

	// 現在時間
	int arrayNum = static_cast<int>(_time);

	// カウントが0の時以外は[現在カウント+1]0未満では[げーむすたーと]を描画
	int image = ((_time > 0.0f)
		? timeText_.at(arrayNum + 1)
		: _uiHandle);


	DrawRotaGraph(Application::SCREEN_HALF_X, TIME_POS_Y
				  , SCALE, 0.0f, image, true);

}

void GameTimer::SetTime(float _time)
{
	float time = _time;
	curTimer_ = ((time > 0.0f) ? time : 0.0f);
}
