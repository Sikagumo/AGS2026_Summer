#include "ShaderMaterial.h"

ShaderMaterial::ShaderMaterial(void)
	: lightX_(0.5f), lightY_(0.5f), lightZ_(0.5f)
	, ambient_(0.8f), time_ (0.0f)
	, waveSpeed_(3.0f) , waveForce_(0.015f), useNormal_(1.0f)
	, resolutionX_(1920), resolutionY_(1080)
	, rainIntensity_(0.0f), rainIntensityBack_(0.0f), rainColor_()
	, texScaleX_(1.0f), texScaleY_(1.0f)
{
}

ShaderMaterial::~ShaderMaterial(void)
{
}

void ShaderMaterial::SetLightDirection(float _lightX, float _lightY, float _lightZ)
{
	lightX_ = _lightX;
	lightY_ = _lightY;
	lightZ_ = _lightZ;
}

void ShaderMaterial::SetAmbient(float _ambient)
{
	ambient_ = _ambient;
}

void ShaderMaterial::SetWaveSpeed(float _speed) 
{
	waveSpeed_ = _speed;
}

void ShaderMaterial::SetWaveForce(float _force) 
{
	waveForce_ = _force;
}

void ShaderMaterial::SetTime(float _time)
{
	time_ = _time;
}

void ShaderMaterial::SetUseNormalMap(bool _use)
{
	useNormal_ = _use ? 1.0f : 0.0f;
}

bool ShaderMaterial::IsUseNormalMap(void) const
{
	return useNormal_ > 0.5f;
}

void ShaderMaterial::SetUseRainy(float _rainPow, float _rainPowBack)
{
	const COLOR_F RAIN_COLOR = { 0.75f, 0.82f, 0.9f };
	rainColor_ = RAIN_COLOR;
	time_ = 0.0f;

	rainIntensity_ = _rainPow;
	rainIntensityBack_ = _rainPowBack;
}

void ShaderMaterial::SetTexScale(float _texScaleX, float _texScaleY)
{
	texScaleX_ = _texScaleX;
	texScaleY_ = _texScaleY;
}
