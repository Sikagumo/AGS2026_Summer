#include "UtilityMath.h"

#include<DxLib.h>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<math.h>
#include <cstdlib>



// 蟆乗焚蛟､繧貞屁謐ｨ莠泌・縺励※謨ｴ謨ｰ縺ｫ螟画鋤縺吶ｋ
int UtilityMath::Round(float v)
{
	return static_cast<int>(roundf(v));
}

// 譁・ｭ怜・繧呈欠螳壹＆繧後◆蛹ｺ蛻・ｊ譁・ｭ励〒蛻・牡縺吶ｋ
std::vector<std::string> UtilityMath::Split(std::string& line, char delimiter)
{
	std::istringstream stream(line);
	std::string field;
	std::vector<std::string> result;

	while (getline(stream, field, delimiter))
	{
		result.push_back(field);
	}

	return result;
}

// 繝ｩ繧ｸ繧｢繝ｳ繧貞ｺｦ縺ｫ螟画鋤縺吶ｋ・・ouble・・
double UtilityMath::Rad2DegD(double rad)
{
	return rad * (180.0 / DX_PI);
}

// 繝ｩ繧ｸ繧｢繝ｳ繧貞ｺｦ縺ｫ螟画鋤縺吶ｋ・・loat・・
float UtilityMath::Rad2DegF(float rad)
{
	return rad * (180.0f / DX_PI_F);
}

// 繝ｩ繧ｸ繧｢繝ｳ繧貞ｺｦ縺ｫ螟画鋤縺励※謨ｴ謨ｰ縺ｫ縺吶ｋ
int UtilityMath::Rad2DegI(int rad)
{
	return rad * Round(180.0f / DX_PI_F);
}

// 蠎ｦ繧偵Λ繧ｸ繧｢繝ｳ縺ｫ螟画鋤縺吶ｋ・・ouble・・
double UtilityMath::Deg2RadD(double deg)
{
	return deg * (DX_PI / 180.0);
}

// 蠎ｦ繧偵Λ繧ｸ繧｢繝ｳ縺ｫ螟画鋤縺吶ｋ・・loat・・
float UtilityMath::Deg2RadF(float deg)
{
	return deg * (DX_PI_F / 180.0f);
}

// 蠎ｦ繧偵Λ繧ｸ繧｢繝ｳ縺ｫ螟画鋤縺励※謨ｴ謨ｰ縺ｫ縺吶ｋ
int UtilityMath::Deg2RadI(int deg)
{
	return deg * Round(DX_PI_F / 180.0f);
}

// 隗貞ｺｦ繧・縲・60蠎ｦ縺ｮ遽・峇縺ｫ豁｣隕丞喧縺吶ｋ
double UtilityMath::DegIn360(double deg)
{
	deg = fmod(deg, 360.0);
	if (deg < 0.0f)
	{
		deg += 360.0;
	}

	return deg;
}

// 繝ｩ繧ｸ繧｢繝ｳ繧・縲・ﾏ縺ｮ遽・峇縺ｫ豁｣隕丞喧縺吶ｋ
double UtilityMath::RadIn2PI(double rad)
{
	rad = fmod(rad, DX_TWO_PI);
	if (rad < 0.0)
	{
		rad += DX_TWO_PI;
	}

	return rad;
}

// 繝ｩ繧ｸ繧｢繝ｳ蜊倅ｽ阪〒縺ｩ縺｡繧牙屓繧翫′霑代＞縺九ｒ蛻､螳壹☆繧具ｼ・1=蜿肴凾險亥屓繧翫・=譎りｨ亥屓繧奇ｼ・
int UtilityMath::DirNearAroundRad(float from, float to)
{
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{

		//豈碑ｼ・・繧医ｊ繧よ凾險亥屓繧翫↓菴咲ｽｮ縺吶ｋ

		if (diff > DX_PI_F)
		{

			//縺ｧ繧ゅ・80蠎ｦ莉･荳企屬繧後※縺・ｋ縺ｮ縺ｧ縲∝渚譎りｨ亥屓繧翫・譁ｹ縺瑚ｿ代＞
			ret = -1.0f;
		}
		else
		{
			//譎りｨ亥屓繧・
			ret = 1.0f;
		}

	}
	else
	{

		//豈碑ｼ・・繧医ｊ繧ょ渚譎りｨ亥屓繧翫↓菴咲ｽｮ縺吶ｋ

		if (diff < -DX_PI_F)
		{

			//縺ｧ繧ゅ・80蠎ｦ莉･荳企屬繧後※縺・ｋ縺ｮ縺ｧ縲∵凾險亥屓繧翫・譁ｹ縺瑚ｿ代＞
			ret = 1.0f;
		}
		else
		{

			//蜿肴凾險亥屓繧・
			ret = -1.0f;
		}
	}

	return static_cast<int>(ret);
}

