#include "aer/managers/light_manager.h"

#include "utils/imgui_utility.h"

#ifdef NEKO_VULKAN
#include "vk/vk_resources.h"
#endif

namespace neko::aer
{
LightManager::LightManager(EntityManager& entityManager, Transform3dManager& transformManager)
   : ComponentManager<PointLight, EntityMask(ComponentType::LIGHT)>(entityManager),
	 transformManager_(transformManager)
{}

#ifdef NEKO_GLES3
void LightManager::SetShaderValues(gl::Shader& shader)
{
	// OpenGL Uniform Blocks are spaced by multiple of 16 bytes
	std::uint64_t curOffset = 0;
	const auto lights = entityManager_.get().FilterEntities(EntityMask(ComponentType::LIGHT));
	ResizeIfNecessary(components_, lights.back(), {});

	const DirectionalLight* dirLight = DirectionalLight::Instance;
	const std::size_t lightNum = lights.size() + (dirLight ? 1 : 0);
	shader.SetUbo(sizeof(unsigned), curOffset, &lightNum, gl::kUboLightsBinding);
	curOffset += sizeof(Vec4f);

	if (dirLight)
	{
		shader.SetUbo(sizeof(Vec3f), curOffset, &dirLight->diffuse, gl::kUboLightsBinding);
		curOffset += sizeof(Vec4f);

		shader.SetUbo(sizeof(Vec3f), curOffset, &dirLight->ambient, gl::kUboLightsBinding);
		curOffset += sizeof(Vec4f);

		shader.SetUbo(sizeof(Vec3f), curOffset, &dirLight->direction, gl::kUboLightsBinding);
		curOffset += sizeof(Vec3f);

		shader.SetUbo(sizeof(float), curOffset, &dirLight->specular, gl::kUboLightsBinding);
		curOffset += sizeof(float);

		shader.SetUbo(sizeof(float), curOffset, &dirLight->intensity, gl::kUboLightsBinding);
		curOffset += sizeof(float);
	}
	else
	{
		curOffset += sizeof(Vec4f) * 2 + sizeof(Vec3f) + sizeof(float) * 2;
	}

	std::uint8_t mod = curOffset % 16;
	curOffset = curOffset + (mod == 0 ? 0 : 16 - mod);
	for (auto&& e : lights)
	{
		const Vec3f position = transformManager_.GetGlobalPosition(e);
		shader.SetUbo(sizeof(Vec3f), curOffset, &position, gl::kUboLightsBinding);
		curOffset += sizeof(Vec4f);

		shader.SetUbo(sizeof(Vec3f), curOffset, &components_[e].diffuse, gl::kUboLightsBinding);
		curOffset += sizeof(Vec3f);

		shader.SetUbo(sizeof(float), curOffset, &components_[e].specular, gl::kUboLightsBinding);
		curOffset += sizeof(float);

		shader.SetUbo(sizeof(float), curOffset, &components_[e].intensity, gl::kUboLightsBinding);
		curOffset += sizeof(float);

		shader.SetUbo(sizeof(float), curOffset, &components_[e].radius, gl::kUboLightsBinding);
		curOffset += sizeof(float);

		mod = curOffset % 16;
		curOffset = curOffset + (mod == 0 ? 0 : 16 - mod);
	}
}
#else
void LightManager::SetShaderValues()
{
	const auto lights = entityManager_.get().FilterEntities(EntityMask(ComponentType::LIGHT));
	ResizeIfNecessary(components_, lights.back(), {});

	for (const auto& light : lights)
	{
		const Vec3f position = transformManager_.GetGlobalPosition(light);

		components_[light].position = position;
		vk::VkResources::Inst->lightCommandBuffer.Draw(components_[light]);
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
			ColorEdit3("Diffuse", &light.diffuse[0], LabelPos::LEFT);

			DragFloat("Specular", &light.specular, LabelPos::LEFT, 0.05f, 0.0f, 1.0f);
			DragFloat("Intensity", &light.intensity, LabelPos::LEFT, 0.05f, 0.0f, 1000.0f);
			DragFloat("Radius", &light.radius, LabelPos::LEFT, 0.05f, 0.0f, 1000.0f);

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

			component["specular"]  = lightManager_.GetSpecular(entity);
			component["intensity"] = lightManager_.GetIntensity(entity);
			component["radius"]    = lightManager_.GetRadius(entity);
		}
	}

	return component;
}

void LightViewer::SetComponentFromJson(Entity entity, const json& jsonComponent)
{
	PointLight light;
	light.diffuse = GetVector3FromJson(jsonComponent, "diffuse");

	light.specular  = jsonComponent["specular"];
	light.intensity = jsonComponent["intensity"];
	light.radius    = jsonComponent["radius"];

	lightManager_.SetComponent(entity, light);
}
}    // namespace neko::aer
