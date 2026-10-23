#define NOMINMAX
#include "Quaternion.h"

#include<math.h>

#include"../Utility/UtilityMath.h"

namespace
{
	// 数値計算用定数関連
	constexpr double VALUE_ZERO_D = 0.0;		// 浮動小数点数（double型）のゼロ
	constexpr double VALUE_ONE_D = 1.0;			// 浮動小数点数（double型）の1.0
	constexpr double VALUE_HALF_D = 0.5;        // 浮動小数点数（double型）の0.5（半分）
	constexpr double VALUE_TWO_D = 2.0;			// 浮動小数点数（double型）の2.0
	constexpr float  VALUE_ZERO_F = 0.0f;       // 浮動小数点数（float型）のゼロ
	constexpr float  VALUE_ONE_F = 1.0f;		// 浮動小数点数（float型）の1.0
	constexpr float  VALUE_HALF_F = 0.5f;		// 浮動小数点数（float型）の0.5（半分）
	constexpr float  VALUE_TWO_F = 2.0f;		// 浮動小数点数（float型）の2.0
	constexpr float  VALUE_QUARTER_F = 0.25f;	// 浮動小数点数（float型）の0.25（4分の1）

	// 角度および比較用定数
	constexpr double ANGLE_LIMIT_DEGREE = 179.9196;   // 180度近傍の判定を行うための限界角度（度数法）
	constexpr float  LENGTH_EPSILON = 0.000001f;      // ベクトルの長さなどの極小値（誤差・ゼロ除算防止用）
	constexpr float  DENOMINATOR_EPSILON = 0.0001f;   // 分母の極小値（ゼロ除算防止用）
	constexpr double DEGREE_CONVERT_FACTOR = 180.0;   // ラジアンから度数法（デグリー）へ変換するための係数

	// 行列のインデックス定数関連
	constexpr int MATRIX_INDEX_0 = 0; // 行列の0番目（X成分関連）
	constexpr int MATRIX_INDEX_1 = 1; // 行列の1番目（Y成分関連）
	constexpr int MATRIX_INDEX_2 = 2; // 行列の2番目（Z成分関連）
	constexpr int MATRIX_INDEX_3 = 3; // 行列の3番目（W成分・平行移動関連）
}

Quaternion::Quaternion(void)
	: w_(VALUE_ONE_D)
	, x_(VALUE_ZERO_D)
	, y_(VALUE_ZERO_D)
	, z_(VALUE_ZERO_D)
{
}

Quaternion::Quaternion(const VECTOR& _radian)
{
	Quaternion quaternion = CreateEuler(_radian.x, _radian.y, _radian.z);
	w_ = quaternion.w_;
	x_ = quaternion.x_;
	y_ = quaternion.y_;
	z_ = quaternion.z_;
}

Quaternion::Quaternion(double _scalarW, double _vectorX, double _vectorY, double _vectorZ)
{
	w_ = _scalarW;
	x_  = _vectorX;
	y_  = _vectorY;
	z_  = _vectorZ;
}

Quaternion Quaternion::CreateEuler(const VECTOR& _radian)
{
	return CreateEuler(_radian.x, _radian.y, _radian.z);
}

Quaternion Quaternion::CreateEuler(double _radianX, double _radianY, double _radianZ)
{
	Quaternion result = Quaternion();

	// 角度を0〜2π範囲に正規化
	_radianX = UtilityMath::RadIn2PI(_radianX);
	_radianY = UtilityMath::RadIn2PI(_radianY);
	_radianZ = UtilityMath::RadIn2PI(_radianZ);

	// オイラー角からクォータニオンへの変換式
	double cosineZ = cos(_radianZ / VALUE_TWO_D);
	double sineZ = sin(_radianZ / VALUE_TWO_D);
	double cosineX = cos(_radianX / VALUE_TWO_D);
	double sineX = sin(_radianX / VALUE_TWO_D);
	double cosineY = cos(_radianY / VALUE_TWO_D);
	double sineY = sin(_radianY / VALUE_TWO_D);

	// ZYXの順で適用するクォータニオン計算
	result.w_ = cosineX * cosineY * cosineZ + sineX * sineY * sineZ;
	result.x_ = sineX * cosineY * cosineZ + cosineX * sineY * sineZ;
	result.y_ = cosineX * sineY * cosineZ - sineX * cosineY * sineZ;
	result.z_ = cosineX * cosineY * sineZ - sineX * sineY * cosineZ;

	return result;
}