// 蠎ｦ蜊倅ｽ阪〒縺ｩ縺｡繧牙屓繧翫′霑代＞縺九ｒ蛻､螳壹☆繧具ｼ・1=蜿肴凾險亥屓繧翫・=譎りｨ亥屓繧奇ｼ・
int UtilityMath::DirNearAroundDeg(float from, float to)
{
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{


		//豈碑ｼ・・繧医ｊ繧よ凾險亥屓繧翫↓菴咲ｽｮ縺吶ｋ

		if (diff > 180.0f)
		{

			//縺ｧ繧ゅ・80蠎ｦ莉･荳企屬繧後※縺・ｋ縺ｮ縺ｧ縲∝渚譎りｨ亥屓繧翫・譁ｹ縺瑚ｿ代＞
			ret = -1.0f;
		}
		else
		{
			//譎りｨ亥屓繧・
			ret = 1.0f;
		}
	}
	else
	{

		//豈碑ｼ・・繧医ｊ繧ょ渚譎りｨ亥屓繧翫↓菴咲ｽｮ縺吶ｋ

		if (diff < -180.0f)
		{
			//縺ｧ繧ゅ・80蠎ｦ莉･荳企屬繧後※縺・ｋ縺ｮ縺ｧ縲よ凾險亥屓繧翫・譁ｹ縺瑚ｿ代＞
			ret = 1.0f;
		}
		else
		{
			//譎りｨ亥屓繧・
			ret = -1.0f;
		}
	}

	return static_cast<int>(ret);
}

// 謨ｴ謨ｰ蛟､繧堤ｷ壼ｽ｢陬憺俣縺吶ｋ
int UtilityMath::Lerp(int start, int end, float t)
{
	//邱壼ｽ｢陬憺俣
	if (t >= 1.0f)
	{
		return end;
	}

	int ret = start;

	ret += Round(t * static_cast<float>(end - start));

	return ret;
}

// 豬ｮ蜍募ｰ乗焚轤ｹ蛟､繧堤ｷ壼ｽ｢陬憺俣縺吶ｋ
float UtilityMath::Lerp(float start, float end, float t)
{
	//邱壼ｽ｢陬憺俣
	if (t >= 1.0f)
	{
		return end;
	}

	float ret = start;

	ret += t * (end - start);

	return ret;
}

// 蛟咲ｲｾ蠎ｦ豬ｮ蜍募ｰ乗焚轤ｹ蛟､繧堤ｷ壼ｽ｢陬憺俣縺吶ｋ
double UtilityMath::Lerp(double start, double end, double t)
{
	//邱壼ｽ｢陬憺俣
	if (t >= 1.0)
	{
		return end;
	}

	double ret = start;

	ret += t * (end - start);

	return ret;
}

// 2D繝吶け繝医Ν繧堤ｷ壼ｽ｢陬憺俣縺吶ｋ
Vector2 UtilityMath::Lerp(const Vector2& start, const Vector2& end, float t)
{
	//邱壼ｽ｢陬憺俣
	if (t >= 1.0f)
	{
		return end;
	}

	Vector2 ret = start;

	ret.x += Round(t * static_cast<float>((end.x - start.x)));
	ret.y += Round(t * static_cast<float>((end.y - start.y)));

	return end;
}

// 3D繝吶け繝医Ν繧堤ｷ壼ｽ｢陬憺俣縺吶ｋ
VECTOR UtilityMath::Lerp(const VECTOR& start, const VECTOR& end, float t)
{
	//邱壼ｽ｢陬憺俣
	if (t >= 1.0f)
	{
		return end;
	}

	VECTOR ret = start;

	ret.x += t * (end.x - start.x);
	ret.y += t * (end.y - start.y);
	ret.z += t * (end.z - start.z);

	return ret;
}

// 蠎ｦ蜊倅ｽ阪〒隗貞ｺｦ繧堤ｷ壼ｽ｢陬憺俣縺励・60蠎ｦ繧定・・縺吶ｋ
double UtilityMath::LerpDeg(double start, double end, double t)
{
	double ret;

	double diff = end - start;

	if (diff < -180.0)
	{
		end += 360.0;

		ret = Lerp(start, end, t);

		if (ret >= 360.0)
		{
			ret -= 360.0;
		}
	}
	else if (diff > 180.0)
	{
		end -= 360.0;

		ret = Lerp(start, end, t);

		if (ret < 0.0)
		{
			ret += 360.0;
		}
	}
	else
	{
		ret = Lerp(start, end, t);
	}

	return ret;
}

