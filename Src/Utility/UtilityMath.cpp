#include "UtilityMath.h"
#include <DxLib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <cstdlib>


// 小数値を四捨五入して整数に変換する
int UtilityMath::Round(float _vec)
{
	return static_cast<int>(roundf(_vec));
}

// 文字列を指定された区切り文字で分割する
std::vector<std::string> UtilityMath::Split(std::string& _line, char _delimiter)
{
	std::istringstream stream(_line);
	std::string field;
	std::vector<std::string> result;

	while (getline(stream, field, _delimiter))
	{
		result.push_back(field);
	}

	return result;
}

// ラジアンを度に変換する（double）
double UtilityMath::Rad2DegD(double _rad)
{
	return _rad * (180.0 / DX_PI);
}

// ラジアンを度に変換する（float）
float UtilityMath::Rad2DegF(float _rad)
{
	return _rad * (180.0f / DX_PI_F);
}

// ラジアンを度に変換して整数にする
int UtilityMath::Rad2DegI(int _rad)
{
	return _rad * Round(180.0f / DX_PI_F);
}

// 度をラジアンに変換する（double）
double UtilityMath::Deg2RadD(double _deg)
{
	return _deg * (DX_PI / 180.0);
}

// 度をラジアンに変換する（float）
float UtilityMath::Deg2RadF(float _deg)
{
	return _deg * (DX_PI_F / 180.0f);
}

// 度をラジアンに変換して整数にする
int UtilityMath::Deg2RadI(int _deg)
{
	return _deg * Round(DX_PI_F / 180.0f);
}

// 角度を0～360度の範囲に正規化する
double UtilityMath::DegIn360(double _deg)
{
	_deg = fmod(_deg, 360.0);
	if (_deg < 0.0f)
	{
		_deg += 360.0;
	}

	return _deg;
}

// ラジアンを0～2πの範囲に正規化する
double UtilityMath::RadIn2PI(double _rad)
{
	_rad = fmod(_rad, DX_TWO_PI);
	if (_rad < 0.0)
	{
		_rad += DX_TWO_PI;
	}

	return _rad;
}

// ラジアン単位でどちら回りが近いかを判定する（-1=反時計回り、1=時計回り）
int UtilityMath::DirNearAroundRad(float _from, float _to)
{
	float ret = 1.0f;

	float diff = (_to - _from);

	if (diff >= 0.0f)
	{
		// 比較元よりも時計回りに位置する
		ret = ((diff > DX_PI_F) ? -1.0f : 1.0f);
	}
	else
	{
		// 比較元よりも反時計回りに位置する
		ret = ((diff < -DX_PI_F) ? 1.0f : -1.0f);
	}

	return static_cast<int>(ret);
}

// 度単位でどちら回りが近いかを判定する（-1=反時計回り、1=時計回り）
int UtilityMath::DirNearAroundDeg(float _from, float _to)
{
	float ret = 1.0f;

	float diff = (_to - _from);

	if (diff >= 0.0f)
	{
		//比較元よりも時計回りに位置する
		ret = ((diff > 180.0f) ? -1.0f : 1.0f);
	}
	else
	{
		//比較元よりも反時計回りに位置する
		ret = ((diff < -180.0f) ? 1.0f : -1.0f);
	}

	return static_cast<int>(ret);
}

// 整数値を線形補間する
int UtilityMath::Lerp(int _start, int _end, float _term)
{
	//線形補間
	if (_term >= 1.0f)
	{
		return _end;
	}

	return (_start + Round(_term * static_cast<float>(_end - _start)));;
}

// 浮動小数点値を線形補間する
float UtilityMath::Lerp(float _start, float _end, float _term)
{
	//線形補間
	if (_term >= 1.0f)
	{
		return _end;
	}

	return (_start + (_term * (_end - _start)));
}

// 倍精度浮動小数点値を線形補間する
double UtilityMath::Lerp(double _start, double _end, double _term)
{
	//線形補間
	if (_term >= 1.0)
	{
		return _end;
	}

	return (_start + (_term * (_end - _start)));;
}

