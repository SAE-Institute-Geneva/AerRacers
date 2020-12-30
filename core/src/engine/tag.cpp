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

#include <engine/tag.h>

#include <engine/log.h>
#include <engine/scene.h>

namespace neko {
TagManager::TagManager(SceneManager& sceneManager) : sceneManager_(sceneManager)
{
    
}

TagManager::~TagManager()
{
    
}

void TagManager::SetEntityTag(Entity entity, const std::string& entityTag)
{
    auto tags = sceneManager_.GetCurrentScene().tags;
    const auto entityTagIt = std::find_if(tags.begin(), tags.end(),
        [entityTag](std::string tagName)
        {
            return tagName == entityTag;
        });
    if (entityTagIt == tags.end())
    {
        SetEntityTag(entity, 0);
    }
    else
    {
        const TagIndex entityTagIndex = entityTagIt - tags.begin();
        SetEntityTag(entity, entityTagIndex);
    }
}

void TagManager::SetEntityTag(Entity entity, TagIndex entityTagIndex)
{
    if (entity != INVALID_ENTITY)
    {
        ResizeIfNecessary(entityTagArray_, entity, INVALID_ENTITY_MASK);
        entityTagArray_[entity] = entityTagIndex;
    }
    else
    {
        logDebug("Entity invalid");
        return;
    }
}

void TagManager::SetEntityLayer(Entity entity, LayerIndex entityLayerIndex)
{
    if (entity != INVALID_ENTITY)
    {
        ResizeIfNecessary(entityLayerArray_, entity, INVALID_ENTITY_MASK);
        entityLayerArray_[entity] = entityLayerIndex;
    }
    else
    {
        logDebug("Entity invalid");
        return;
    }
}

bool TagManager::IsEntityTag(
    Entity entity,
    const std::string& entityTag) const
{
    auto tags = sceneManager_.GetCurrentScene().tags;
    const auto entityTagIt = std::find_if(tags.begin(), tags.end(),
        [entityTag](std::string tagName)
        {
            return tagName == entityTag;
        });
    if (entityTagIt == tags.end())
    {
        logDebug("Tag " + entityTag + " not in scene");
        return false;
    }
    else
    {
        const TagIndex entityTagIndex = entityTagIt - tags.begin();
        return IsEntityTag(entity, entityTagIndex);
    }
}

bool TagManager::IsEntityTag(Entity entity, TagIndex entityTagIndex) const
{
    if (entity == INVALID_ENTITY || entity > entityTagArray_.size()) {
        logDebug("Entity invalid");
        return false;
    }
    return entityTagArray_[entity] == entityTagIndex;
}

bool TagManager::IsEntityLayer(
    Entity entity,
    LayerIndex entityLayerIndex) const
{
    if (entity == INVALID_ENTITY || entity > entityLayerArray_.size()) {
        logDebug("Entity invalid");
        return false;
    }
    return entityLayerArray_[entity] == entityLayerIndex;
}

bool TagManager::CompareEntitiesTag(Entity entityA, Entity entityB) const
{
    if (entityA == INVALID_ENTITY || entityA > entityTagArray_.size() || entityB == INVALID_ENTITY || entityB > entityTagArray_.size()) {
        logDebug("Entity invalid");
        return false;
    }
    return entityTagArray_[entityA] == entityTagArray_[entityB];
}
}