// 濶ｲ・・OLOR_F・峨ｒ邱壼ｽ｢陬憺俣縺吶ｋ
COLOR_F UtilityMath::Lerp(const COLOR_F& start, const COLOR_F& end, float t)
{
	//邱壼ｽ｢陬憺俣
	if (t >= 1.0f)
	{
		return end;
	}

	COLOR_F ret = start;

	ret.r += t * (end.r - start.r);
	ret.g += t * (end.g - start.g);
	ret.b += t * (end.b - start.b);
	ret.a += t * (end.a - start.a);

	return ret;
}

// 2D繝吶け繝医Ν縺ｮ3轤ｹ髢薙〒繝吶ず繧ｨ陬憺俣縺吶ｋ
Vector2 UtilityMath::Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
{
	Vector2 a = Lerp(p1, p2, t);
	Vector2 b = Lerp(p2, p3, t);

	return Lerp(a, b, t);
}

// 3D繝吶け繝医Ν縺ｮ3轤ｹ髢薙〒繝吶ず繧ｨ陬憺俣縺吶ｋ
VECTOR UtilityMath::Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t)
{
	VECTOR a = Lerp(p1, p2, t);
	VECTOR b = Lerp(p2, p3, t);

	return Lerp(a, b, t);
}

// XZ蟷ｳ髱｢荳翫〒荳ｭ蠢・せ繧定ｻｸ縺ｫ蝗櫁ｻ｢蠕後・蠎ｧ讓吶ｒ豎ゅａ繧・
VECTOR UtilityMath::RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad)
{
	float x = ((radiusPos.x - centerPos.x) * cosf(rad)) - ((radiusPos.z - centerPos.z) * sinf(rad));
	float z = ((radiusPos.x - centerPos.x) * cosf(rad)) + ((radiusPos.z - centerPos.z) * cosf(rad));

	return VGet(centerPos.x + x, radiusPos.y, centerPos.z + z);
}

// 2D繝吶け繝医Ν縺ｮ髟ｷ縺輔ｒ豎ゅａ繧・
double UtilityMath::Magnitude(const Vector2& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y));
}

