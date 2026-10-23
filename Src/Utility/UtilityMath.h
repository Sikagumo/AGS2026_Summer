#pragma once

#include <string>
#include <vector>
#include <DxLib.h>

#include "../Common/Vector2.h"
#include "../Common/Quaternion.h"

/// @brief 各種ベクトル・角度・補間などのユーティリティ関数群を提供する静的クラス
class UtilityMath
{
public:

	// ラジアンから度への変換定数(float)
	static constexpr float RAD2DEG = (180 / DX_PI_F);

	// 度からラジアンへの変換定数(float)
	static constexpr float DEG2RAD = (DX_PI_F / 180.0f);

	/// @brief ゼロベクトル(0, 0, 0)
	static constexpr VECTOR VECTOR_ZERO = { 0.0f, 0.0f, 0.0f };
	static constexpr Vector2 VECTOR2_ZERO = { 0, 0 };
	static constexpr Vector2F VECTOR2F_ZERO = { 0.0f, 0.0f };

	/// @brief 単位ベクトル(1, 1, 1)
	static constexpr VECTOR VECTOR_ONE = { 1.0f, 1.0f, 1.0f };
	static constexpr Vector2 VECTOR2_ONE = { 1, 1 };
	static constexpr Vector2F VECTOR2F_ONE = { 1.0f, 1.0f };

	//回転軸

	// X軸方向の単位ベクトル
	static constexpr VECTOR AXIS_X = { 1.0f, 0.0f, 0.0f };

	// Y軸方向の単位ベクトル
	static constexpr VECTOR AXIS_Y = { 0.0f, 1.0f, 0.0f };

	// Z軸方向の単位ベクトル
	static constexpr VECTOR AXIS_Z = { 0.0f, 0.0f, 1.0f };

	//方向

	// 前方方向 (Z+)
	static constexpr VECTOR DIR_FORWARD = { 0.0f, 0.0f, 1.0f };

	// 後方方向 (Z-)
	static constexpr VECTOR DIR_BACK = { 0.0f, 0.0f, -1.0f };

	// 右方向 (X+)
	static constexpr VECTOR DIR_R = { 1.0f, 0.0f, 0.0f };

	// 左方向 (X-)
	static constexpr VECTOR DIR_L = { -1.0f, 0.0f, 0.0f };

	// 上方向 (Y+)
	static constexpr VECTOR DIR_UP = { 0.0f, 1.0f, 0.0f };

	// 下方向 (Y-)
	static constexpr VECTOR DIR_DOWN = { 0.0f, -1.0f, 0.0f };

	// 浮動小数点の誤差比較用の最小値
	static constexpr float kEpsilonNormalSqrt = 1e-15F;

	// 0.5を表す定数
	static constexpr float HALF_NUM = 0.5f;

	/// @brief 小数を四捨五入して整数に変換する
	/// @param _value 対象の値
	/// @return 四捨五入された整数
	static int Round(float _value);

	/// @brief 文字列を指定文字で分割する
	/// @param _line 分割対象の文字列
	/// @param _delimiter 区切り文字
	/// @return 分割された文字列の配列
	static std::vector<std::string> Split(std::string& _line, char _delimiter);

	/// @brief ラジアンから度(double)へ変換
	/// @param _rad ラジアン角
	/// @return 度
	static double Rad2DegD(double _rad);

	/// @brief ラジアンから度(float)へ変換
	/// @param _rad ラジアン角
	/// @return 度
	static float Rad2DegF(float _rad);

	/// @brief ラジアンから度(int)へ変換
	/// @param _rad ラジアン角
	/// @return 度
	static int Rad2DegI(int _rad);

	/// @brief 度からラジアンに変換(double)
	/// @param _deg 度数値
	/// @return ラジアン値
	static double Deg2RadD(double _deg);

	/// @brief 度からラジアンに変換(float)
	/// @param _deg 度数値
	/// @return ラジアン値
	static float Deg2RadF(float _deg);

	/// @brief 度からラジアンに変換(int)
	/// @param _deg 度数値
	/// @return ラジアン値
	static int Deg2RadI(int _deg);

	/// @brief 角度を0～360に正規化
	/// @param _deg 入力角度(度)
	/// @return 0～360度の範囲に正規化された角度
	static double DegIn360(double _deg);

	/// @brief 角度を0～2πに正規化
	/// @param _rad 入力角度(ラジアン)
	/// @return 0～2πの範囲に正規化された角度
	static double RadIn2PI(double _rad);

