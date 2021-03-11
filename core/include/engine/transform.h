#pragma once
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

#include "engine/component.h"
#include "engine/entity.h"
#include "globals.h"
#include "graphics/graphics.h"
#include "mathematics/matrix.h"
#include "mathematics/quaternion.h"
#include "mathematics/vector.h"

namespace neko
{
class Position2dManager : public ComponentManager<Vec2f, EntityMask(ComponentType::POSITION2D)>
{
	using ComponentManager::ComponentManager;
};

class Scale2dManager : public ComponentManager<Vec2f, EntityMask(ComponentType::SCALE2D)>
{
public:
	using ComponentManager::ComponentManager;
	void AddComponent(Entity entity) override;
};

class Rotation2dManager : public ComponentManager<degree_t, EntityMask(ComponentType::ROTATION2D)>
{
	using ComponentManager::ComponentManager;
};

class Position3dManager : public ComponentManager<Vec3f, EntityMask(ComponentType::POSITION3D)>
{
	using ComponentManager::ComponentManager;
};

class Rotation3dManager : public ComponentManager<EulerAngles, EntityMask(ComponentType::ROTATION3D)>
{
	using ComponentManager::ComponentManager;
};

class Scale3dManager : public ComponentManager<Vec3f, EntityMask(ComponentType::SCALE3D)>
{
public:
	using ComponentManager::ComponentManager;

	void AddComponent(Entity entity) override;
};

class TransformManagerInterface : public OnChangeParentInterface
{
public:
	virtual void UpdateDirtyComponent(Entity entity) = 0;
	virtual void Update()                            = 0;

protected:
	virtual void UpdateTransform(Entity entity) = 0;
};

class Transform2dManager : public ComponentManager<Mat4f, EntityMask(ComponentType::TRANSFORM2D)>,
						   public TransformManagerInterface
{
public:
	explicit Transform2dManager(EntityManager& entityManager);

	void Update() override;
	void UpdateDirtyComponent(Entity entity) override;

	[[nodiscard]] Vec2f GetPosition(Entity entity) const;
	[[nodiscard]] degree_t GetRotation(Entity entity) const;
	[[nodiscard]] Vec2f GetScale(Entity entity) const;
	void SetPosition(Entity entity, Vec2f position);
	void SetRotation(Entity entity, degree_t angles);
	void SetScale(Entity entity, Vec2f scale);

	void AddComponent(Entity entity) override;
	void OnChangeParent(Entity entity, Entity newParent, Entity oldParent) override;

protected:
	void UpdateTransform(Entity entity) override;

	Position2dManager positionManager_;
	Scale2dManager scaleManager_;
	Rotation2dManager rotationManager_;
	DirtyManager dirtyManager_;
};

class Transform3dManager
   : public DoubleBufferComponentManager<Mat4f, EntityMask(ComponentType::TRANSFORM3D)>,
	 public TransformManagerInterface

{
public:
	explicit Transform3dManager(EntityManager& entityManager);

	void Init();
	void Update() override;
	void UpdateDirtyComponent(Entity entity) override;

	[[nodiscard]] Vec3f GetRelativePosition(Entity entity) const;
	[[nodiscard]] EulerAngles GetRelativeRotation(Entity entity) const;
	[[nodiscard]] Vec3f GetRelativeScale(Entity entity) const;
	void SetRelativePosition(Entity entity, const Vec3f& position);
	void SetRelativeRotation(Entity entity, const EulerAngles& angles);
	void SetRelativeScale(Entity entity, const Vec3f& scale);

    [[nodiscard]] Vec3f GetGlobalPosition(Entity entity) const;
    [[nodiscard]] EulerAngles GetGlobalRotation(Entity entity) const;
    [[nodiscard]] Vec3f GetGlobalScale(Entity entity) const;
    void SetGlobalPosition(Entity entity, const Vec3f& position);
    void SetGlobalRotation(Entity entity, const EulerAngles& angles);
    void SetGlobalScale(Entity entity, const Vec3f& scale);

	void AddComponent(Entity entity) override;
	void OnChangeParent(Entity entity, Entity newParent, Entity oldParent) override;


protected:
	void UpdateTransform(Entity entity) override;

protected:
	Position3dManager position3DManager_;
	Scale3dManager scale3DManager_;
	Rotation3dManager rotation3DManager_;
	DirtyManager dirtyManager_;
};
//OldTransform3dViewer
//class Transform3dViewer : public DrawImGuiInterface
//{
//public:
//	explicit Transform3dViewer(
//		EntityManager& entityManager,
//		Transform3dManager& transform3dManager);
//	void DrawImGui() override;
//	void SetSelectedEntity(Entity selectedEntity) { selectedEntity_ = selectedEntity; };
//
//protected:
//	Entity selectedEntity_ = INVALID_ENTITY;
//	EntityManager& entityManager_;
//	Transform3dManager& transform3dManager_;
//};


/**
 * \brief The Component Manager use to serialize to json and imgui components
 */
class Transform3dViewer : public ComponentViewer
{
public:
	explicit Transform3dViewer(
		EntityManager& entityManager, Transform3dManager& transform3dManager);

	virtual ~Transform3dViewer() = default;

	/**
     * \brief Get a json object of the component of an entity
     * \return json object with component parameter
     */
	json GetJsonFromComponent(Entity entity) const override;

	/**
     * \brief Set a component of an entity from a json of the component
     * \componentJson json object with component parameter
     */
	void SetComponentFromJson(Entity entity, const json& jsonComponent) override;

	/**
     * \brief Draw the Imgui with the component parameter of an entity
     */
	void DrawImGui(Entity entity) override;

private:
	Transform3dManager& transform3dManager_;
};
}    // namespace neko
