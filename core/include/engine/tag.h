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

#include <sole.hpp>

#include "engine/entity.h"
#include "utils/service_locator.h"


namespace neko
{
using TagIndex = unsigned;
using LayerIndex = unsigned;

class SceneManager;

//-----------------------------------------------------------------------------
// TagManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class TagManagerInterface
{
protected:
	~TagManagerInterface() = default;
public:
	virtual void SetEntityTag(Entity entity, const std::string& entityTag) = 0;

	virtual void SetEntityTag(Entity entity, TagIndex entityTagIndex) = 0;

    virtual void SetEntityLayer(Entity entity, const std::string& entityLayer) = 0;

	virtual void SetEntityLayer(Entity entity, LayerIndex entityLayerIndex) = 0;

	virtual bool IsEntityTag(Entity entity, const std::string& entityTag) const = 0;

	virtual bool IsEntityTag(Entity entity, TagIndex entityTagIndex) const = 0;

    virtual bool IsEntityLayer(Entity entity, const std::string& entityLayer) const = 0;

	virtual bool IsEntityLayer(Entity entity, LayerIndex entityLayerIndex) const = 0;

    virtual bool CompareEntitiesTag(Entity entityA, Entity entityB) const = 0;

};

//-----------------------------------------------------------------------------
// NullTagManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullTagManager final : public TagManagerInterface
{
public:
    void SetEntityTag([[maybe_unused]] Entity entity, [[maybe_unused]] const std::string& entityTag) override
    {
        
    }

    void SetEntityTag([[maybe_unused]] Entity entity, [[maybe_unused]] TagIndex entityTagIndex) override
    {
        
    }

    void SetEntityLayer([[maybe_unused]] Entity entity, [[maybe_unused]] LayerIndex entityLayerIndex) override
    {
        
    }

    bool IsEntityTag(
        [[maybe_unused]] Entity entity,
        [[maybe_unused]] const std::string& entityTag) const override
    {
        return false;
    }

    bool IsEntityTag([[maybe_unused]] Entity entity, [[maybe_unused]] TagIndex entityTagIndex) const override
    {
        return false;
    }

    bool IsEntityLayer(
        [[maybe_unused]] Entity entity,
        [[maybe_unused]] LayerIndex entityLayerIndex) const override
    {
        return false;
    }

    bool CompareEntitiesTag([[maybe_unused]] Entity entityA, [[maybe_unused]] Entity entityB) const override
    {
        return false;
    }

    void SetEntityLayer(Entity entity, const std::string& entityLayer) override
    {
        
    }
    bool IsEntityLayer(
        Entity entity,
        const std::string& entityLayer) const override
    {
        return false;
    }
};

//-----------------------------------------------------------------------------
// TagManager
//-----------------------------------------------------------------------------
/// \brief Stores entity tags
class TagManager final : public TagManagerInterface
{
public:
	TagManager(SceneManager& sceneManager);
	~TagManager();

    void SetEntityTag(Entity entity, const std::string& entityTag) override;

    void SetEntityTag(Entity entity, TagIndex entityTagIndex) override;

    void SetEntityLayer(Entity entity, LayerIndex entityLayerIndex) override;

    bool IsEntityTag(
        Entity entity,
        const std::string& entityTag) const override;

    bool IsEntityTag(Entity entity, TagIndex entityTagIndex) const override;

    bool IsEntityLayer(
        Entity entity,
        LayerIndex entityLayerIndex) const override;
    bool CompareEntitiesTag(Entity entityA, Entity entityB) const override;
    void SetEntityLayer(Entity entity, const std::string& entityLayer) override;
    bool IsEntityLayer(
        Entity entity,
        const std::string& entityLayer) const override;
private:
	std::vector<TagIndex> entityTagArray_;
	std::vector<LayerIndex> entityLayerArray_;

	SceneManager& sceneManager_;
};

//-----------------------------------------------------------------------------
// Service Locator definition
//-----------------------------------------------------------------------------
using TagLocator = Locator<TagManagerInterface, NullTagManager>;

}