	/// @brief 回転が少ない方の方向を判定(ラジアン)。時計回りなら1、反時計回りなら-1を返す。
	/// @param _from 開始角度(ラジアン)
	/// @param _to 終了角度(ラジアン)
	/// @return 回転方向(1: 時計回り, -1: 反時計回り)
	static int DirNearAroundRad(float _from, float _to);

	/// @brief 回転が少ない方の方向を判定(度)。時計回りなら1、反時計回りなら-1を返す。
	/// @param _from 開始角度(度)
	/// @param _to 終了角度(度)
	/// @return 回転方向(1: 時計回り, -1: 反時計回り)
	static int DirNearAroundDeg(float _from, float _to);

	//線形補間

	/// @brief 線形補間(int)
	/// @param _start 開始値
	/// @param _end 終了値
	/// @param _term 補間係数(0～1)
	/// @return 補間後の値
	static int Lerp(int _start, int _end, float _term);

	/// @brief 線形補間(float)
	/// @param _start 開始値
	/// @param _end 終了値
	/// @param _term 補間係数(0～1)
	/// @return 補間後の値
	static float Lerp(float _start, float _end, float _term);

	/// @brief 線形補間(double)
	/// @param _start 開始値
	/// @param _end 終了値
	/// @param _term 補間係数(0～1)
	/// @return 補間後の値
	static double Lerp(double _start, double _end, double _term);

	/// @brief 線形補間(Vector2)
	/// @param _start 開始ベクトル
	/// @param _end 終了ベクトル
	/// @param _term 補間係数(0～1)
	/// @return 補間後のベクトル
	static Vector2 Lerp(const Vector2& _start, const Vector2& _end, float _term);

	/// @brief 線形補間(VECTOR)
	/// @param _start 開始ベクトル
	/// @param _end 終了ベクトル
	/// @param _term 補間係数(0～1)
	/// @return 補間後のベクトル
	static VECTOR Lerp(const VECTOR& _start, const VECTOR& _end, float _term);

	/// @brief 角度の線形補間(度)
	/// @param _start 開始角度(度)
	/// @param _end 終了角度(度)
	/// @param "_term 補間係数(0～1)
	/// @return 補間後の角度(度)
	static double LerpDeg(double _start, double _end, double t);

	/// @brief 色の線形補間
	/// @param _start 開始色
	/// @param _end 終了色
	/// @param _term 補間係数(0～1)
	/// @return 補間後の色
	static COLOR_F Lerp(const COLOR_F& _start, const COLOR_F& _end, float _term);

	/// @brief 2Dベジェ曲線(Vector2)での位置計算
	/// @param _point1 開始点
	/// @param _point2 中間点
	/// @param _point3 終了点
	/// @param _term 補間係数(0～1)
	/// @return 補間後の位置(ベジェ曲線上)
	static Vector2 Bezier(const Vector2& _point1, const Vector2& _point2, const Vector2& _point3, float _term);

	/// @brief 3Dベジェ曲線(VECTOR)での位置計算
	/// @param _point1 開始点
	/// @param _point2 中間点
	/// @param _point3 終了点
	/// @param _term 補間係数(0～1)
	/// @return 補間後の位置(ベジェ曲線上)
	static VECTOR Bezier(const VECTOR& _point1, const VECTOR& _point2, const VECTOR& _point3, float _term);

	/// @brief Y軸を中心としたXZ平面での回転座標を求める
	/// @param _centerPos 回転中心位置
	/// @param _radiusPos 回転させる対象の位置
	/// @param _rad 回転角度(ラジアン)
	/// @return 回転後の位置
	static VECTOR RotXZPos(const VECTOR& _centerPos, const VECTOR& _radiusPos, float _rad);

	/// @brief ベクトルの長さ(2D)
	/// @param _vec 対象のベクトル(2D)
	/// @return ベクトルの長さ
	static double Magnitude(const Vector2& _vec);

	/// @brief ベクトルの長さ(3D)
	/// @param _vec 対象のベクトル(3D)
	/// @return ベクトルの長さ
	static double Magnitude(const VECTOR& _vec);

	/// @brief ベクトルの長さ(3D・float版)
	/// @param _vec 対象のベクトル(3D)
	/// @return ベクトルの長さ
	static float MagnitudeF(const VECTOR& _vec);

	/// @brief ベクトルの長さの2乗(2D)
	/// @param _vec 対象のベクトル(2D)
	/// @return ベクトルの長さの2乗
	static int SqrMagnitude(const Vector2& _vec);