// 2Dベクトルを線形補間する
Vector2 UtilityMath::Lerp(const Vector2& _start, const Vector2& _end, float _term)
{
	//線形補間
	if (_term >= 1.0f)
	{
		return _end;
	}

	return Vector2(_start.x + Round(_term * static_cast<float>((_end.x - _start.x)))
				 , _start.y + Round(_term * static_cast<float>((_end.y - _start.y))));
}

// 3Dベクトルを線形補間する
VECTOR UtilityMath::Lerp(const VECTOR& _start, const VECTOR& _end, float _term)
{
	//線形補間
	if (_term >= 1.0f)
	{
		return _end;
	}

	return VGet(_start.x + _term * (_end.x - _start.x)
		, _start.y + _term * (_end.y - _start.y)
		, _start.z + _term * (_end.z - _start.z));
}

// 度単位で角度を線形補間し、360度を考慮する
double UtilityMath::LerpDeg(double _start, double _end, double _term)
{
	double ret;

	double diff = _end - _start;

	if (diff < -180.0)
	{
		_end += 360.0;

		ret = Lerp(_start, _end, _term);

		if (ret >= 360.0)
		{
			ret -= 360.0;
		}
	}
	else if (diff > 180.0)
	{
		_end -= 360.0;

		ret = Lerp(_start, _end, _term);

		if (ret < 0.0)
		{
			ret += 360.0;
		}
	}
	else
	{
		ret = Lerp(_start, _end, _term);
	}

	return ret;
}

// 色（COLOR_F）を線形補間する
COLOR_F UtilityMath::Lerp(const COLOR_F& _start, const COLOR_F& _end, float _term)
{
	//線形補間
	if (_term >= 1.0f)
	{
		return _end;
	}
	return COLOR_F(_start.r + _term * (_end.r - _start.r)
		, _start.g + _term * (_end.g - _start.g)
		, _start.b + _term * (_end.b - _start.b)
		, _start.a + _term * (_end.a - _start.a));
}

// 2Dベクトルの3点間でベジエ補間する
Vector2 UtilityMath::Bezier(const Vector2& _point1, const Vector2& _point2, const Vector2& _point3, float _term)
{
	Vector2 a = Lerp(_point1, _point2, _term);
	Vector2 b = Lerp(_point2, _point3, _term);

	return Lerp(a, b, _term);
}

// 3Dベクトルの3点間でベジエ補間する
VECTOR UtilityMath::Bezier(const VECTOR& _point1, const VECTOR& _point2, const VECTOR& _point3, float _term)
{
	VECTOR a = Lerp(_point1, _point2, _term);
	VECTOR b = Lerp(_point2, _point3, _term);

	return Lerp(a, b, _term);
}

// XZ平面上で中心点を軸に回転後の座標を求める
VECTOR UtilityMath::RotXZPos(const VECTOR& _centerPos, const VECTOR& _radiusPos, float _rad)
{
	float x = ((_radiusPos.x - _centerPos.x) * cosf(_rad)) - ((_radiusPos.z - _centerPos.z) * sinf(_rad));
	float z = ((_radiusPos.x - _centerPos.x) * cosf(_rad)) + ((_radiusPos.z - _centerPos.z) * cosf(_rad));

	return VGet(_centerPos.x + x, _radiusPos.y, _centerPos.z + z);
}

// 2Dベクトルの長さを求める
double UtilityMath::Magnitude(const Vector2& _vec)
{
	return sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y));
}

// 3Dベクトルの長さを求める
double UtilityMath::Magnitude(const VECTOR& _vec)
{
	return sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y) + (_vec.z * _vec.z));
}

// 3Dベクトルの長さをfloatで求める
float UtilityMath::MagnitudeF(const VECTOR& _vec)
{
	return sqrtf((_vec.x * _vec.x) + (_vec.y * _vec.y) + (_vec.z * _vec.z));
}

// 2Dベクトルの長さの2乗を求める
int UtilityMath::SqrMagnitude(const Vector2& _vec)
{
	return ((_vec.x * _vec.x) + (_vec.y * _vec.y));
}

