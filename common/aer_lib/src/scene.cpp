#include "aer/scene.h"

#include <fmt/format.h>
#ifdef EASY_PROFILE_USE
    #include "easy/profiler.h"
#endif

#include "aer/log.h"
#include "aer/tag.h"
#include "aer/managers/manager_container.h"
#include "engine/configuration.h"
#include "engine/engine.h"
#include "utils/file_utility.h"

namespace neko::aer
{
constexpr std::string_view SceneExtension = ".scene";

SceneManager::SceneManager(
	EntityManager& entityManager, ComponentManagerContainer& componentManagerContainer)
   : filesystem_(BasicEngine::GetInstance()->GetFilesystem()),
	 entityManager_(entityManager),
	 tagManager_(*this),
	 componentManagerContainer_(componentManagerContainer)
{
	TagLocator::provide(&tagManager_);
}

void SceneManager::ParseComponentJson(const json& componentJson, Entity entity)
{
	if (CheckJsonParameter(componentJson, "transform", json::value_t::object))
	{
		if (CheckJsonParameter(componentJson["transform"], "exist", json::value_t::boolean))
		{
			if (componentJson["transform"]["exist"])
			{
				componentManagerContainer_.transform3dManager.AddComponent(entity);
				componentManagerContainer_.transform3dViewer.SetComponentFromJson(
					entity, componentJson["transform"]);
			}
		}
	}

	if (CheckJsonParameter(componentJson, "rigidbody", json::value_t::object))
	{
		if (CheckJsonParameter(componentJson["rigidbody"], "exist", json::value_t::boolean))
		{
			if (componentJson["rigidbody"]["exist"])
			{
				if (CheckJsonParameter(
						componentJson["rigidbody"], "isStatic", json::value_t::boolean))
				{
					if (componentJson["rigidbody"]["isStatic"])
					{
						componentManagerContainer_.rigidStaticViewer.SetComponentFromJson(
							entity, componentJson["rigidbody"]);
					}
					else
					{
						componentManagerContainer_.rigidDynamicViewer.SetComponentFromJson(
							entity, componentJson["rigidbody"]);
					}
				}
			}
		}
	}

	//if (CheckJsonParameter(componentJson, "shipControl", json::value_t::object))
	//{
	//	if (CheckJsonParameter(componentJson["shipControl"], "exist", json::value_t::boolean))
	//	{
	//		if (componentJson["shipControl"]["exist"])
	//		{
 //               componentManagerContainer_.shipControllerManager.AddComponent(entity);
	//		}
	//	}
	//}

	if (CheckJsonParameter(componentJson, "modelRenderer", json::value_t::object))
	{
		if (CheckJsonParameter(componentJson["modelRenderer"], "exist", json::value_t::boolean))
		{
			if (componentJson["modelRenderer"]["exist"])
			{
				componentManagerContainer_.renderManager.AddComponent(entity);
				componentManagerContainer_.rendererViewer.SetComponentFromJson(
					entity, componentJson["modelRenderer"]);
			}
		}
	}
}

void SceneManager::ParseEntityJson(const json& entityJson)
{
    if (CheckJsonParameter(entityJson, "isActive", json::value_t::boolean)) {
        if (!entityJson["isActive"]) return;
    }
    Entity entity = entityManager_.CreateEntity();
    if (CheckJsonParameter(entityJson, "name", json::value_t::string)) {
        //entityManager_.SetEntityName(entity, entityJson["name"]); TODO(@Luca) Set when name is done
    }

    if (CheckJsonNumber(entityJson, "instanceId")) {
        InstanceId instanceId = entityJson["instanceId"];
        ResizeIfNecessary(entityInstanceIdArray_, entity, INVALID_INSTANCE_ID);
        entityInstanceIdArray_[entity] = instanceId;
    }

    if (CheckJsonNumber(entityJson, "parent")) {
        InstanceId instanceId = entityJson["parent"];
        ResizeIfNecessary(entityParentInstanceIdArray_, entity, INVALID_INSTANCE_ID);
        entityParentInstanceIdArray_[entity] = instanceId;
    }

    if (CheckJsonParameter(entityJson, "layer", json::value_t::string)) {
        TagLocator::get().SetEntityLayer(entity, static_cast<std::string>(entityJson["layer"]));
    }

    if (CheckJsonParameter(entityJson, "tag", json::value_t::string)) {
        TagLocator::get().SetEntityTag(entity, static_cast<std::string>(entityJson["tag"]));
    }

    //TODO (@Luca) Set active

    ParseComponentJson(entityJson, entity);
}

void SceneManager::ParseSceneJson(const json& sceneJson)
{
    if (CheckJsonParameter(sceneJson, "name", json::value_t::string)) {
        currentScene_.sceneName = sceneJson["name"];
    } else { currentScene_.sceneName = "New Scene"; }

    if (CheckJsonParameter(sceneJson, "tags", json::value_t::array)) {
        for (auto& tag : sceneJson["tags"]) { if (!TagExist(tag)) { AddTag(tag); } }
    }

	if (CheckJsonParameter(sceneJson, "layers", json::value_t::array))
	{
		for (auto& layer : sceneJson["layers"])
		{
			if (!LayerExist(layer)) { AddLayer(layer); }
		}
	}

	if (CheckJsonParameter(sceneJson, "objects", json::value_t::array))
	{
		for (auto& entityJson : sceneJson["objects"]) { ParseEntityJson(entityJson); }
		for (Entity entity = 0; entity < entityInstanceIdArray_.size(); ++entity)
		{
			InstanceId parentInstanceId = entityParentInstanceIdArray_[entity];
			if (parentInstanceId != INVALID_INSTANCE_ID)
			{
				const auto entityInstanceIdIt = std::find_if(entityInstanceIdArray_.begin(),
					entityInstanceIdArray_.end(),
					[parentInstanceId](InstanceId instanceId)
					{ return instanceId == parentInstanceId; });
				if (entityInstanceIdIt != entityInstanceIdArray_.end())
				{
					const Entity parentEntity = entityInstanceIdIt - entityInstanceIdArray_.begin();
					entityManager_.SetEntityParent(entity, parentEntity);
				}
			}
		}
	}
}

bool SceneManager::LoadScene(const std::string_view& jsonPath)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("LoadScene");
#endif
	if (filesystem_.FileExists(jsonPath))
	{
		json scene              = neko::LoadJson(jsonPath);
		currentScene_.scenePath = jsonPath;
		entityManager_.CleanEntity();
		ParseSceneJson(scene);
		return true;
	}
	else
	{
		LogDebug(fmt::format("Scene not found", currentScene_.sceneName));
		return false;
	}
}

