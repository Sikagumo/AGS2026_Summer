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
		float lightDir[3] = {
			params_->lightVectorX,
			params_->lightVectorY,
			params_->lightVectorZ
		};

		if (ImGui::SliderFloat3("Light Direction", lightDir, -1.0f, 1.0f))
		{
			// 変更があれば構造体に直接セットする
			params_->lightVectorX = lightDir[0];
			params_->lightVectorY = lightDir[1];
			params_->lightVectorZ = lightDir[2];
		}

		// 環境光
		if (ImGui::SliderFloat("Ambient", &params_->ambientRate, 0.0f, 1.0f))
		{
			// 変更はそのまま反映される
		}
	}

	// 波エフェクト設定
	if (ImGui::CollapsingHeader("Wave Effect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 波の速さ
		if (ImGui::SliderFloat("Wave Speed", &params_->waveSpeedValue, 0.0f, 5.0f))
		{
		}

		// 波の強さ
		if (ImGui::SliderFloat("Wave Force", &params_->waveForceValue, 0.0f, 0.1f))
		{
		}

		ImGui::Text("Current Time: %.3f", params_->timeValue);
	}

	// フラグ設定
	if (ImGui::CollapsingHeader("Flags"))
	{
		// ノーマルマップのON/OFF（useNormalMapFlag は float なので bool に変換して扱う）
		bool useNormal = (params_->useNormalMapFlag > 0.5f);
		if (ImGui::Checkbox("Use NormalMap", &useNormal))
		{
			params_->useNormalMapFlag = useNormal ? 1.0f : 0.0f;
		}
	}

	ImGui::End();
}