	/// @brief ベクトルの長さの2乗(3D・float版)
	/// @param _vec 対象のベクトル(3D)
	/// @return ベクトルの長さの2乗
	static float SqrMagnitudeF(const VECTOR& _vec);

	/// @brief ベクトルの長さの2乗(3D)
	/// @param _vec 対象のベクトル(3D)
	/// @return ベクトルの長さの2乗
	static double SqrMagnitude(const VECTOR& _vec);

	/// @brief 2点間の距離の2乗(3D)
	/// @param _vec1 開始ベクトル(3D)
	/// @param _vec2 終了ベクトル(3D)
	/// @return 2点間の距離の2乗
	static double SqrMagnitude(const VECTOR& _vec1, const VECTOR& _vec2);

	/// @brief 2点間の距離(2D)
	/// @param _vec1 開始ベクトル(2D)
	/// @param _vec2 終了ベクトル(2D)
	/// @return 2点間の距離
	static double Distance(const Vector2& _vec1, const Vector2& _vec2);

	/// @brief 2点間の距離(3D)
	/// @param _vec1 開始ベクトル(3D)
	/// @param _vec2 終了ベクトル(3D)
	/// @return 2点間の距離
	static double Distance(const VECTOR& _vec1, const VECTOR& _vec2);

	
	/// @brief 2つのベクトルが等しいか判定
	/// @param _vec1 比較するベクトル１
	/// @param _vec2 比較するベクトル２
	static bool Equals(const VECTOR& _vec1, const VECTOR& _vec2);

	/// @brief ベクトルがゼロベクトルか判定
	/// @param _vec 対象のベクトル
	static bool EqualsVZero(const VECTOR& _vec);
	static bool EqualsVZero(const Vector2& _vec);
	static bool EqualsVZero(const Vector2F& _vec);


	/// @brief 2Dベクトルを正規化し3Dベクトルに変換
	/// @param _vec 対象の2Dベクトル
	static VECTOR Normalize(const Vector2& _vec);

	
	/// @brief ベクトルを正規化
	/// @param _vec 対象の3Dベクトル
	static VECTOR VNormalize(const VECTOR& _vec);
	static Vector2 VNormalize(const Vector2& _vec);
	static Vector2F VNormalize(const Vector2F& _vec);

	/// @brief 2つのベクトルの間の角度(度)を返す
	/// @param _from 開始ベクトル
	/// @param _to 終了ベクトル
	/// @return ベクトル間の角度(度)
	static double AngleDeg(const VECTOR& _from, const VECTOR& _to);

	//描画系

	/// @brief 指定位置から方向ベクトルに向けて線を描画する
	/// @param _pos 開始位置
	/// @param _dir 方向ベクトル
	/// @param color 色(DxLibの色コード)
	/// @param len 線の長さ(デフォルトは50.0f)
	static void DrawLineDir(const VECTOR& _pointos, const VECTOR& _dir, int color, float len = 50.0f);

	/// @brief 指定位置から回転行列の軸方向に線を描画する
	/// @param _pos 開始位置
	/// @param rot 回転行列
	/// @param len 線の長さ(デフォルトは50.0f)
	static void DrawLineXYZ(const VECTOR& _pointos, const MATRIX& rot, float len = 50.0f);

	/// @brief 指定位置からクォータニオンの軸方向に線を描画する
	/// @param _pos 開始位置
	/// @param rot クォータニオン回転
	/// @param len 線の長さ(デフォルトは50.0f)
	static void DrawLineXYZ(const VECTOR& _pointos, const Quaternion& rot, float len = 50.0f);

	/// @brief 指定した中心点から円周上の位置を計算する
    /// @param _center 中心座標
    /// @param _radius 半径
    /// @param angle 角度(ラジアン)
    /// @return 円周上の位置座標
	static VECTOR GetCirclePos(const VECTOR& _center, float _radius, float angle);

	/// @brief ランダムな数値を返す(float用)
	/// @param _min 最小値
	/// @param _max 最大値 
	static float RandRangeF(float _min, float _max);

	/// @brief 線分上の最もターゲットに近い座標を算出
	/// @param _startPos 線分の開始点
	/// @param _endPos 線分の終了点
	/// @param targetPos ターゲット座標
	/// @return 線分上の最近接座標
	static VECTOR GetNearestPointOnSegment(const VECTOR& __startPos,
		const VECTOR& __endPos, const VECTOR& _targetPos);
};

