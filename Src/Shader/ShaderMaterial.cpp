#include "ShaderMaterial.h"

ShaderMaterial::ShaderMaterial(void)
{
	lightX_ = 0.5f;
	lightY_ = 0.5f;
	lightZ_ = 0.5f;
	ambient_ = 0.8f;
	time_ = 0.0f;
	waveSpeed_ = 3.0f;
	waveForce_ = 0.015f;
	useNormal_ = 1.0f;
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
