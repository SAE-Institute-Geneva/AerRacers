#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author : Simon Canas
 Co-Author :
 Date : 09.03.2021
---------------------------------------------------------- */
#include "engine/transform.h"
#include "graphics/lights.h"

namespace neko::aer
{
class LightManager : public ComponentManager<PointLight, EntityMask(ComponentType::LIGHT)>
{
public:
	using ComponentManager::ComponentManager;
	explicit LightManager(EntityManager& entityManager, Transform3dManager& transformManager);

#ifdef NEKO_GLES3
	void SetShaderValues(gl::Shader& shader);
#endif

	[[nodiscard]] float GetRadius(Entity e) const { return components_[e].radius; }
	[[nodiscard]] const Vec3f& GetDiffuse(Entity e) const { return components_[e].diffuse; }
	[[nodiscard]] float GetSpecular(Entity e) const { return components_[e].specular; }
	[[nodiscard]] float GetIntensity(Entity e) const { return components_[e].intensity; }

	void SetRadius(Entity e, float radius) { components_[e].radius = radius; }
	void SetDiffuse(Entity e, Vec3f diffuse) { components_[e].diffuse = diffuse; }
	void SetSpecular(Entity e, float specular) { components_[e].specular = specular; }
	void SetIntensity(Entity e, float intensity) { components_[e].intensity = intensity; }

private:
	Transform3dManager& transformManager_;
};

class LightViewer : public ComponentViewer
{
public:
	LightViewer(EntityManager& entityManager, LightManager& lightManager);

	void DrawImGui(Entity entity) override;

	[[nodiscard]] json GetJsonFromComponent(Entity entity) const override;
	void SetComponentFromJson(Entity entity, const json& jsonComponent) override;

private:
	LightManager& lightManager_;
};
}    // namespace neko::aer
