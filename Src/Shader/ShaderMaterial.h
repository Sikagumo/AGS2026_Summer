#pragma once
#include <DxLib.h>

/// @brief シェーダのデータ管理クラス
class ShaderMaterial
{
public:

	/// @brief コンストラクタ
	ShaderMaterial(void);

	/// @brief デストラクタ 
	~ShaderMaterial(void);

	/// @brief 光の方向を設定
	/// @param _lightX X座標からの光方向
	/// @param _lightY Y座標からの光方向
	/// @param _lightZ Z座標からの光方向
	void SetLightDirection(float _lightX, float _lightY, float _lightZ);

	/// @brief X座標の光方向を取得 
	/// @return X座標の光方向
	float GetLightDirX(void) const { return lightX_; }

	/// @brief Y座標の光方向を取得 
	/// @return Y座標の光方向
	float GetLightDirY(void) const { return lightY_; }

	/// @brief Z座標の光方向を取得 
	/// @return Z座標の光方向
	float GetLightDirZ(void) const { return lightZ_; }

	/// @brief 環境光の設定
	/// @param _ambient 環境光
	void SetAmbient(float _ambient);

	/// @brief 環境光の取得 
	/// @return 環境光
	float GetAmbient(void) const { return ambient_; }

	/// @brief 波の速さの設定
	/// @param _speed 波の速さ
	void SetWaveSpeed(float _speed);

	/// @brief 波の速さの取得 
	/// @return 波の速さ
	float GetWaveSpeed(void) const { return waveSpeed_; }

	/// @brief 波の強さを設定
	/// @param _force 波の強さ
	void SetWaveForce(float _force);

	/// @brief 波の強さを取得
	/// @return 波の強さ
	float GetWaveForce(void) const { return waveForce_; }

	/// @brief シェーダの用時間を設定
	/// @param _time 時間
	void SetTime(float _time);

	/// @brief 時間を取得 
	/// @return 時間
	float GetTime(void) const { return time_; }

	/// @brief ノーマルマップを使用するかどうか
	/// @param isUse 使用するかどうか
	void SetUseNormalMap(bool isUse);

	/// @brief ノーマルマップフラグを取得 
	/// @return ノーマルマップフラグ
	bool IsUseNormalMap(void) const;


	/// @brief 雨シェーダを割り当て処理
	/// @param _rainPow 雨の強さ(0.0～1.0)
	/// @param _rainPowBack 後ろの雨の強さ(0.0～1.0)
	/// @param _rainPowBack 後ろの雨の強さ(0.0～1.0)
	void SetUseRainy(float _rainPow, float _rainPowBack);

	/// @brief 雨シェーダを利用しているか否か
	bool IsUseRainy(void)const { return (rainIntensity_ > 0.0f || rainIntensityBack_ > 0.0f); };

	/// @brief 解像度
	float GetResolutionX(void)const { return resolutionX_; };
	float GetResolutionY(void)const { return resolutionY_; };

	/// @brief 雨の強さを取得
	float GetRainIntensity(void)const { return rainIntensity_; };
	float GetRainIntensityBack(void)const { return rainIntensityBack_; };

	COLOR_F GetRainColor(void)const { return rainColor_; };


	void SetTexScale(float _texScaleX, float _texScaleY);
	float GetTexScaleX(void)const { return texScaleX_; }
	float GetTexScaleY(void)const { return texScaleY_; }


private:

	float lightX_;
	float lightY_;
	float lightZ_;
	float ambient_;
	float time_;
	float waveSpeed_;
	float waveForce_;
	float useNormal_;

	// 画面解像度
	float resolutionX_;
	float resolutionY_;

	// 雨の強さ 0.0～1.0
	float rainIntensity_;
	float rainIntensityBack_;
	COLOR_F rainColor_;

	float texScaleX_;
	float texScaleY_;
};

