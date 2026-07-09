#pragma once
#include <array>

class GameTimer
{
public:

	/// @brief コンストラクタ
	/// @param _gameTime ゲーム最大時間
	GameTimer(float _gameTime);

	void Update(void);
	void Draw(void);


	/// @brief カウントダウン描画処理
	/// @param _time カウントダウン時間
	/// @param _startTextUi げーむすたーとUI画像ハンドル
	void DrawCountDown(float _time, int _startTextUi);

	/// @brief 現在時間取得
	float GetTime(void)const { return curTimer_; };

	void SetTime(float _time);

	/// @brief 時間を動かすか否か割り当て
	void SetIsTimeActive(bool _isActive) { isTimeActive_ = _isActive; };

	/// @brief 時間を動かしているか否か
	bool GetIsTimeActive(void)const { return isTimeActive_; };


private:

	// 数字の数(0～9)+秒
	static constexpr int TIME_NUM = 11;

	// 画像の"秒"の格納番地
	static constexpr int STORAGE_NUM_SECOND = (TIME_NUM - 1);


	// 数字の画像
	std::array<int, TIME_NUM> timeText_;

	// 現在時間
	float curTimer_;

	// 時間を変動させるか否か
	bool isTimeActive_;

};