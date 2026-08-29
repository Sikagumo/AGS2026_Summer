#include "ShaderEditorComponent.h"
#include "../../Lib/ImGUI/imgui.h"

ShaderEditorComponent::ShaderEditorComponent(const std::string& _name, 
	NormalWaveShaderParams* _params)
	: name_(_name)
	, params_(_params)
{
}

void ShaderEditorComponent::DrawUI(void)
{
	// パラメータが存在しない場合は安全のために処理を抜ける
	if (params_ == nullptr)
	{
		return;
	}

	// 登録された名前でウィンドウを作成
	ImGui::Begin(name_.c_str());

	// ライティング設定
	if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const float SLIDER_MIN_DIRECTION = -1.0f; // ライト方向スライダーの最小値
		const float SLIDER_MAX_DIRECTION = 1.0f;  // ライト方向スライダーの最大値
		const float SLIDER_MIN_RATE = 0.0f;       // 割合スライダーの最小値
		const float SLIDER_MAX_RATE = 1.0f;       // 割合スライダーの最大値

		float lightDirection[3] = {               // ライトの方向ベクトル
			params_->lightVectorX,
			params_->lightVectorY,
			params_->lightVectorZ
		};

		if (ImGui::SliderFloat3("Light Direction", lightDirection, SLIDER_MIN_DIRECTION, 
			SLIDER_MAX_DIRECTION))
		{
			// 変更があれば構造体に直接セットする
			params_->lightVectorX = lightDirection[0];
			params_->lightVectorY = lightDirection[1];
			params_->lightVectorZ = lightDirection[2];
		}

		// 環境光
		if (ImGui::SliderFloat("Ambient", &params_->ambientRate, SLIDER_MIN_RATE, SLIDER_MAX_RATE))
		{
			// 変更はそのまま反映される
		}
	}

	// 波エフェクト設定
	if (ImGui::CollapsingHeader("Wave Effect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const float SLIDER_MIN_WAVE = 0.0f;  // 波パラメータスライダーの最小値
		const float SLIDER_MAX_SPEED = 5.0f; // 波速度スライダーの最大値
		const float SLIDER_MAX_FORCE = 0.1f; // 波強度スライダーの最大値

		// 波の速さ
		if (ImGui::SliderFloat("Wave Speed", &params_->waveSpeedValue, SLIDER_MIN_WAVE, 
			SLIDER_MAX_SPEED))
		{
		}

		// 波の強さ
		if (ImGui::SliderFloat("Wave Force", &params_->waveForceValue, SLIDER_MIN_WAVE,
			SLIDER_MAX_FORCE))
		{
		}

		ImGui::Text("Current Time: %.3f", params_->timeValue);
	}

	// フラグ設定
	if (ImGui::CollapsingHeader("Flags"))
	{
		const float THRESHOLD_FLAG = 0.5f; // フラグ判定のしきい値
		const float FLAG_ON = 1.0f;        // フラグONの値
		const float FLAG_OFF = 0.0f;       // フラグOFFの値

		// ノーマルマップのON/OFF
		bool isUseNormalMap = (params_->useNormalMapFlag > THRESHOLD_FLAG);
		if (ImGui::Checkbox("Use NormalMap", &isUseNormalMap))
		{
			params_->useNormalMapFlag = isUseNormalMap ? FLAG_ON : FLAG_OFF;
		}
	}

	ImGui::End();
}