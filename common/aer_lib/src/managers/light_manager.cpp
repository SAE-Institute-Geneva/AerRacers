#include "aer/managers/light_manager.h"

#include "utils/imgui_utility.h"

namespace neko::aer
{
LightManager::LightManager(EntityManager& entityManager, Transform3dManager& transformManager)
   : ComponentManager<PointLight, EntityMask(ComponentType::LIGHT)>(entityManager),
	 transformManager_(transformManager)
{}

#ifdef NEKO_GLES3
void LightManager::SetShaderValues(gl::Shader& shader)
{
	std::uint64_t curOffset = 0;
	const auto lights = entityManager_.get().FilterEntities(EntityMask(ComponentType::LIGHT));
	const std::size_t lightNum = lights.size();

	shader.SetUbo(sizeof(unsigned), curOffset, &lightNum, gl::kUboLightsBinding);
	curOffset += sizeof(Vec4f); // OpenGL Uniform Blocks are spaced by multiple of 16 bytes
	for (std::size_t i = 0; i < lightNum; ++i)
	{
		const Entity e = lights[i];
		const Vec3f position = transformManager_.GetGlobalPosition(e);
		shader.SetUbo(sizeof(Vec3f), curOffset, &position, gl::kUboLightsBinding);
		curOffset += sizeof(Vec4f);

		shader.SetUbo(sizeof(Vec3f), curOffset, &components_[e].diffuse, gl::kUboLightsBinding);
		curOffset += sizeof(Vec4f);

		shader.SetUbo(sizeof(Vec3f), curOffset, &components_[e].ambient, gl::kUboLightsBinding);
		curOffset += sizeof(Vec3f);

		shader.SetUbo(sizeof(float), curOffset, &components_[e].intensity, gl::kUboLightsBinding);
		curOffset += sizeof(float);

		shader.SetUbo(sizeof(float), curOffset, &components_[e].radius, gl::kUboLightsBinding);
		curOffset += sizeof(float);

		shader.SetUbo(sizeof(float), curOffset, &components_[e].specular, gl::kUboLightsBinding);
		curOffset += sizeof(float);

		const std::uint8_t mod = curOffset % 16;
		curOffset = curOffset + (mod == 0 ? 0 : 16 - mod);
	}
}
#endif

LightViewer::LightViewer(EntityManager& entityManager, LightManager& lightManager)
   : ComponentViewer(entityManager), lightManager_(lightManager)
{}

void LightViewer::DrawImGui(Entity entity)
{
	if (entity == INVALID_ENTITY) return;
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::LIGHT)))
	{
		using namespace ImGui;
		if (TreeNode("Light"))
		{
			PointLight light = lightManager_.GetComponent(entity);
			DragFloat3("Diffuse", &light.diffuse[0], LabelPos::LEFT, 0.05f, 0.0f, 1.0f);
			DragFloat3("Ambient", &light.ambient[0], LabelPos::LEFT, 0.05f, 0.0f, 1.0f);

			DragFloat("Intensity", &light.intensity, LabelPos::LEFT, 0.05f, 0.0f, 1000.0f);
			DragFloat("Radius", &light.radius, LabelPos::LEFT, 0.05f, 0.0f, 1000.0f);
			DragFloat("Specular", &light.specular, LabelPos::LEFT, 0.05f, 0.0f, 1.0f);

			lightManager_.SetComponent(entity, light);
			TreePop();
		}
	}
}

json LightViewer::GetJsonFromComponent(Entity entity) const
{
	json component = json::object();
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::LIGHT)))
	{
		if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
		{
			component["diffuse"] = GetJsonFromVector3(lightManager_.GetDiffuse(entity));
			component["ambient"] = GetJsonFromVector3(lightManager_.GetAmbient(entity));

			component["intensity"] = lightManager_.GetIntensity(entity);
			component["radius"]    = lightManager_.GetRadius(entity);
			component["specular"]  = lightManager_.GetSpecular(entity);
		}
	}

	return component;
}

void LightViewer::SetComponentFromJson(Entity entity, const json& jsonComponent)
{
	PointLight light;
	light.diffuse = GetVector3FromJson(jsonComponent, "diffuse");
	light.ambient = GetVector3FromJson(jsonComponent, "ambient");

	light.intensity = jsonComponent["intensity"];
	light.radius    = jsonComponent["radius"];
	light.specular  = jsonComponent["specular"];

	lightManager_.SetComponent(entity, light);
}
}    // namespace neko::aer