// 3Dベクトルの長さの2乗をfloatで求める
float UtilityMath::SqrMagnitudeF(const VECTOR& _vec)
{
	return ((_vec.x * _vec.x) + (_vec.y * _vec.y) + (_vec.z * _vec.z));
}

// 3Dベクトルの長さの2乗を求める
double UtilityMath::SqrMagnitude(const VECTOR& _vec)
{
	return ((_vec.x * _vec.x) + (_vec.y * _vec.y) + (_vec.z * _vec.z));
}

// 2つの3Dベクトル間の距離の2乗を求める

double UtilityMath::SqrMagnitude(const VECTOR& _vec1, const VECTOR& _vec2)
{
	return pow(_vec2.x - _vec1.x, 2) + pow(_vec2.y - _vec1.y, 2) + pow(_vec2.z - _vec1.z, 2);
}

// 2つの2Dベクトル間の距離を求める
double UtilityMath::Distance(const Vector2& _vec1, const Vector2& _vec2)
{
	return sqrt(pow(_vec2.x - _vec1.x, 2) + pow(_vec2.y - _vec1.y, 2));
}

// 2つの3Dベクトル間の距離を求める
double UtilityMath::Distance(const VECTOR& _vec1, const VECTOR& _vec2)
{
	return sqrt(pow(_vec2.x - _vec1.x, 2) + pow(_vec2.y - _vec1.y, 2) + pow(_vec2.z - _vec1.z, 2));
}

// 2つの3Dベクトルが等しいかどうかを判定する
bool UtilityMath::Equals(const VECTOR& _vec1, const VECTOR& _vec2)
{
	if ((_vec1.x == _vec2.x) && (_vec1.y == _vec2.y) && (_vec1.z == _vec2.z))
	{
		return true;
	}

	return false;
}

/* ベクトルがゼロベクトルか判定 */
bool UtilityMath::EqualsVZero(const VECTOR& _vec)
{
	const VECTOR& ZERO = VECTOR_ZERO;
	if (_vec.x == ZERO.x && _vec.y == ZERO.y && _vec.z == ZERO.z)
	{
		return true;
	}

	return false;
}
bool UtilityMath::EqualsVZero(const Vector2& _vec)
{
	const Vector2& ZERO = VECTOR2_ZERO;
	if (_vec.x == ZERO.x && _vec.y == ZERO.y)
	{
		return true;
	}

	return false;
}
bool UtilityMath::EqualsVZero(const Vector2F& _vec)
{
	const Vector2F& ZERO = VECTOR2F_ZERO;
	if (_vec.x == ZERO.x && _vec.y == ZERO.y)
	{
		return true;
	}
	return false;
}

// 2Dベクトルを正規化して3Dベクトルで返す
VECTOR UtilityMath::Normalize(const Vector2& _vec)
{
	VECTOR ret = VGet(
		static_cast<float>(_vec.x)
		, static_cast<float>(_vec.y)
		, 0.0f
	);

	float len = static_cast<float>(Magnitude(_vec));

	ret.x /= len;
	ret.y /= len;
	ret.z /= len;

	return ret;
}

/* ベクトルを正規化 */
VECTOR UtilityMath::VNormalize(const VECTOR& _vec)
{
	// ゼロ除算対策で元のゼロベクトルを返す
	if (UtilityMath::EqualsVZero(_vec)) { return _vec; }

	return VNorm(_vec);
}
Vector2 UtilityMath::VNormalize(const Vector2& _vec)
{
	// ゼロ除算対策で元のゼロベクトルを返す
	if (UtilityMath::EqualsVZero(_vec)) { return _vec; }

	float mag = sqrtf(static_cast<float>((_vec.x * _vec.x) + (_vec.y * _vec.y)));

	return Vector2(static_cast<int>(_vec.x / mag), static_cast<int>(_vec.y / mag));
}
Vector2F UtilityMath::VNormalize(const Vector2F& _vec)
{
	// ゼロ除算対策で元のゼロベクトルを返す
	if (UtilityMath::EqualsVZero(_vec)) { return _vec; }

	float mag = sqrtf((_vec.x * _vec.x) + (_vec.y * _vec.y));

	return Vector2F(_vec.x / mag, _vec.y / mag);
}