void SceneManager::SaveCurrentScene()
{
	const neko::Configuration config = neko::BasicEngine::GetInstance()->GetConfig();
	WriteStringToFile(config.dataRootPath + "scenes/" + currentScene_.sceneName + ".aerscene",
		WriteSceneJson().dump(4));
	currentScene_.saved = true;
}

json SceneManager::WriteEntityJson(Entity entity) const
{
	json entityJson = json::object();
	//entity["name"] = TODO(@Luca) Implement name
	entityJson["tag"]        = TagLocator::get().GetEntityTag(entity);
	entityJson["instanceId"] = entity;
	entityJson["parent"]     = entityManager_.GetEntityParent(entity);
	entityJson["layer"]      = TagLocator::get().GetEntityLayer(entity);
	//entityJson["isActive"] = TagLocator::get().GetEntityTag(entity);  //TODO (@Luca) Set active
	entityJson["transform"] = json::object();
	entityJson["transform"] =
		componentManagerContainer_.transform3dViewer.GetJsonFromComponent(entity);
	entityJson["transform"]["exist"] =
		entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	entityJson["rigidbody"] = json::object();
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC)))
	{
		entityJson["rigidbody"] =
			componentManagerContainer_.rigidStaticViewer.GetJsonFromComponent(entity);
	}
	else if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC)))
	{
		entityJson["rigidbody"] =
			componentManagerContainer_.rigidDynamicViewer.GetJsonFromComponent(entity);
	}
	entityJson["rigidbody"]["exist"] =
		entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC)) ||
		entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC));
	//entityJson["shipControl"] = json::object();
	//entityJson["shipControl"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["shipControl"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	//entityJson["shipRotation"] = json::object();
	//entityJson["shipRotation"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["shipRotation"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	entityJson["modelRenderer"] = json::object();
	entityJson["modelRenderer"] =
		componentManagerContainer_.rendererViewer.GetJsonFromComponent(entity);
	entityJson["modelRenderer"]["exist"] =
		entityManager_.HasComponent(entity, EntityMask(ComponentType::MODEL));
	return entityJson;
}

json SceneManager::WriteSceneJson()
{
	json scene       = json::object();
	scene["name"]    = currentScene_.sceneName;
	scene["tags"]    = currentScene_.tags;
	scene["layers"]  = currentScene_.layers;
	scene["objects"] = json::array_t();
	for (size_t i = 0; i < entityManager_.GetEntitiesNmb(); ++i)
	{
		scene["objects"][i] = WriteEntityJson(i);
	}
	return scene;
}

void SceneManager::AddTag(const std::string& newTagName)
{
	if (!LayerExist(newTagName)) currentScene_.tags.push_back(newTagName);
	else LogDebug("Tag already set");
}

void SceneManager::AddLayer(const std::string& newLayerName)
{
	if (!LayerExist(newLayerName)) currentScene_.layers.push_back(newLayerName);
	else LogDebug("Layer already set");
}

bool SceneManager::TagExist(const std::string& newTagName)
{
	const auto entityTagIt = std::find_if(currentScene_.tags.begin(),
		currentScene_.tags.end(),
		[newTagName](std::string tagName) { return newTagName == tagName; });
	if (entityTagIt == currentScene_.tags.end()) { return false; }
	return true;
}

bool SceneManager::LayerExist(const std::string& newLayerName)
{
	const auto entityLayerIt = std::find_if(currentScene_.layers.begin(),
		currentScene_.layers.end(),
		[newLayerName](std::string layerName) { return newLayerName == layerName; });
	if (entityLayerIt == currentScene_.layers.end()) { return false; }
	return true;
}

const std::vector<std::string> SceneManager::GetTags() const { return currentScene_.tags; }

const std::vector<std::string> SceneManager::GetLayers() const { return currentScene_.layers; }
}    // namespace neko::aer
