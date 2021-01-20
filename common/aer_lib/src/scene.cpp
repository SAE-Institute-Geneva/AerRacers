#include "aer/scene.h"

#include <fmt/format.h>

#include "aer/log.h"
#include "aer/tag.h"
#include "engine/configuration.h"
#include "engine/engine.h"
#include "utils/file_utility.h"

namespace neko::aer
{
static const std::string_view SCENE_EXTENSION = ".scene";

SceneManager::SceneManager(EntityManager& entityManager, Transform3dManager& transform3dManager)
   : filesystem_(BasicEngine::GetInstance()->GetFilesystem()),
	 entityManager_(entityManager),
	 transformManager_(transform3dManager),
	 tagManager_(*this)
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
				transformManager_.AddComponent(entity);
				transformManager_.SetComponentFromJson(entity, componentJson["transform"]);
			}
		}
	}

	if (CheckJsonParameter(componentJson, "rigidbody", json::value_t::object))
	{
		if (CheckJsonParameter(componentJson["rigidbody"], "exist", json::value_t::boolean))
		{
			if (componentJson["rigidbody"]["exist"])
			{
				//transformManager_.AddComponent(entity);
				//transformManager_.SetComponentFromJson(entity, componentJson["transform"]);
			}
		}
	}

	if (CheckJsonParameter(componentJson, "boxCollider", json::value_t::object))
	{
		if (CheckJsonParameter(componentJson["boxCollider"], "exist", json::value_t::boolean))
		{
			if (componentJson["boxCollider"]["exist"])
			{
				//transformManager_.AddComponent(entity);
				//transformManager_.SetComponentFromJson(entity, componentJson["transform"]);
			}
		}
	}

	if (CheckJsonParameter(componentJson, "sphereCollider", json::value_t::object))
	{
		if (CheckJsonParameter(componentJson["sphereCollider"], "exist", json::value_t::boolean))
		{
			if (componentJson["sphereCollider"]["exist"])
			{
				//transformManager_.AddComponent(entity);
				//transformManager_.SetComponentFromJson(entity, componentJson["transform"]);
			}
		}
	}

	if (CheckJsonParameter(componentJson, "shipControl", json::value_t::object))
	{
		if (CheckJsonParameter(componentJson["shipControl"], "exist", json::value_t::boolean))
		{
			if (componentJson["shipControl"]["exist"])
			{
				//transformManager_.AddComponent(entity);
				//transformManager_.SetComponentFromJson(entity, componentJson["transform"]);
			}
		}
	}
}

void SceneManager::ParseEntityJson(const json& entityJson)
{
	Entity entity = entityManager_.CreateEntity();
	if (CheckJsonParameter(entityJson, "name", json::value_t::string))
	{
		//entityManager_.SetEntityName(entity, entityJson["name"]); TODO(@Luca) Set when name is done
	}

	if (CheckJsonParameter(entityJson, "instanceId", json::value_t::number_integer))
	{
		InstanceId instanceId = entityJson["instanceId"];
		ResizeIfNecessary(entityInstanceIdArray_, entity, INVALID_INSTANCE_ID);
		entityInstanceIdArray_[entity] = instanceId;
	}

	if (CheckJsonParameter(entityJson, "parent", json::value_t::number_integer))
	{
		int parentInstanceId = entityJson["parent"];
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

	if (CheckJsonParameter(entityJson, "layer", json::value_t::string))
	{
		TagLocator::get().SetEntityLayer(entity, static_cast<std::string>(entityJson["layer"]));
	}

	if (CheckJsonParameter(entityJson, "tag", json::value_t::string))
	{
		TagLocator::get().SetEntityTag(entity, static_cast<std::string>(entityJson["tag"]));
	}

	if (CheckJsonParameter(entityJson, "isActive", json::value_t::boolean))
	{
		//TODO (@Luca) Set active
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
            if (tag != INVALID_TAG)
            {
                currentScene_.tags.push_back(tag);
            }
        }
    }

    if (CheckJsonParameter(sceneJson, "layers", json::value_t::array))
    {
        for (auto& layer : sceneJson["layers"])
        {
            if (layer != INVALID_LAYER)
            {
                currentScene_.layers.push_back(layer);
            }
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
		json scene              = neko::LoadJson(jsonPath);
		currentScene_.scenePath = jsonPath;
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
	entityJson["transform"] = transformManager_.GetJsonFromComponent(entity);
	entityJson["transform"]["exist"] =
		entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	//entityJson["rigidbody"] = json::object();
	//entityJson["rigidbody"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["rigidbody"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	//entityJson["boxCollider"] = json::object();
	//entityJson["boxCollider"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["boxCollider"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	//entityJson["sphereCollider"] = json::object();
	//entityJson["sphereCollider"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["sphereCollider"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	//entityJson["shipControl"] = json::object();
	//entityJson["shipControl"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["shipControl"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	//entityJson["shipRotation"] = json::object();
	//entityJson["shipRotation"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["shipRotation"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
	//entityJson["modelRenderer"] = json::object();
	//entityJson["modelRenderer"] = transformManager_.GetJsonFromComponent(entity);
	//entityJson["modelRenderer"]["exist"] = entityManager_.HasComponent(entity, EntityMask(ComponentType::TRANSFORM3D));
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
	const auto entityTagIt = std::find_if(currentScene_.tags.begin(),
		currentScene_.tags.end(),
		[newTagName](std::string tagName) { return newTagName == tagName; });
	if (entityTagIt == currentScene_.tags.end()) { currentScene_.tags.push_back(newTagName); }
	else
	{
		LogDebug("Tag already set");
	}
}

void SceneManager::AddLayer(const std::string& newLayerName)
{
	const auto entityLayerIt = std::find_if(currentScene_.layers.begin(),
		currentScene_.layers.end(),
		[newLayerName](std::string layerName) { return newLayerName == layerName; });
	if (entityLayerIt == currentScene_.layers.end())
	{
		currentScene_.layers.push_back(newLayerName);
	}
	else
	{
		LogDebug("Layer already set");
	}
}

const std::vector<std::string>& SceneManager::GetTags() const { return currentScene_.tags; }

const std::vector<std::string>& SceneManager::GetLayers() const { return currentScene_.layers; }
}    // namespace neko::aer
