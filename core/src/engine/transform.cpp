/*
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
 */

#include "engine/transform.h"
#include "mathematics/transform.h"
#include "utils/imgui_utility.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko
{
void Scale2dManager::AddComponent(Entity entity)
{
    ResizeIfNecessary(components_, entity, Vec2f::one);
    return ComponentManager::AddComponent(entity);
}

void Scale3dManager::AddComponent(Entity entity)
{
    ResizeIfNecessary(components_, entity, Vec3f::one);
    return ComponentManager::AddComponent(entity);
}

Transform2dManager::Transform2dManager(EntityManager& entityManager)
   : ComponentManager<Mat4f, EntityMask(ComponentType::TRANSFORM2D)>(entityManager),
	 positionManager_(entityManager),
	 scaleManager_(entityManager, Vec2f::one),
	 rotationManager_(entityManager),
	 dirtyManager_(entityManager)
{
	entityManager_.get().RegisterOnChangeParent(this);
	dirtyManager_.RegisterComponentManager(this);
}

void Transform2dManager::Update()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Update Transform");
#endif
	dirtyManager_.UpdateDirtyEntities();
}

void Transform2dManager::UpdateDirtyComponent(Entity entity) { UpdateTransform(entity); }

Vec2f Transform2dManager::GetPosition(Entity entity) const
{
    return positionManager_.GetComponent(entity);
}

degree_t Transform2dManager::GetRotation(Entity entity) const
{
    return rotationManager_.GetComponent(entity);
}

Vec2f Transform2dManager::GetScale(Entity entity) const
{
	return scaleManager_.GetComponent(entity);
}

void Transform2dManager::SetPosition(Entity entity, Vec2f position)
{
	positionManager_.SetComponent(entity, position);
	dirtyManager_.SetDirty(entity);
}

void Transform2dManager::SetRotation(Entity entity, degree_t angles)
{
	rotationManager_.SetComponent(entity, angles);
	dirtyManager_.SetDirty(entity);
}

void Transform2dManager::SetScale(Entity entity, Vec2f scale)
{
	scaleManager_.SetComponent(entity, scale);
	dirtyManager_.SetDirty(entity);
}

void Transform2dManager::AddComponent(Entity entity)
{
	positionManager_.AddComponent(entity);
	scaleManager_.AddComponent(entity);
	scaleManager_.SetComponent(entity, Vec2f::one);
	rotationManager_.AddComponent(entity);
	return ComponentManager::AddComponent(entity);
}

void Transform2dManager::OnChangeParent(Entity entity, Entity, Entity)
{
    //TODO change local transform to not change the global transform when changing parent
    dirtyManager_.SetDirty(entity);
}

void Transform2dManager::UpdateTransform(Entity entity)
{
	const auto eulerAngles =
		EulerAngles(degree_t(0), degree_t(0), -rotationManager_.GetComponent(entity));
	const auto scale = scaleManager_.GetComponent(entity);
	const auto position = positionManager_.GetComponent(entity);

	Mat4f transform =
		Transform3d::Transform(Vec3f(position, 0.0f), eulerAngles, Vec3f(scale, 1.0f));

	const auto parent = entityManager_.get().GetEntityParent(entity);
	if (parent != INVALID_ENTITY) { transform = GetComponent(parent) * transform; }

	SetComponent(entity, transform);
}

Transform3dManager::Transform3dManager(EntityManager& entityManager)
   : DoubleBufferComponentManager(entityManager),
	 position3DManager_(entityManager),
	 scale3DManager_(entityManager, Vec3f::one),
	 rotation3DManager_(entityManager),
	 dirtyManager_(entityManager)
{
    entityManager_.get().RegisterOnChangeParent(this);
    dirtyManager_.RegisterComponentManager(this);
}

void Transform3dManager::Init() { RendererLocator::get().RegisterSyncBuffersFunction(this); }

void Transform3dManager::Update()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Transform3dManager::Update", profiler::colors::Cyan);
#endif
	dirtyManager_.UpdateDirtyEntities();
}

void Transform3dManager::UpdateDirtyComponent(Entity entity) { UpdateTransform(entity); }

void Transform3dManager::UpdateTransform(Entity entity)
{
	Mat4f transform = Transform3d::Transform(position3DManager_.GetComponent(entity),
		rotation3DManager_.GetComponent(entity),
		scale3DManager_.GetComponent(entity));

	const auto parent = entityManager_.get().GetEntityParent(entity);
	if (parent != INVALID_ENTITY) { transform = GetComponent(parent) * transform; }

	SetComponent(entity, transform);
}