Quaternion Quaternion::Multiply(const Quaternion& _quaternion1, const Quaternion& _quaternion2)
{
	Quaternion result = Quaternion();
	double value1, value2, value3, value4;

	// wの計算 
	value1 = _quaternion1.w_ * _quaternion2.w_;
	value2 = -_quaternion1.x_ * _quaternion2.x_;
	value3 = -_quaternion1.y_ * _quaternion2.y_;
	value4 = -_quaternion1.z_ * _quaternion2.z_;
	result.w_ = value1 + value2 + value3 + value4;

	// xの計算 
	value1 = _quaternion1.w_ * _quaternion2.x_;
	value2 = _quaternion2.w_ * _quaternion1.x_;
	value3 = _quaternion1.y_ * _quaternion2.z_;
	value4 = -_quaternion1.z_ * _quaternion2.y_;
	result.x_ = value1 + value2 + value3 + value4;

	// yの計算
	value1 = _quaternion1.w_ * _quaternion2.y_;
	value2 = _quaternion2.w_ * _quaternion1.y_;
	value3 = _quaternion1.z_ * _quaternion2.x_;
	value4 = -_quaternion1.x_ * _quaternion2.z_;
	result.y_ = value1 + value2 + value3 + value4;

	// zの計算
	value1 = _quaternion1.w_ * _quaternion2.z_;
	value2 = _quaternion2.w_ * _quaternion1.z_;
	value3 = _quaternion1.x_ * _quaternion2.y_;
	value4 = -_quaternion1.y_ * _quaternion2.x_;
	result.z_ = value1 + value2 + value3 + value4;

	return result;
}

Quaternion Quaternion::Multiply(const Quaternion& quaternion) const
{
	return Multiply(Quaternion(w_, x_, y_, z_), quaternion);
}

Quaternion Quaternion::CreateAngleAxis(double radian, VECTOR axis)
{
	Quaternion result = Quaternion();

	double squaredNorm;
	double cosineValue, sineValue;

	// 単位クォータニオンで初期化
	result.w_ = VALUE_ONE_D;
	result.x_ = result.y_ = result.z_ = VALUE_ZERO_D;
	
	// 軸ベクトルの正規化
	squaredNorm = (double)axis.x * (double)axis.x +
		(double)axis.y * (double)axis.y +
		(double)axis.z * (double)axis.z;

	if (squaredNorm <= VALUE_ZERO_D)
	{
		return result;
	}

	squaredNorm = VALUE_ONE_D / sqrt(squaredNorm);
	axis.x = static_cast<float>(axis.x * squaredNorm);
	axis.y = static_cast<float>(axis.y * squaredNorm);
	axis.z = static_cast<float>(axis.z * squaredNorm);

	// 角度の半分のcos, sinを計算
	cosineValue = cos(VALUE_HALF_D * radian);
	sineValue = sin(VALUE_HALF_D * radian);

	// クォータニオン生成
	result.w_ = cosineValue;
	result.x_ = sineValue * axis.x;
	result.y_ = sineValue * axis.y;
	result.z_ = sineValue * axis.z;

	return result;
}

VECTOR Quaternion::RotatePosition(const Quaternion& quaternion, VECTOR position)
{
	// 位置情報に回転情報を反映させる
	// p' = q * p * q^-1 (pを純四元数として扱う)
	Quaternion temporary = Quaternion();
	temporary = temporary.Multiply(quaternion);
	temporary = temporary.Multiply(Quaternion(VALUE_ZERO_F, position.x, position.y, position.z));
	temporary = temporary.Multiply(quaternion.CalculateInverse());

	return { static_cast<float>(temporary.x_), static_cast<float>(temporary.y_),
			 static_cast<float>(temporary.z_) };
}

VECTOR Quaternion::RotatePosition(VECTOR position) const
{
	return RotatePosition(Quaternion(w_, x_, y_, z_), position);
}

