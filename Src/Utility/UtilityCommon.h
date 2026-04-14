#pragma once
#include <string>
#include "../Common/Vector2.h"
#include <DxLib.h>

class UtilityCommon
{
public:

	// 色の割合(0～255)
	struct Color
	{
		unsigned int r = 0, g = 0, b = 0, a = 255;
	};

	
	static constexpr int COLOR_RATE_MAX = 255;

	/// @brief 色を16進数で生成
	static unsigned int SetColor(Color _color);

	/// @brief 色を16進数で生成
	static unsigned int SetColor(COLOR_F _color);

	/// @brief 色を16進数で生成
	/// @param _red 赤の割合(0.1～1.0)
	/// @param _green 緑の割合(0.1～1.0)
	/// @param _blue 青の割合(0.1～1.0)
	static unsigned int SetColor(float _red, float _green, float _blue);

	/// @brief 色の割合を float化
	/// @param _color 色の割合(0 ～ 255)
	/// @return 色の割合(0.0 ～ 1.0)
	static COLOR_F GetColorRate(const Color& _color);


	/// @brief 文字列→int型変換																							
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	/// @param _minNum 最小値
	static void ChangeString(const std::string& _text, int& _target, int _minNum);

	/// @brief 文字列→int型変換
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	static void ChangeString(const std::string& _text, int& _target);

	/// @brief 文字列→float型変換
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	/// @param _minNum 最小値
	static void ChangeString(const std::string& _text, float& _target, float _minNum);

	/// @brief 文字列→float型変換
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	static void ChangeString(const std::string& _text, float& _target);


	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _vec 対象
	/// @param _maxVec 最大値
	/// @param _minVec 最小値
	static bool WrapValue(VECTOR& _vec, const VECTOR& _maxVec, const VECTOR& _minVec = {0.0f, 0.0f, 0.0f});

	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _vec 対象
	/// @param _maxVec 最大値
	/// @param _minVec 最小値
	/// @return 
	static bool WrapValue(Vector2& _vec, const Vector2& _maxVec, const Vector2& _minVec = {0, 0});

	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _num 対象
	/// @param _maxNum 最大値
	/// @param _minNum 最小値
	/// @return 
	static bool WrapValue(int& _num, int _maxNum, int _minNum = 0);

	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _num 対象
	/// @param _maxNum 最大値
	/// @param _minNum 最小値
	static bool WrapValue(float& _num, float _maxNum, float _minNum = 0.0f);


	/// @brief 線形補間
	/// @param _curNum 現在値
	/// @param _maxNum 最大値
	/// @param _delta 累乗の値
	/// @return 補間後の値
	static float EasingLine(float _curNum, float _maxNum, float _powNum);
};