//Old Transform3dViewer
//Transform3dViewer::Transform3dViewer(
//	EntityManager& entityManager, Transform3dManager& transform3dManager)
//   : entityManager_(entityManager), transform3dManager_(transform3dManager)
//{}
//
//void Transform3dViewer::DrawImGui()
//{
//	if (selectedEntity_ == INVALID_ENTITY) return;
//
//	if (entityManager_.HasComponent(
//			selectedEntity_, static_cast<EntityMask>(ComponentType::TRANSFORM3D)))
//	{
//		auto position = transform3dManager_.GetRelativePosition(selectedEntity_);
//		if (ImGui::InputFloat3("Position", &position[0]))
//		{
//			transform3dManager_.SetRelativePosition(selectedEntity_, position);
//		}
//
//		auto scale = transform3dManager_.GetRelativeScale(selectedEntity_);
//		if (ImGui::InputFloat3("Scale", &scale[0]))
//		{
//			transform3dManager_.SetRelativeScale(selectedEntity_, scale);
//		}
//
//		Vec3f angles = Vec3f(transform3dManager_.GetRelativeRotation(selectedEntity_));
//		if (ImGui::InputFloat3("Euler Angles", &angles[0]))
//		{
//			const EulerAngles newEulerAngles =
//				EulerAngles(degree_t(angles[0]), degree_t(angles[1]), degree_t(angles[2]));
//			transform3dManager_.SetRelativeRotation(selectedEntity_, newEulerAngles);
//		}
//	}
//}

Vec3f Transform3dManager::GetRelativePosition(Entity entity) const
{
    return position3DManager_.GetComponent(entity);
}

EulerAngles Transform3dManager::GetRelativeRotation(Entity entity) const
{
	return rotation3DManager_.GetComponent(entity);
}

Vec3f Transform3dManager::GetRelativeScale(Entity entity) const
{
	return scale3DManager_.GetComponent(entity);
}

void Transform3dManager::SetRelativePosition(Entity entity, const Vec3f& position)
{
    position3DManager_.SetComponent(entity, position);
    UpdateTransform(entity);
	dirtyManager_.SetDirty(entity);
}

void Transform3dManager::SetRelativeRotation(Entity entity, const EulerAngles& angles)
{
    rotation3DManager_.SetComponent(entity, angles);
    UpdateTransform(entity);
	dirtyManager_.SetDirty(entity);
}

void Transform3dManager::SetRelativeScale(Entity entity, const Vec3f& scale)
{
    scale3DManager_.SetComponent(entity, scale);
    UpdateTransform(entity);
	dirtyManager_.SetDirty(entity);
}

Vec3f Transform3dManager::GetGlobalPosition(Entity entity) const
{
    return Transform3d::GetPosition(GetComponent(entity));
}

EulerAngles Transform3dManager::GetGlobalRotation(Entity entity) const
{
    //return GetRelativeRotation(entity);
    return Transform3d::GetRotation(GetComponent(entity));
}

Vec3f Transform3dManager::GetGlobalScale(Entity entity) const
{
    return Transform3d::GetScale(GetComponent(entity));
}

void Transform3dManager::SetGlobalPosition(Entity entity, const Vec3f& position)
{
    Mat4f transform = Transform3d::Transform(
        position,
        Transform3d::GetRotation(GetComponent(entity)), Transform3d::GetScale(GetComponent(entity)));
    SetComponent(entity, transform);
    const auto parent = entityManager_.get().GetEntityParent(entity);
    if (parent != INVALID_ENTITY) { transform = GetComponent(parent).Inverse() * transform; }

    position3DManager_.SetComponent(entity, Transform3d::GetPosition(transform));
    dirtyManager_.SetDirty(entity);
}

void Transform3dManager::SetGlobalRotation(Entity entity, const EulerAngles& angles)
{
    //SetRelativeRotation(entity, angles);
    //return;
	Mat4f transform   = Transform3d::Transform(Transform3d::GetPosition(GetComponent(entity)),
        angles,
        Transform3d::GetScale(GetComponent(entity)));
	const auto parent = entityManager_.get().GetEntityParent(entity);
	SetComponent(entity, transform);
	if (parent != INVALID_ENTITY) { transform = GetComponent(parent).Inverse() * transform; }

	rotation3DManager_.SetComponent(entity, Transform3d::GetRotation(transform));
	dirtyManager_.SetDirty(entity);
}