VECTOR Quaternion::ConvertToEuler(const Quaternion& quaternion)
{
	VECTOR result;

	// クォータニオンから回転行列の要素を計算
	double r11 = VALUE_TWO_D * (quaternion.x_ * quaternion.z_ + quaternion.w_ * quaternion.y_);
	double r12 = quaternion.w_ * quaternion.w_ - quaternion.x_ * quaternion.x_ -
		quaternion.y_ * quaternion.y_ + quaternion.z_ * quaternion.z_;
	double r21 = -VALUE_TWO_D * (quaternion.y_ * quaternion.z_ - quaternion.w_ * quaternion.x_);
	double r31 = VALUE_TWO_D * (quaternion.x_ * quaternion.y_ + quaternion.w_ * quaternion.z_);
	double r32 = quaternion.w_ * quaternion.w_ - quaternion.x_ * quaternion.x_ +
		quaternion.y_ * quaternion.y_ - quaternion.z_ * quaternion.z_;

	// オイラー角（X, Y, Z）を計算
	result.x = static_cast<float>(asin(r21));          // X軸回転（ピッチ）
	result.y = static_cast<float>(atan2(r11, r12));    // Y軸回転（ヨー）
	result.z = static_cast<float>(atan2(r31, r32));    // Z軸回転（ロール）

	return result;
}

VECTOR Quaternion::ConvertToEuler(void) const
{
	return ConvertToEuler(Quaternion(w_, x_, y_, z_));
}

MATRIX Quaternion::ConvertToMatrix(const Quaternion& _quaternion)
{
	MATRIX matrix;

	FLOAT4 floatQuaternion = {
		static_cast<float>(_quaternion.x_),
		static_cast<float>(_quaternion.y_),
		static_cast<float>(_quaternion.z_),
		static_cast<float>(_quaternion.w_)
	};

	// クォータニオンの成分から回転行列の要素を計算
	float scaleX = floatQuaternion.x * floatQuaternion.x * VALUE_TWO_F;
	float scaleY = floatQuaternion.y * floatQuaternion.y * VALUE_TWO_F;
	float scaleZ = floatQuaternion.z * floatQuaternion.z * VALUE_TWO_F;
	float crossX = floatQuaternion.y * floatQuaternion.z * VALUE_TWO_F;
	float crossY = floatQuaternion.x * floatQuaternion.z * VALUE_TWO_F;
	float crossZ = floatQuaternion.x * floatQuaternion.y * VALUE_TWO_F;
	float weightX = floatQuaternion.w * floatQuaternion.x * VALUE_TWO_F;
	float weightY = floatQuaternion.w * floatQuaternion.y * VALUE_TWO_F;
	float weightZ = floatQuaternion.w * floatQuaternion.z * VALUE_TWO_F;

	// 4x4回転行列を生成
	matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0] = VALUE_ONE_F - (scaleY + scaleZ);
	matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1] = crossZ + weightZ;
	matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2] = crossY - weightY;
	matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_3] = VALUE_ZERO_F;

	matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0] = crossZ - weightZ;
	matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] = VALUE_ONE_F - (scaleX + scaleZ);
	matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2] = crossX + weightX;
	matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_3] = VALUE_ZERO_F;

	matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0] = crossY + weightY;
	matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1] = crossX - weightX;
	matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] = VALUE_ONE_F - (scaleX + scaleY);
	matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_3] = VALUE_ZERO_F;

	matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_0] = VALUE_ZERO_F;
	matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_1] = VALUE_ZERO_F;
	matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_2] = VALUE_ZERO_F;
	matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_3] = VALUE_ONE_F;

	return matrix;
}

MATRIX Quaternion::ConvertToMatrix(void) const
{
	return ConvertToMatrix(Quaternion(w_, x_, y_, z_));
}

Quaternion Quaternion::CreateLookRotation(VECTOR direction)
{
	VECTOR upDirection = VGet(VALUE_ZERO_F, VALUE_ONE_F, VALUE_ZERO_F);
	return CreateLookRotation(direction, upDirection);
}

