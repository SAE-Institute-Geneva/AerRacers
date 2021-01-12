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

#include <utils/json_utility.h>
#include "engine/entity.h"
#include <engine/component.h>

#include "tag.h"
#include "graphics/color.h"
#include "engine/filesystem.h"
#include "utils/service_locator.h"
#include <engine/transform.h>


namespace neko
{
using InstanceId = int;
const InstanceId INVALID_INSTANCE_ID = -1;
using SceneId = sole::uuid;
const SceneId INVALID_SCENE_ID = sole::uuid{};
struct Scene
{
    std::string sceneName = "New Scene";
    std::string scenePath = "";
    std::vector<std::string> layers = {INVALID_LAYER};
    std::vector<std::string> tags = { INVALID_TAG };
};

class SceneManager 
{
public:
    explicit SceneManager(EntityManager& entityManager, const FilesystemInterface& fileSystem, Transform3dManager& transform3dManager);
	virtual ~SceneManager() = default;
    virtual void ParseComponentJson(const json& componentJson, Entity entity);
    virtual void ParseEntityJson(const json& entityJson);
    virtual void ParseSceneJson(const json& sceneJson);
    bool LoadScene(const std::string_view& jsonPath);
    const Scene& GetCurrentScene() const { return currentScene_;}
    void SetSceneName(const std::string& sceneName) { currentScene_.sceneName = sceneName; }
	static SceneId GenerateSceneId() { return sole::uuid0(); };
	static std::string_view GetExtension();
    void SaveCurrentScene();
    virtual json WriteEntityJson(Entity entity);
    virtual json WriteSceneJson();
    virtual void AddTag(const std::string& tagName);
    virtual void AddLayer(const std::string& layerName);
    virtual const std::vector<std::string>& GetTags() const;
    virtual const std::vector<std::string>& GetLayers() const;
protected:
    const FilesystemInterface& filesystem_;
    Scene currentScene_;
    EntityManager& entityManager_;
    TagManager tagManager_;

    Transform3dManager& transformManager_;
    std::vector<InstanceId> entityInstanceIdArray_;
};
}