// 2つの3Dベクトルのなす角（度）を求める
double UtilityMath::AngleDeg(const VECTOR& _from, const VECTOR& _to)
{
	auto fLen = SqrMagnitude(_from);
	auto tLen = SqrMagnitude(_to);
	auto denomina_tor = sqrt(fLen * tLen);
	if (denomina_tor < kEpsilonNormalSqrt)
	{
		return 0.0f;
	}

	auto dot = VDot(_from, _to) / denomina_tor;

	if (dot < -1.0f)
	{
		dot = -1.0f;
	}
	if (dot > 1.0f)
	{
		dot = 1.0f;
	}

	return acos(dot) * (180.0 / DX_PI);

}

// 指定方向に線と終点に球体を描画する
void UtilityMath::DrawLineDir(const VECTOR& _pos, const VECTOR& _dir, int color, float len)
{
	auto nDir = UtilityMath::VNormalize(_dir);
	auto sPos = VAdd(_pos, VScale(nDir, -len));
	auto ePos = VAdd(_pos, VScale(nDir, len));

	DrawLine3D(sPos, ePos, color);
	DrawSphere3D(ePos, 5.0f, 5, color, color, true);
}

// 回転行列を基にXYZ軸方向の線を描画する
void UtilityMath::DrawLineXYZ(const VECTOR& _pos, const MATRIX& rot, float len)
{
	VECTOR dir;

	// X
	dir = VTransform(UtilityMath::DIR_R, rot);
	DrawLineDir(_pos, dir, 0xff0000, len);

	// Y
	dir = VTransform(UtilityMath::DIR_UP, rot);
	DrawLineDir(_pos, dir, 0x00ff00, len);

	// Z
	dir = VTransform(UtilityMath::DIR_FORWARD, rot);
	DrawLineDir(_pos, dir, 0x0000ff, len);
}

// クォータニオンを基にXYZ軸方向の線を描画する
void UtilityMath::DrawLineXYZ(const VECTOR& _pos, const Quaternion& rot, float len)
{
	VECTOR dir;

	// X
	dir = rot.GetRight();
	DrawLineDir(_pos, dir, 0xff0000, len);

	// Y
	dir = rot.GetUp();
	DrawLineDir(_pos, dir, 0x00ff00, len);

	// Z
	dir = rot.GetForward();
	DrawLineDir(_pos, dir, 0x0000ff, len);

	// W
	dir = VAdd(VNormalize(rot.GetRight()), VNormalize(rot.GetUp()));
	dir = VAdd(dir, VNormalize(rot.GetForward()));
	dir = VNormalize(dir);
	DrawLineDir(_pos, dir, 0xffffff, len);
}

VECTOR UtilityMath::GetCirclePos(const VECTOR& _center, float _radius, float angle)
{
	return VGet(_center.x + std::cosf(angle) * _radius,
		_center.y + std::sinf(angle) * _radius, _center.z);
}

float UtilityMath::RandRangeF(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

VECTOR UtilityMath::GetNearestPointOnSegment(const VECTOR& _startPos,
	const VECTOR& _endPos, const VECTOR& _targetPos)
{
	VECTOR segmentVec = VSub(_endPos, _startPos);
	VECTOR _toTargetVec = VSub(_targetPos, _startPos);

	float lenSquare = static_cast<float>(SqrMagnitude(segmentVec));

	if (lenSquare < 1e-6)
	{
		return _startPos;
	}

	float segmentRatio = VDot(_toTargetVec, segmentVec) / lenSquare;

	if (segmentRatio < 0.0f) { segmentRatio = 0.0f; }
	if (segmentRatio > 1.0f) { segmentRatio = 1.0f; }

	VECTOR nearestPos = VAdd(_startPos, VScale(segmentVec, segmentRatio));

	return nearestPos;
}