#include "ShaderEditorComponent.h"
#include "../../Lib/ImGUI/imgui.h"

ShaderEditorComponent::ShaderEditorComponent(const std::string& _name, 
	ShaderMaterial* _material)
	: name_(_name)
	, material_(_material)
{
}

void ShaderEditorComponent::DrawUI(void)
{
	// マテリアルが存在しない場合は安全のために処理を抜ける
	if (material_ == nullptr)
	{
		return;
	}

	// 登録された名前でウィンドウを作成
	ImGui::Begin(name_.c_str());

	// ライティング設定
	if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float lightDir[3] = {
			material_->GetLightDirX(),
			material_->GetLightDirY(),
			material_->GetLightDirZ()
		};

		if (ImGui::SliderFloat3("Light Direction", lightDir, -1.0f, 1.0f))
		{
			// 変更があればマテリアルにセットする
			material_->SetLightDirection(lightDir[0], lightDir[1], lightDir[2]);
		}

		// 環境光
		float ambient = material_->GetAmbient();
		if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f))
		{
			material_->SetAmbient(ambient);
		}
	}

	// 波エフェクト設定
	if (ImGui::CollapsingHeader("Wave Effect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 波の速さ
		float speed = material_->GetWaveSpeed();
		if (ImGui::SliderFloat("Wave Speed", &speed, 0.0f, 5.0f))
		{
			material_->SetWaveSpeed(speed);
		}

		// 波の強さ
		float force = material_->GetWaveForce();
		if (ImGui::SliderFloat("Wave Force", &force, 0.0f, 0.1f))
		{
			material_->SetWaveForce(force);
		}

		float currentTime = material_->GetTime();
		ImGui::Text("Current Time: %.3f", currentTime);
	}

	// フラグ設定
	if (ImGui::CollapsingHeader("Flags"))
	{
		// ノーマルマップのON/OFF
		bool useNormal = material_->IsUseNormalMap();
		if (ImGui::Checkbox("Use NormalMap", &useNormal))
		{
			material_->SetUseNormalMap(useNormal);
		}
	}

	ImGui::End();
}