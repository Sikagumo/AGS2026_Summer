#include "ShaderEditorComponent.h"
#include "../../Lib/ImGUI/imgui.h"

ShaderEditorComponent::ShaderEditorComponent(const std::string& _name,
	IntegratedGpuBuffer* _targetBuffer)
	: name_(_name)
	, targetBuffer_(_targetBuffer)
{

}

void ShaderEditorComponent::DrawUI(void)
{
	// 編集対象のデータが存在しない場合は処理を抜ける
	if (targetBuffer_ == nullptr)
	{
		return;
	}

	// 登録された名前でウィンドウを作成
	ImGui::Begin(name_.c_str());

	if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Light Direction");
		ImGui::SliderFloat("Light X", &targetBuffer_->lightX, -1.0f, 1.0f);
		ImGui::SliderFloat("Light Y", &targetBuffer_->lightY, -1.0f, 1.0f);
		ImGui::SliderFloat("Light Z", &targetBuffer_->lightZ, -1.0f, 1.0f);

		ImGui::Text("Ambient");
		ImGui::SliderFloat("Ambient", &targetBuffer_->ambient, 0.0f, 1.0f);
	}

	if (ImGui::CollapsingHeader("Wave Effect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Wave Settings");
		ImGui::SliderFloat("Wave Speed", &targetBuffer_->waveSpeed, 0.0f, 5.0f);
		ImGui::SliderFloat("Wave Force", &targetBuffer_->waveForce, 0.0f, 0.1f);

		ImGui::Text("Time");
		ImGui::InputFloat("Time", &targetBuffer_->time);
	}

	if (ImGui::CollapsingHeader("Flags"))
	{
		bool useNormal = (targetBuffer_->useNormal > 0.5f);

		if (ImGui::Checkbox("Use NormalMap", &useNormal))
		{
			targetBuffer_->useNormal = useNormal ? 1.0f : 0.0f;
		}
	}

	ImGui::End();
}