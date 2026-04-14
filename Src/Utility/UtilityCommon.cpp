#include "UtilityCommon.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include <math.h>
#include "./AsoUtility.h"


void UtilityCommon::ChangeString(const std::string& _text, int& target, int minNum)
{
    /* 文字列→int */

    std::string txt = "";

    // マイナス
    const char& minus = *"-";

    // 文字が空白のとき、最小値にする
    if (_text == "" || _text == " ")
    {
        target = minNum;
        return;
    }

    for (char t : _text)
    {
        // 数字・マイナスではない文字列の時、終了
        if (!std::isdigit(static_cast<unsigned char>(t)) &&
            t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }
    
    
    if (!txt.empty())
    {    
        // 文字列→int変換
        int num = stoi(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < minNum) ? minNum : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！int→文字列変換の値に誤りがあります。！！\n");
        assert(false);
    }
}

void UtilityCommon::ChangeString(const std::string& text, int& target)
{
    /* 文字列→int */

    std::string txt = "";

    // マイナス
    const char& minus = *"-";


    // 文字が空白のとき、0にする
    if (text == "" || text == " ")
    {
        target = 0;
        return;
    }

    for (char t : text)
    {
        // 数字・マイナスではない文字列のとき、終了
        if (!isdigit(static_cast<unsigned char>(t)) &&
            t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }
    

    if (!txt.empty())
    {
        // 文字列→int変換
        int num = stoi(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < 0) ? 0 : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！int→文字列変換の値に誤りがあります。！！\n");
        assert(false);
    }
}

void UtilityCommon::ChangeString(const std::string& text, float& target, float minNum)
{
    /* 文字列→float */

    std::string txt = "";

    // 小数点
    const char& dot = *".";

    // マイナス
    const char& minus = *"-";


    // 文字が空白のとき、最小値の文字列にする
    if (text == "" || text == " ")
    {
        target = minNum;
        return;
    }

    for (auto t : text)
    {
        // 数字・小数点・マイナスではない文字列のとき、終了
        if (!isdigit(static_cast<unsigned char>(t)) &&
            t != dot && t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }

    if (!txt.empty())
    {
        // 文字列→float変換
        float num = stof(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < minNum) ? minNum : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！float→文字列変換の値に誤りがあります。！！\n");
        assert(false);

    }
}

void UtilityCommon::ChangeString(const std::string& text, float& target)
{
    /* 文字列→float */

    /* 文字列→float */
    std::string txt = "";

    // 小数点
    const char& dot = *".";

    // マイナス
    const char& minus = *"-";


    // 文字が空白のとき、0にする
    if (text == "" || text == " ")
    {
        txt = std::to_string(0.0f);
        return;
    }


    for (auto t : text)
    {
        // 数字・小数点・マイナスではない文字列のとき、終了
        if (!isdigit(static_cast<unsigned char>(t)) &&
            t != dot && t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }

    if (!txt.empty())
    {
        // 文字列→float変換
        float num = stof(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < 0.0f) ? 0.0f : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！float→文字列変換の値に誤りがあります。！！\n");
        assert(false);
    }
}


unsigned int UtilityCommon::SetColor(UtilityCommon::Color _color)
{
    return GetColor(_color.r, _color.g, _color.b);
}
unsigned int UtilityCommon::SetColor(COLOR_F _color)
{
    if (_color.r > 1.0f || _color.g > 1.0f || _color.b > 1.0f)
    {
        OutputDebugString("\n引数の値が1.0以上になっています...\n");
    }

    int r, g, b;
    r = static_cast<int>(_color.r * COLOR_RATE_MAX);
    g = static_cast<int>(_color.g * COLOR_RATE_MAX);
    b = static_cast<int>(_color.b * COLOR_RATE_MAX);
    return GetColor(r, g, b);
}

unsigned int UtilityCommon::SetColor(float _red, float _green, float _blue)
{
    if (_red > 1.0f || _green > 1.0f || _blue > 1.0f)
    {
        OutputDebugString("\n色の引数の値が1.0以上になっています...\n");
        return GetColor(_red, _green, _blue);
    }
    int r, g, b;
    r = static_cast<int>(_red * COLOR_RATE_MAX);
    g = static_cast<int>(_green * COLOR_RATE_MAX);
    b = static_cast<int>(_blue * COLOR_RATE_MAX);
    return GetColor(r, g, b);
}

COLOR_F UtilityCommon::GetColorRate(const Color& _color)
{
    float r, g, b, a;
    r = static_cast<float>(_color.r) / 255;
    g = static_cast<float>(_color.g) / 255;
    b = static_cast<float>(_color.b) / 255;
    a = static_cast<float>(_color.a) / 255;

    return COLOR_F(r, g, b, a);
}

bool UtilityCommon::WrapValue(VECTOR& _vec, const VECTOR& _maxVec, const VECTOR& _minVec)
{
    // 最小値を超えている場合は最大値に、最大値を超えている場合は最小値で返す
    bool isWrap = (WrapValue(_vec.x, _maxVec.x, _minVec.x) ||
                   WrapValue(_vec.y, _maxVec.y, _minVec.y) ||
                   WrapValue(_vec.z, _maxVec.z, _minVec.z));
    return isWrap;
}
bool UtilityCommon::WrapValue(Vector2& _vec, const Vector2& _maxVec, const Vector2& _minVec)
{
    bool isWrap = (WrapValue(_vec.x, _maxVec.x, _minVec.x) ||
                   WrapValue(_vec.y, _maxVec.y, _minVec.y));
    return isWrap;
}
bool UtilityCommon::WrapValue(int& _num, int _maxNum, int _minNum)
{
    bool isWrap = false;

    // 最小値を超えている場合は最大値に、最大値を超えている場合は最小値で返す
    if (_num < _minNum)
    {
        isWrap = true;
        _num = _maxNum;
    }
    else if (_num > _maxNum)
    {
        isWrap = true;
        _num = _minNum;
    }

    return isWrap;
}
bool UtilityCommon::WrapValue(float& _num, float _maxNum, float _minNum)
{
    bool isWrap = false;

    // 最小値を超えている場合は最大値に、最大値を超えている場合は最小値で返す
    if (_num < _minNum)
    {
        isWrap = true;
        _num = _maxNum;
    }
    else if (_num > _maxNum)
    {
        isWrap = true;
        _num = _minNum;
    }

    return isWrap;
}

float UtilityCommon::EasingLine(float _curNum, float _maxNum, float _powNum)
{
    /* 線形補間処理 */
    float num = (_curNum / _maxNum);

    // 補正値を最大に補正
    if (num > 1.0f) { num = 1.0f; }

    return (1.0f - powf((1.0f - num), _powNum));
}
