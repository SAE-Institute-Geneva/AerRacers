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

namespace neko
{

static const std::string_view sceneExtension = ".scene";

neko::SceneManager::SceneManager(EntityManager& entityManager, const FilesystemInterface& filesystem) :
    filesystem_(filesystem),
    entityManager_(entityManager)
{

}

void SceneManager::ParseComponentJson(
    const json& componentJson,
    Entity entity)
{
    if (CheckJsonParameter(componentJson, "transform", json::value_t::string))
    {
        currentScene_.sceneName = componentJson["transform"];
    }
}

void SceneManager::ParseEntityJson(const json& entityJson)
{
    Entity entity = entityManager_.CreateEntity();
    if (CheckJsonParameter(entityJson, "name", json::value_t::string))
    {
        currentScene_.sceneName = entityJson["name"];
    }
    if (CheckJsonParameter(entityJson, "instanceId", json::value_t::string))
    {
        currentScene_.sceneName = entityJson["instanceId"];
    }

    if (CheckJsonParameter(entityJson, "parent", json::value_t::string))
    {
        currentScene_.sceneName = entityJson["parent"];
    }

    if (CheckJsonParameter(entityJson, "layer", json::value_t::string))
    {
        currentScene_.sceneName = entityJson["layer"];
    }
    if (CheckJsonParameter(entityJson, "tag", json::value_t::string))
    {
        currentScene_.sceneName = entityJson["tag"];
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

    if (CheckJsonParameter(sceneJson, "objects", json::value_t::array))
    {
        for (auto& entityJson : sceneJson["objects"])
        {
            ParseEntityJson(entityJson);
        }
    }
}

void SceneManager::LoadScene(const std::string_view jsonPath)
{
    if (filesystem_.FileExists(jsonPath))
    {
        json scene = neko::LoadJson(jsonPath);
        currentScene_.scenePath = jsonPath;
        ParseSceneJson(scene);
    }
    else
    {
        logDebug(fmt::format("Scene not found", currentScene_.sceneName));
    }
}

void SceneManager::SetCurrentScene(const Scene& currentScene)
{
    currentScene_ = currentScene;
}

std::string_view SceneManager::GetExtension()
{
    return sceneExtension;
}

}