Quaternion Quaternion::CreateLookRotation(VECTOR direction, VECTOR upDirection)
{
	// 方向ベクトルを正規化
	direction = UtilityMath::VNormalize(direction);
	
	// 右方向ベクトルを計算し正規化（アップベクトルと方向ベクトルの外積）
	VECTOR rightDirection = UtilityMath::VNormalize(VCross(upDirection, direction));
	
	// 正確なアップベクトルを再計算（直交性を保証）
	upDirection = VCross(direction, rightDirection);

	// 回転行列の要素を計算
	auto matrix00 = rightDirection.x;
	auto matrix01 = rightDirection.y;
	auto matrix02 = rightDirection.z;
	auto matrix10 = upDirection.x;
	auto matrix11 = upDirection.y;
	auto matrix12 = upDirection.z;
	auto matrix20 = direction.x;
	auto matrix21 = direction.y;
	auto matrix22 = direction.z;

	// 回転行列からクォータニオンへの変換
	float traceValue = (matrix00 + matrix11) + matrix22;
	auto resultQuaternion = Quaternion();

	if (traceValue > VALUE_ZERO_F)
	{
		// 対角和が正の場合
		double scaleValue = sqrt(traceValue + VALUE_ONE_D);
		resultQuaternion.w_ = scaleValue * VALUE_HALF_D;
		scaleValue = VALUE_HALF_D / scaleValue;
		resultQuaternion.x_ = (static_cast<double>(matrix12) - matrix21) * scaleValue;
		resultQuaternion.y_ = (static_cast<double>(matrix20) - matrix02) * scaleValue;
		resultQuaternion.z_ = (static_cast<double>(matrix01) - matrix10) * scaleValue;
		
		return resultQuaternion.GetNormalized();
	}

	if ((matrix00 >= matrix11) && (matrix00 >= matrix22))
	{
		// m00が最大の場合
		auto scaleX = sqrt(((VALUE_ONE_F + matrix00) - matrix11) - matrix22);
		auto scaleFactor = VALUE_HALF_F / scaleX;
		resultQuaternion.x_ = VALUE_HALF_D * scaleX;
		resultQuaternion.y_ = (static_cast<double>(matrix01) + matrix10) * scaleFactor;
		resultQuaternion.z_ = (static_cast<double>(matrix02) + matrix20) * scaleFactor;
		resultQuaternion.w_ = (static_cast<double>(matrix12) - matrix21) * scaleFactor;
		
		return resultQuaternion.GetNormalized();
	}

	if (matrix11 > matrix22)
	{
		// m11が最大の場合
		auto scaleY = sqrt(((VALUE_ONE_F + matrix11) - matrix00) - matrix22);
		auto scaleFactor = VALUE_HALF_F / scaleY;
		resultQuaternion.x_ = (static_cast<double>(matrix10) + matrix01) * scaleFactor;
		resultQuaternion.y_ = VALUE_HALF_D * scaleY;
		resultQuaternion.z_ = (static_cast<double>(matrix21) + matrix12) * scaleFactor;
		resultQuaternion.w_ = (static_cast<double>(matrix20) - matrix02) * scaleFactor;
		
		return resultQuaternion.GetNormalized();
	}

	// m22が最大の場合
	auto scaleZ = sqrt(((VALUE_ONE_F + matrix22) - matrix00) - matrix11);
	auto scaleFactor = VALUE_HALF_F / scaleZ;
	resultQuaternion.x_ = (static_cast<double>(matrix20) + matrix02) * scaleFactor;
	resultQuaternion.y_ = (static_cast<double>(matrix21) + matrix12) * scaleFactor;
	resultQuaternion.z_ = VALUE_HALF_D * scaleZ;
	resultQuaternion.w_ = (static_cast<double>(matrix01) - matrix10) * scaleFactor;

	return resultQuaternion.GetNormalized();
}

Quaternion Quaternion::GetRotation(MATRIX _matrix)
{
	Quaternion result;

	float scaleValue;
	// 行列の対角和+1を計算
	float trace = _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0] +
		_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] +
		_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] + VALUE_ONE_F;

	if (trace >= VALUE_ONE_F)
	{
		// 対角和が大きい場合
		scaleValue = VALUE_HALF_F / sqrtf(trace);
		result.w_ = VALUE_QUARTER_F / scaleValue;
		result.x_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2] - _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
		result.y_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0] - _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
		result.z_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1] - _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
	}
	else
	{
		// 対角和が小さい場合は最大対角要素を基準に計算
		float maxValue;
		maxValue = _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] > _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2]
			? _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1]
			: _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2];

		if (maxValue < _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0])
		{
			// m00が最大の場合
			scaleValue = sqrtf(_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0] -
				(_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] + _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2]) + VALUE_ONE_F);

			float calculateX = scaleValue * VALUE_HALF_F;
			scaleValue = VALUE_HALF_F / scaleValue;
			result.x_ = calculateX;
			result.y_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1] + _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
			result.z_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0] + _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
			result.w_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2] - _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
		}
		else if (maxValue == _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1])
		{
			// m11が最大の場合
			scaleValue = sqrtf(_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] -
				(_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] + _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0]) + VALUE_ONE_F);

			float calculateY = scaleValue * VALUE_HALF_F;
			scaleValue = VALUE_HALF_F / scaleValue;
			result.x_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1] + _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
			result.y_ = calculateY;
			result.z_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2] + _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
			result.w_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0] - _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
		}
		else
		{
			// m22が最大の場合
			scaleValue = sqrtf(_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] -
				(_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0] + _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1]) + VALUE_ONE_F);

			float calculateZ = scaleValue * VALUE_HALF_F;
			scaleValue = VALUE_HALF_F / scaleValue;
			result.x_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0] + _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
			result.y_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2] + _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
			result.z_ = calculateZ;
			result.w_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1] - _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
		}
	}

	return result;
}

