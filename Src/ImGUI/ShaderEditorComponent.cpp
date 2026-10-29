#include "ShaderEditorComponent.h"
#include "../../Lib/ImGUI/imgui.h"

ShaderEditorComponent::ShaderEditorComponent(
	const std::string& _name, NormalWaveShaderParams* _parameters)
	: name_(_name)
	, parameters_(_parameters)
{
}

void ShaderEditorComponent::DrawUI(void)
{
	// パラメータが存在しない場合は安全のために処理を抜ける
	if (parameters_ == nullptr)
	{
		return;
	}

	// 登録された名前でウィンドウを作成
	ImGui::Begin(name_.c_str());

	// ライティング設定
	if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
	{
		constexpr float SLIDER_MIN_DIRECTION = -1.0f; // ライト方向スライダーの最小値
		constexpr float SLIDER_MAX_DIRECTION = 1.0f;  // ライト方向スライダーの最大値
		constexpr float SLIDER_MIN_RATE = 0.0f;       // 割合スライダーの最小値
		constexpr float SLIDER_MAX_RATE = 1.0f;       // 割合スライダーの最大値

		constexpr int VECTOR_ELEMENT_COUNT = 3; // ベクトルの要素数
		constexpr int INDEX_X = 0;              // X成分のインデックス
		constexpr int INDEX_Y = 1;              // Y成分のインデックス
		constexpr int INDEX_Z = 2;              // Z成分のインデックス

		float lightDirection[VECTOR_ELEMENT_COUNT] = { // ライトの方向ベクトル
			parameters_->lightVectorX,
			parameters_->lightVectorY,
			parameters_->lightVectorZ
		};

		if (ImGui::SliderFloat3(
			"Light Direction", lightDirection, SLIDER_MIN_DIRECTION, SLIDER_MAX_DIRECTION))
		{
			// 変更があれば構造体に直接セットする
			parameters_->lightVectorX = lightDirection[INDEX_X];
			parameters_->lightVectorY = lightDirection[INDEX_Y];
			parameters_->lightVectorZ = lightDirection[INDEX_Z];
		}

		// 環境光（ポインタ渡しにより直接値が更新される）
		ImGui::SliderFloat(
			"Ambient", &parameters_->ambientRate, SLIDER_MIN_RATE, SLIDER_MAX_RATE);
	}

	// 波エフェクト設定
	if (ImGui::CollapsingHeader("Wave Effect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		constexpr float SLIDER_MIN_WAVE = 0.0f;  // 波パラメータスライダーの最小値
		constexpr float SLIDER_MAX_SPEED = 5.0f; // 波速度スライダーの最大値
		constexpr float SLIDER_MAX_FORCE = 0.1f; // 波強度スライダーの最大値

		// 波の速さ
		ImGui::SliderFloat(
			"Wave Speed", &parameters_->waveSpeedValue, SLIDER_MIN_WAVE, SLIDER_MAX_SPEED);

		// 波の強さ
		ImGui::SliderFloat(
			"Wave Force", &parameters_->waveForceValue, SLIDER_MIN_WAVE, SLIDER_MAX_FORCE);

		ImGui::Text("Current Time: %.3f", parameters_->timeValue);
	}

	// フラグ設定
	if (ImGui::CollapsingHeader("Flags"))
	{
		constexpr float THRESHOLD_FLAG = 0.5f; // フラグ判定のしきい値
		constexpr float FLAG_ON = 1.0f;        // フラグONの値
		constexpr float FLAG_OFF = 0.0f;       // フラグOFFの値

		// ノーマルマップのON/OFF
		bool isUseNormalMap = (parameters_->useNormalMapFlag > THRESHOLD_FLAG);
		if (ImGui::Checkbox("Use NormalMap", &isUseNormalMap))
		{
			parameters_->useNormalMapFlag = isUseNormalMap ? FLAG_ON : FLAG_OFF;
		}
	}

	ImGui::End();
}