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

 Author : Floreau Luca
 Co-Author :
 Date : 13.01.2021
---------------------------------------------------------- */

#include "engine/entity.h"

#include "utils/service_locator.h"

namespace neko::aer
{
using TagIndex   = unsigned;
using LayerIndex = unsigned;

const std::string INVALID_TAG   = "Untagged";
const std::string INVALID_LAYER = "Default";

class SceneManager;

//-----------------------------------------------------------------------------
// TagManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class ITagManager
{
protected:
	~ITagManager() = default;

public:
	/**
	 * \brief Set the tag of an entity
	 * \param entityTag entity tag name
	 */
	virtual void SetEntityTag(Entity entity, const std::string& entityTag) = 0;

	/**
     * \brief Set the tag of an entity
     * \param entityTagIndex entity tag index
     */
	virtual void SetEntityTag(Entity entity, TagIndex entityTagIndex) = 0;

	/**
     * \brief Set the layer of an entity
     * \param entityLayer entity layer
     */
	virtual void SetEntityLayer(Entity entity, const std::string& entityLayer) = 0;

	/**
     * \brief Set the layer of an entity
     * \param entityLayerIndex entity layer index
     */
	virtual void SetEntityLayer(Entity entity, LayerIndex entityLayerIndex) = 0;

	/**
	 * \brief Return if an entity has the tag
	 * \param entityTag entity tag name
	 */
	virtual bool IsEntityTag(Entity entity, const std::string& entityTag) const = 0;

	/**
     * \brief Return if an entity has the tag
     * \param entityTagIndex entity tag index
     */
	virtual bool IsEntityTag(Entity entity, TagIndex entityTagIndex) const = 0;

	/**
     * \brief Return if an entity has the layer
     * \param entityLayer entity layer name
     */
	virtual bool IsEntityLayer(Entity entity, const std::string& entityLayer) const = 0;

	/**
     * \brief Return if an entity has the layer
     * \param entityLayerIndex entity layer index
     */
	virtual bool IsEntityLayer(Entity entity, LayerIndex entityLayerIndex) const = 0;

	/**
     * \brief Compare to entities tag
     * \param entityA first entity index
     * \param entityB second entity index
     * \return true if two entity has the same tag
     */
	virtual bool CompareEntitiesTag(Entity entityA, Entity entityB) const = 0;

	/**
     * \brief Return the tag name of an entity
     */
	virtual std::string GetEntityTag(Entity entity) const = 0;

	/**
     * \brief Get the entity tag index
     */
	virtual TagIndex GetEntityTagIndex(Entity entity) const = 0;

	/**
     * \brief Return the layer name of an entity
     */
	virtual std::string GetEntityLayer(Entity entity) const = 0;

	/**
     * \brief Get the entity layer index
     */
	virtual LayerIndex GetEntityLayerIndex(Entity entity) const = 0;
};

//-----------------------------------------------------------------------------
// NullTagManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullTagManager final : public ITagManager
{
public:
	std::string GetEntityTag(Entity) const override { return "Untagged"; }
	TagIndex GetEntityTagIndex(Entity) const override { return 0; }
	void SetEntityTag(Entity, const std::string&) override {}
	void SetEntityTag(Entity, TagIndex) override {}
	bool IsEntityTag(Entity, const std::string&) const override { return false; }
	bool IsEntityTag(Entity, TagIndex) const override { return false; }
	bool CompareEntitiesTag(Entity, Entity) const override { return false; }

	std::string GetEntityLayer(Entity) const override { return "Default"; }
	LayerIndex GetEntityLayerIndex(Entity) const override { return 0; }
	void SetEntityLayer(Entity, LayerIndex) override {}
	void SetEntityLayer(Entity, const std::string&) override {}
	bool IsEntityLayer(Entity, LayerIndex) const override { return false; }
	bool IsEntityLayer(Entity, const std::string&) const override { return false; }
};

//-----------------------------------------------------------------------------
// TagManager
//-----------------------------------------------------------------------------
/// \brief Stores entity tags
class TagManager final : public ITagManager
{
public:
	TagManager(SceneManager& sceneManager);
	~TagManager();

	std::string GetEntityTag(Entity entity) const override;
	TagIndex GetEntityTagIndex(Entity entity) const override;
	void SetEntityTag(Entity entity, const std::string& entityTag) override;
	void SetEntityTag(Entity entity, TagIndex entityTagIndex) override;
	bool IsEntityTag(Entity entity, const std::string& entityTag) const override;
	bool IsEntityTag(Entity entity, TagIndex entityTagIndex) const override;
	bool CompareEntitiesTag(Entity entityA, Entity entityB) const override;

	std::string GetEntityLayer(Entity entity) const override;
	LayerIndex GetEntityLayerIndex(Entity entity) const override;
	void SetEntityLayer(Entity entity, const std::string& entityLayer) override;
	void SetEntityLayer(Entity entity, LayerIndex entityLayerIndex) override;
	bool IsEntityLayer(Entity entity, const std::string& entityLayer) const override;
	bool IsEntityLayer(Entity entity, LayerIndex entityLayerIndex) const override;

private:
	std::vector<TagIndex> entityTagArray_;
	std::vector<LayerIndex> entityLayerArray_;

	SceneManager& sceneManager_;
};

//-----------------------------------------------------------------------------
// Service Locator definition
//-----------------------------------------------------------------------------
using TagLocator = Locator<ITagManager, NullTagManager>;
}    // namespace neko::aer