VECTOR Quaternion::GetDirection(VECTOR direction) const
{
	return RotatePosition(direction);
}

VECTOR Quaternion::GetForward(void) const
{
	return GetDirection(UtilityMath::DIR_FORWARD);
}

VECTOR Quaternion::GetBack(void) const
{
	return GetDirection(UtilityMath::DIR_BACK);
}

VECTOR Quaternion::GetRight(void) const
{
	return GetDirection(UtilityMath::DIR_R);
}

VECTOR Quaternion::GetLeft(void) const
{
	return GetDirection(UtilityMath::DIR_L);
}

VECTOR Quaternion::GetUp(void) const
{
	return GetDirection(UtilityMath::DIR_UP);
}

VECTOR Quaternion::GetDown(void) const
{
	return GetDirection(UtilityMath::DIR_DOWN);
}

double Quaternion::CalculateDot(const Quaternion& quaternion1, const Quaternion& quaternion2)
{
	return (quaternion1.w_ * quaternion2.w_ + quaternion1.x_ * quaternion2.x_ +
		quaternion1.y_ * quaternion2.y_ + quaternion1.z_ * quaternion2.z_);
}

double Quaternion::CalculateDot(const Quaternion& target) const
{
	return (w_ * target.w_ + x_ * target.x_ + y_ * target.y_ + z_ * target.z_);
}

Quaternion Quaternion::Normalize(const Quaternion& quaternion)
{
	float scaleFactor = VALUE_ONE_F / static_cast<float>(quaternion.CalculateLength());
	VECTOR scaledVector = VScale(quaternion.GetXYZ(), scaleFactor);
	Quaternion result = Quaternion(quaternion.w_ * scaleFactor, scaledVector.x, 
		scaledVector.y, scaledVector.z);

	return result;
}

Quaternion Quaternion::GetNormalized(void) const
{
	double magnitude = sqrt(w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);

	return Quaternion(w_ / magnitude, x_ / magnitude, y_ / magnitude, z_ / magnitude);
}

void Quaternion::Normalize(void)
{
	double magnitude = sqrt(w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);

	w /= magnitude;
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
}

Quaternion Quaternion::Inverse(void) const
{
	double n = 1.0f / (w * w + x * x + y * y + z * z);
	Quaternion tmp = Quaternion(w, -x, -y, -z);  // 共役クォータニオン
	return Quaternion(tmp.w * n, tmp.x * n, tmp.y * n, tmp.z * n);  // 共役/長さの二乗
}

Quaternion Quaternion::Slerp(Quaternion from, Quaternion to, double t)
{
	if (t > 1) t = 1;
	if (t < 0) t = 0;
	return SlerpUnclamped(from, to, (float)t);
}

inline float SIGN(float x)
{
	return (x >= 0.0f) ? +1.0f : -1.0f;
}

inline float NORM(float a, float b, float c, float d)
{
	return sqrt(a * a + b * b + c * c + d * d);
}

Quaternion Quaternion::FromToRotation(VECTOR fromDir, VECTOR toDir)
{
	// 回転軸を計算（fromDirとtoDirの外積）
	VECTOR axis = VCross(fromDir, toDir);
	double angle = UtilityMath::AngleDeg(fromDir, toDir);

	// 180度近くの回転の場合、別の方法で軸を決定
	if (angle >= 179.9196)
	{
		auto r = VCross(fromDir, UtilityMath::DIR_R);
		axis = VCross(r, fromDir);
		float len = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
		if (len < 0.000001f)
		{
			axis = UtilityMath::DIR_UP; // 軸が小さすぎる場合はY軸を使用
		}
	}

	// 軸を正規化し、angle-axis表現からクォータニオンを生成
	axis = UtilityMath::VNormalize(axis);
	return Quaternion::AngleAxis(UtilityMath::Deg2RadD(angle), axis);
}