void Transform3dManager::SetGlobalScale(Entity entity, const Vec3f& scale)
{
    Mat4f transform = Transform3d::Transform(Transform3d::GetPosition(GetComponent(entity)),
        Transform3d::GetRotation(GetComponent(entity)), scale);
    const auto parent = entityManager_.get().GetEntityParent(entity);
    SetComponent(entity, transform);
    if (parent != INVALID_ENTITY) { transform = GetComponent(parent).Inverse() * transform; }

    scale3DManager_.SetComponent(entity, Transform3d::GetScale(transform));
    dirtyManager_.SetDirty(entity);
}

void Transform3dManager::AddComponent(Entity entity)
{
    position3DManager_.AddComponent(entity);
    scale3DManager_.AddComponent(entity);
    scale3DManager_.SetComponent(entity, Vec3f::one);
    rotation3DManager_.AddComponent(entity);
    return DoubleBufferComponentManager::AddComponent(entity);
}

void Transform3dManager::OnChangeParent(Entity entity, Entity, Entity)
{
    Mat4f transform = GetComponent(entity);
    SetGlobalScale(entity, Transform3d::GetScale(transform));
    SetGlobalRotation(entity, Transform3d::GetRotation(transform));
    SetGlobalPosition(entity, Transform3d::GetPosition(transform));
	dirtyManager_.SetDirty(entity);
}

Transform3dViewer::Transform3dViewer(
    EntityManager& entityManager, Transform3dManager& transform3dManager) : ComponentViewer(entityManager), transform3dManager_(transform3dManager)
{}

json Transform3dViewer::GetJsonFromComponent(Entity entity) const
{
	json transformComponent = json::object();
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D)))
	{
		if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
		{
			transformComponent["position"] =
				GetJsonFromVector3(transform3dManager_.GetGlobalPosition(entity));
			transformComponent["rotation"] = GetJsonFromVector4(
				Vec4f(Quaternion::FromEuler(transform3dManager_.GetGlobalRotation(entity))));
			transformComponent["scale"] =
				GetJsonFromVector3(transform3dManager_.GetGlobalScale(entity));
		}
	}
	return transformComponent;
}

void Transform3dViewer::SetComponentFromJson(Entity entity, const json& jsonComponent)
{
    if (CheckJsonParameter(jsonComponent, "position", json::object())) {
		Vec3f position = GetVector3FromJson(jsonComponent, "position");
		//Fix for Unity Export
		position = Vec3f(-position.x, position.y, position.z);
        transform3dManager_.SetRelativePosition(
            entity,
			position);
    }
    if (CheckJsonParameter(jsonComponent, "rotation", json::object())) {
        transform3dManager_.SetRelativeRotation(entity,
            Quaternion::ToEulerAngles(Quaternion(GetVector4FromJson(jsonComponent, "rotation"))));
    }
    if (CheckJsonParameter(jsonComponent, "scale", json::object())) {
        transform3dManager_.SetRelativeScale(entity, GetVector3FromJson(jsonComponent, "scale"));
    }
}

void Transform3dViewer::DrawImGui(Entity entity)
{
	if (entity == INVALID_ENTITY) return;
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D)))
	{
		if (ImGui::TreeNode("Transform"))
		{
			static bool globalPos;
			ImGui::Checkbox("GlobalPos", &globalPos);

			Vec3f position;
			if (globalPos) position = transform3dManager_.GetGlobalPosition(entity);
			else position = transform3dManager_.GetRelativePosition(entity);
			if (ImGui::DragFloat3("Position", position.coord, ImGui::LabelPos::LEFT, 0.05f))
				transform3dManager_.SetRelativePosition(entity, position);

			Vec3f rotation;
			if (globalPos) rotation = Vec3f(transform3dManager_.GetGlobalRotation(entity));
			else rotation = Vec3f(transform3dManager_.GetRelativeRotation(entity));
			if (ImGui::DragFloat3("Rotation", rotation.coord, ImGui::LabelPos::LEFT, 0.05f))
				transform3dManager_.SetRelativeRotation(
					entity, EulerAngles(rotation.x, rotation.y, rotation.z));

			Vec3f scale;
			if (globalPos) scale = transform3dManager_.GetGlobalScale(entity);
			else scale = transform3dManager_.GetRelativeScale(entity);
			if (ImGui::DragFloat3("Scale", scale.coord, ImGui::LabelPos::LEFT, 0.05f))
				transform3dManager_.SetRelativeScale(entity, scale);
			ImGui::TreePop();
		}
	}
}
}// namespace neko
 