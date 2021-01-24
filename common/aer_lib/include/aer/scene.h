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
 Date : 12.01.2021
---------------------------------------------------------- */

#include <engine/entity.h>
#include <engine/filesystem.h>
#include <utils/json_utility.h>

#include "aer/tag.h"

namespace neko::aer
{
struct ComponentManagerContainer;
/**
 * \brief Temporary InstanceId use to determined parents
 */
using InstanceId                     = int;
const InstanceId INVALID_INSTANCE_ID = -1;

/**
 * \brief Struct of scene infos
 */
struct Scene
{
	std::string sceneName           = "New Scene";
	std::string scenePath           = "";
	std::vector<std::string> layers = {INVALID_LAYER};
	std::vector<std::string> tags   = {INVALID_TAG};
};

/**
 * \brief Manager of the current scene
 */
class SceneManager
{
public:
    explicit SceneManager(
        EntityManager& entityManager, ComponentManagerContainer& componentManagerContainer);
	~SceneManager() = default;

	/**
     * \brief Parse the components of an entity from the json of an entity
     * \param componentJson json object of an entity
     */
	void ParseComponentJson(const json& componentJson, Entity entity);

	/**
     * \brief Parse the entity infos from the json of an entity
     * \param entityJson json object of an entity
     */
	void ParseEntityJson(const json& entityJson);

	/**
     * \brief Parse the scene infos from the json of an scene
     * \param sceneJson json object of the scene
     */
	void ParseSceneJson(const json& sceneJson);

	/**
     * \brief Load scene as current scene
     * \return true if loading succeed
     */
	bool LoadScene(const std::string_view& jsonPath);

	/**
     * \brief Get the current scene infos
     */
	const Scene& GetCurrentScene() const { return currentScene_; }

	/**
     * \brief Set the name of of current scene
     */
	void SetSceneName(const std::string& sceneName) { currentScene_.sceneName = sceneName; }

	/**
     * \brief Save the current scene on a file
     */
	void SaveCurrentScene();

	/**
     * \brief Write the infos and components of an entity on json object
     */
	json WriteEntityJson(Entity entity) const;

	/**
     * \brief Write the current scene infos on json object
     */
	json WriteSceneJson();

	/**
     * \brief Add new tag to the current scene
     */
	void AddTag(const std::string& newTagName);

	/**
     * \brief Add new layer to the current scene
     */
	void AddLayer(const std::string& newLayerName);

	/**
     * \brief Get tag list of the current scene
     */
	const std::vector<std::string>& GetTags() const;

	/**
     * \brief Get layer list of the current scene
     */
	const std::vector<std::string>& GetLayers() const;

protected:
	const FilesystemInterface& filesystem_;

	EntityManager& entityManager_;
	TagManager tagManager_;
	ComponentManagerContainer& componentManagerContainer_;

	Scene currentScene_;

	/**
     * \brief Temporary vector of entities InstanceId 
     */
	std::vector<InstanceId> entityInstanceIdArray_;

    /**
     * \brief Temporary vector of entities parent InstanceId 
     */
    std::vector<InstanceId> entityParentInstanceIdArray_;
};
}    // namespace neko::aer