Quaternion Quaternion::RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta)
{
	double num = Quaternion::Angle(from, to);
	if (num == 0.0)
	{
		return to; // 角度が0の場合はtoをそのまま返す
	}

	// 補間比率tを計算（最大角度で制限）
	float t = std::min(1.0f, maxDegreesDelta / (float)num);
	return Quaternion::SlerpUnclamped(from, to, t);
}

double Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
{
	double cos = Quaternion::Dot(q1, q2);
	double ac = acos(cos);
	return ac * (180.0 / DX_PI); // ラジアンから度に変換
}

Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t)
{
	// いずれかの入力が0の場合はもう一方を返す
	if (a.LengthSquared() == 0.0f)
	{
		if (b.LengthSquared() == 0.0f)
		{
			return Identity();
		}
		return b;
	}
	else if (b.LengthSquared() == 0.0f)
	{
		return a;
	}

	// 内積から回転の角度を判断
	float cosHalfAngle = (float)(a.w * b.w) + VDot(a.xyz(), b.xyz());

	if (cosHalfAngle >= 1.0f || cosHalfAngle <= -1.0f)
	{
		// 角度が0の場合は入力のひとつを返す
		return a;
	}
	else if (cosHalfAngle < 0.0f)
	{
		// 内積が負の場合、-bを使用して最短経路で補間
		b.x = b.x * -1.0f;
		b.y = b.y * -1.0f;
		b.z = b.z * -1.0f;
		b.w = -b.w;
		cosHalfAngle = -cosHalfAngle;
	}

	float blendA;
	float blendB;
	if (cosHalfAngle < 0.99f)
	{
		// 大きな角度の場合は真のSlerp
		float halfAngle = acosf(cosHalfAngle);
		float sinHalfAngle = sinf(halfAngle);
		float oneOverSinHalfAngle = 1.0f / sinHalfAngle;
		blendA = sinf(halfAngle * (1.0f - t)) * oneOverSinHalfAngle;
		blendB = sinf(halfAngle * t) * oneOverSinHalfAngle;
	}
	else
	{
		// 角度が小さい場合は線形補間（Lerp）
		blendA = 1.0f - t;
		blendB = t;
	}

	// 重み付け合成
	VECTOR v = VAdd(VScale(a.xyz(), blendA), VScale(b.xyz(), blendB));
	Quaternion result = Quaternion(blendA * a.w + blendB * b.w, v.x, v.y, v.z);

	// 正規化して返す
	if (result.LengthSquared() > 0.0f)
	{
		return Normalize(result);
	}
	else
	{
		return Identity();
	}
}

Quaternion Quaternion::Identity(void)
{
	return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

double Quaternion::Length(void) const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

double Quaternion::LengthSquared(void) const
{
	return x * x + y * y + z * z + w * w;
}

VECTOR Quaternion::xyz(void) const
{
	return { (float)x, (float)y, (float)z };
}

void Quaternion::ToAngleAxis(float* angle, VECTOR* axis)
{
	// クォータニオンの正規化
	if (abs(this->w) > 1.0f)
	{
		this->Normalize();
	}

	// 回転角を計算
	*angle = 2.0f * acosf((float)this->w); // angle

	// 回転角が極めて小さい場合は0とする
	if (x == 0 && y == 0 && z == 0)
	{
		*angle = 0.0f;
	}

	// 回転軸を計算
	float den = sqrtf(1.0f - (float)(this->w * this->w));
	if (den > 0.0001f)
	{
		// 軸を正規化
		auto v = this->xyz();
		axis->x = v.x / den;
		axis->y = v.y / den;
		axis->z = v.z / den;
	}
	else
	{
		// 角度が0の場合は任意の軸を設定
		// （この場合、回転がないため軸は意味を持たない）
		*axis = { 1.0f, 0.0f, 0.0f };
	}
}

Quaternion Quaternion::operator*(float& f)
{
	return Quaternion(w * f, x * f, y * f, z * f);
}

const Quaternion Quaternion::operator*(const float& f)
{
	return Quaternion(w * f, x * f, y * f, z * f);
}

Quaternion Quaternion::operator+(Quaternion& rhs)
{
	return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}

const Quaternion Quaternion::operator+(const Quaternion& rhs)
{
	return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}