// 3D繝吶け繝医Ν縺ｮ髟ｷ縺輔ｒ豎ゅａ繧・
double UtilityMath::Magnitude(const VECTOR& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// 3D繝吶け繝医Ν縺ｮ髟ｷ縺輔ｒfloat縺ｧ豎ゅａ繧・
float UtilityMath::MagnitudeF(const VECTOR& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// 2D繝吶け繝医Ν縺ｮ髟ｷ縺輔・2荵励ｒ豎ゅａ繧・
int UtilityMath::SqrMagnitude(const Vector2& v)
{
	return v.x * v.x + v.y * v.y;
}

// 3D繝吶け繝医Ν縺ｮ髟ｷ縺輔・2荵励ｒfloat縺ｧ豎ゅａ繧・
float UtilityMath::SqrMagnitudeF(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 3D繝吶け繝医Ν縺ｮ髟ｷ縺輔・2荵励ｒ豎ゅａ繧・
double UtilityMath::SqrMagnitude(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 2縺､縺ｮ3D繝吶け繝医Ν髢薙・霍晞屬縺ｮ2荵励ｒ豎ゅａ繧・
double UtilityMath::SqrMagnitude(const VECTOR& v1, const VECTOR& v2)
{
	return pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2);
}

// 2縺､縺ｮ2D繝吶け繝医Ν髢薙・霍晞屬繧呈ｱゅａ繧・
double UtilityMath::Distance(const Vector2& v1, const Vector2& v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

// 2縺､縺ｮ3D繝吶け繝医Ν髢薙・霍晞屬繧呈ｱゅａ繧・
double UtilityMath::Distance(const VECTOR& v1, const VECTOR& v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

// 2縺､縺ｮ3D繝吶け繝医Ν縺檎ｭ峨＠縺・°縺ｩ縺・°繧貞愛螳壹☆繧・
bool UtilityMath::Equals(const VECTOR& v1, const VECTOR& v2)
{
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
	{
		return true;
	}

	return false;
}

// 繝吶け繝医Ν縺後ぞ繝ｭ繝吶け繝医Ν縺九←縺・°繧貞愛螳壹☆繧・
bool UtilityMath::EqualsVZero(const VECTOR& v1)
{
	const VECTOR& v2 = VECTOR_ZERO;
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
	{
		return true;
	}

	return false;
}

// 2D繝吶け繝医Ν繧呈ｭ｣隕丞喧縺励※3D繝吶け繝医Ν縺ｧ霑斐☆
VECTOR UtilityMath::Normalize(const Vector2& v)
{
	VECTOR ret = VGet(
		static_cast<float>(v.x),
		static_cast<float>(v.y),
		0.0f
	);

	float len = static_cast<float>(Magnitude(v));

	ret.x /= len;
	ret.y /= len;
	ret.z /= len;

	return ret;
}

// 3D繝吶け繝医Ν繧呈ｭ｣隕丞喧縺吶ｋ・医ぞ繝ｭ繝吶け繝医Ν縺ｮ縺ｨ縺阪・縺昴・縺ｾ縺ｾ霑斐☆・・
VECTOR UtilityMath::VNormalize(const VECTOR& v)
{
	if (UtilityMath::EqualsVZero(v))
	{
		//Quaternion險育ｮ励〒繧ｼ繝ｭ繧呈ｸ｡縺励※縲・
		//繧ｨ繝ｩ繝ｼ(-1, -1, -1)縺瑚ｿ斐▲縺ｦ縺上ｋ縺ｨ蝗ｰ繧・
		return v;
	}
	return VNorm(v);
}

// 2縺､縺ｮ3D繝吶け繝医Ν縺ｮ縺ｪ縺呵ｧ抵ｼ亥ｺｦ・峨ｒ豎ゅａ繧・
double UtilityMath::AngleDeg(const VECTOR& from, const VECTOR& to)
{
	auto fLen = SqrMagnitude(from);
	auto tLen = SqrMagnitude(to);
	auto denominator = sqrt(fLen * tLen);
	if (denominator < kEpsilonNormalSqrt)
	{
		return 0.0f;
	}

	auto dot = VDot(from, to) / denominator;

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

// 謖・ｮ壽婿蜷代↓邱壹→邨らせ縺ｫ逅・ｽ薙ｒ謠冗判縺吶ｋ
void UtilityMath::DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len)
{
	auto nDir = UtilityMath::VNormalize(dir);
	auto sPos = VAdd(pos, VScale(nDir, -len));
	auto ePos = VAdd(pos, VScale(nDir, len));

	DrawLine3D(sPos, ePos, color);
	DrawSphere3D(ePos, 5.0f, 5, color, color, true);
}

// 蝗櫁ｻ｢陦悟・繧貞渕縺ｫXYZ霆ｸ譁ｹ蜷代・邱壹ｒ謠冗判縺吶ｋ
void UtilityMath::DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len)
{
	VECTOR dir;

	// X
	dir = VTransform(UtilityMath::DIR_R, rot);
	DrawLineDir(pos, dir, 0xff0000, len);

	// Y
	dir = VTransform(UtilityMath::DIR_UP, rot);
	DrawLineDir(pos, dir, 0x00ff00, len);

	// Z
	dir = VTransform(UtilityMath::DIR_F, rot);
	DrawLineDir(pos, dir, 0x0000ff, len);
}

// 繧ｯ繧ｩ繝ｼ繧ｿ繝九が繝ｳ繧貞渕縺ｫXYZ霆ｸ譁ｹ蜷代・邱壹ｒ謠冗判縺吶ｋ
void UtilityMath::DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len)
{
	VECTOR dir;

	// X
	dir = rot.GetRight();
	DrawLineDir(pos, dir, 0xff0000, len);

	// Y
	dir = rot.GetUp();
	DrawLineDir(pos, dir, 0x00ff00, len);

	// Z
	dir = rot.GetForward();
	DrawLineDir(pos, dir, 0x0000ff, len);
}

//bool UtilityMath::IsTimeOver(float& totalTime, const float& waitTime)
//{
//	//繝・Ν繧ｿ繧ｿ繧､繝
//	auto delta = SceneManager::GetInstance().GetDeltaTime();
//	totalTime += delta;
//
//	//蠕・ｩ滓凾髢薙ｒ雜・℃縺励※縺・ｋ縺句愛譁ｭ
//	if (totalTime >= waitTime)
//	{
//		return true;
//	}
//
//	return false;
//}

// 繝ｩ繝ｳ繝繝縺ｪ謨ｰ蛟､繧定ｿ斐☆(float逕ｨ)
float UtilityMath::RandRangeF(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}