#include "aer/editor/tool/light_controller.h"

#include "utils/imgui_utility.h"

namespace neko::aer
{
LightController::LightController(AerEngine& engine) : EditorToolInterface(engine) {}

void LightController::DrawImGui()
{
	using namespace ImGui;
	DirectionalLight* dirLight = DirectionalLight::Instance;
	if (!dirLight)
	{
		ImGui::TextColored(Color::red, "No DirectionalLight in scene!");
		return;
	}

	ColorEdit3("Ambient", &dirLight->ambient[0], LabelPos::LEFT);
	ColorEdit3("Diffuse", &dirLight->diffuse[0], LabelPos::LEFT);
	DragFloat3("Direction", &dirLight->direction[0], LabelPos::LEFT, 0.05f, -1.0f, 1.0f);

	DragFloat("Specular", &dirLight->specular, LabelPos::LEFT, 0.05f, 0.0f, 1.0f);
	DragFloat("Intensity", &dirLight->intensity, LabelPos::LEFT, 0.05f, 0.0f, 1000.0f);
}
}    // namespace neko::aer
