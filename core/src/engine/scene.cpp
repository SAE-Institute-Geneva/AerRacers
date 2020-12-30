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

#include <engine/scene.h>
#include <utils/file_utility.h>
#include <engine/log.h>

#include <fmt/format.h>

#include "engine/tag.h"

namespace neko
{

static const std::string_view sceneExtension = ".scene";

neko::SceneManager::SceneManager(EntityManager& entityManager, const FilesystemInterface& filesystem) :
    filesystem_(filesystem),
    entityManager_(entityManager),
    tagManager_(*this)
{
    TagLocator::provide(&tagManager_);
}

void SceneManager::ParseComponentJson(
    const json& componentJson,
    Entity entity)
{
    if (CheckJsonParameter(componentJson, "transform", json::value_t::object))
    {
        entityManager_.AddComponentType(entity, EntityMask(ComponentType::TRANSFORM3D));
    }
}

void SceneManager::ParseEntityJson(const json& entityJson)
{
    Entity entity = entityManager_.CreateEntity();
    if (CheckJsonParameter(entityJson, "name", json::value_t::string))
    {
        //entityManager_.SetEntityName(entity, entityJson["name"]); TODO(@Luca) set with name
    }
    if (CheckJsonParameter(entityJson, "instanceId", json::value_t::number_integer))
    {
        int instanceId = entityJson["instanceId"];
        //entityManager_.SetEntityName(entity, std::to_string(instanceId));
    }

    if (CheckJsonParameter(entityJson, "parent", json::value_t::number_integer))
    {
        int parentInstanceId = entityJson["parent"];
        //entityManager_.SetEntityParent(entity, entityManager_.FindEntityByName(std::to_string(parentInstanceId)));
    }

    if (CheckJsonParameter(entityJson, "layer", json::value_t::number_unsigned))
    {
        TagLocator::get().SetEntityLayer(entity, entityJson["layer"]);
    }
    if (CheckJsonParameter(entityJson, "tag", json::value_t::string))
    {
        TagLocator::get().SetEntityTag(entity, static_cast<std::string>(entityJson["tag"]));
    }
    ParseComponentJson(entityJson, entity);
}

void SceneManager::ParseSceneJson(const json& sceneJson)
{
    if (CheckJsonParameter(sceneJson, "name", json::value_t::string))
    {
        currentScene_.sceneName = sceneJson["name"];
    }
    else
    {
        currentScene_.sceneName = "New Scene";
    }
    if (CheckJsonParameter(sceneJson, "tags", json::value_t::array))
    {
        for (auto& tag : sceneJson["tags"])
        {
            currentScene_.tags.push_back(tag);
        }
    }
    if (CheckJsonParameter(sceneJson, "layers", json::value_t::array))
    {
        for (auto& layer : sceneJson["layers"])
        {
            currentScene_.layers.push_back(layer);
        }
    }
    if (CheckJsonParameter(sceneJson, "objects", json::value_t::array))
    {
        for (auto& entityJson : sceneJson["objects"])
        {
            ParseEntityJson(entityJson);
        }
    }
}

bool SceneManager::LoadScene(const std::string_view& jsonPath)
{
    if (filesystem_.FileExists(jsonPath))
    {
        json scene = neko::LoadJson(jsonPath);
        currentScene_.scenePath = jsonPath;
        ParseSceneJson(scene);
        return true;
    }
    else
    {
        logDebug(fmt::format("Scene not found", currentScene_.sceneName));
        return false;
    }
}

std::string_view SceneManager::GetExtension()
{
    return sceneExtension;
}

}
