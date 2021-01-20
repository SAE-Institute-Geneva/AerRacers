#include <tag.h>

#include <engine/log.h>
#include <scene.h>

namespace neko::aer
{
TagManager::TagManager(SceneManager& sceneManager) : sceneManager_(sceneManager) {}

TagManager::~TagManager() {}

std::string TagManager::GetEntityTag(Entity entity) const
{
	if (entityTagArray_.empty())
	{
		logDebug("No Tags");
		return INVALID_TAG;
	}

	auto tags         = sceneManager_.GetCurrentScene().tags;
	TagIndex tagIndex = GetEntityTagIndex(entity);
	if (tagIndex >= entityTagArray_.size())
	{
		logDebug("Tag invalid");
		return INVALID_TAG;
	}

	return tags[tagIndex];
}

TagIndex TagManager::GetEntityTagIndex(Entity entity) const
{
	if (entityTagArray_.empty())
	{
		logDebug("No Tags");
		return 0;
	}

	if (entity == INVALID_ENTITY || entity >= entityTagArray_.size())
	{
		logDebug("Entity invalid");
		return 0;
	}

	return entityTagArray_[entity];
}

void TagManager::SetEntityTag(Entity entity, const std::string& entityTag)
{
	auto tags              = sceneManager_.GetCurrentScene().tags;
	const auto entityTagIt = std::find_if(tags.begin(),
		tags.end(),
		[entityTag](std::string tagName) { return tagName == entityTag; });
	if (entityTagIt == tags.end()) { SetEntityTag(entity, 0); }
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
	}
}

bool TagManager::IsEntityTag(Entity entity, const std::string& entityTag) const
{
	auto tags              = sceneManager_.GetCurrentScene().tags;
	const auto entityTagIt = std::find_if(tags.begin(),
		tags.end(),
		[entityTag](std::string tagName) { return tagName == entityTag; });
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
	if (entity == INVALID_ENTITY || entity >= entityTagArray_.size())
	{
		logDebug("Entity invalid");
		return false;
	}
	return entityTagArray_[entity] == entityTagIndex;
}

bool TagManager::CompareEntitiesTag(Entity entityA, Entity entityB) const
{
	if (entityA == INVALID_ENTITY || entityA >= entityTagArray_.size() ||
		entityB == INVALID_ENTITY || entityB >= entityTagArray_.size())
	{
		logDebug("Entity invalid");
		return false;
	}
	return entityTagArray_[entityA] == entityTagArray_[entityB];
}

std::string TagManager::GetEntityLayer(Entity entity) const
{
	if (entityLayerArray_.empty())
	{
		logDebug("No Layers");
		return INVALID_LAYER;
	}

	auto layers           = sceneManager_.GetCurrentScene().layers;
	LayerIndex layerIndex = GetEntityLayerIndex(entity);
	if (layerIndex >= entityLayerArray_.size())
	{
		logDebug("Layer invalid");
		return INVALID_LAYER;
	}

	return layers[layerIndex];
}

LayerIndex TagManager::GetEntityLayerIndex(Entity entity) const
{
	if (entityLayerArray_.empty())
	{
		logDebug("No Layers");
		return 0;
	}

	if (entity == INVALID_ENTITY || entity >= entityLayerArray_.size())
	{
		logDebug("Entity invalid");
		return 0;
	}

	return entityLayerArray_[entity];
}

void TagManager::SetEntityLayer(Entity entity, const std::string& entityLayer)
{
	auto layers              = sceneManager_.GetCurrentScene().layers;
	const auto entityLayerIt = std::find_if(layers.begin(),
		layers.end(),
		[entityLayer](std::string layerName) { return layerName == entityLayer; });
	if (entityLayerIt == layers.end()) { SetEntityLayer(entity, 0); }
	else
	{
		const TagIndex entityTagIndex = entityLayerIt - layers.begin();
		SetEntityLayer(entity, entityTagIndex);
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
	}
}

bool TagManager::IsEntityLayer(Entity entity, const std::string& entityLayer) const
{
	auto layers              = sceneManager_.GetCurrentScene().layers;
	const auto entityLayerIt = std::find_if(layers.begin(),
		layers.end(),
		[entityLayer](std::string layerName) { return layerName == entityLayer; });
	if (entityLayerIt == layers.end())
	{
		logDebug("Layer " + entityLayer + " not in scene");
		return false;
	}
	else
	{
		const TagIndex entityTagIndex = entityLayerIt - layers.begin();
		return IsEntityLayer(entity, entityTagIndex);
	}
}

bool TagManager::IsEntityLayer(Entity entity, LayerIndex entityLayerIndex) const
{
	if (entity == INVALID_ENTITY || entity >= entityLayerArray_.size())
	{
		logDebug("Entity invalid");
		return false;
	}
	return entityLayerArray_[entity] == entityLayerIndex;
}
}
