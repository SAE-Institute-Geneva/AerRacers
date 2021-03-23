#include "fmod/audio_manager.h"

#include "utils/imgui_utility.h"

#include "fmod/fmod_engine.h"

namespace neko
{
AudioManager::AudioManager(EntityManager& entityManager, Transform3dManager& transformManager)
   : ComponentManager<fmod::AudioSource, EntityMask(ComponentType::AUDIO_SOURCE)>(entityManager),
	 transformManager_(transformManager)
{}

void AudioManager::Init()
{
	for (std::size_t e = 0; e < components_.size(); ++e)
	{
		const Vec3f position = transformManager_.GetGlobalPosition(e);
		if (components_[e].DoPlayOnWakeUp()) components_[e].Play(position);
	}
}

void AudioManager::Destroy()
{
	for (auto& component : components_) component.Stop();
}

void AudioManager::SetComponent(Entity entity, const fmod::AudioSource& component)
{
	ComponentManager::SetComponent(entity, component);
}

void AudioManager::Play(Entity e)
{
	const Vec3f position = transformManager_.GetGlobalPosition(e);
	components_[e].Play(position);
}

void AudioManager::PlayOnce(Entity e) const
{
	const Vec3f position = transformManager_.GetGlobalPosition(e);
	components_[e].PlayOnce(position);
}

void AudioManager::Pause(Entity e) { components_[e].Pause(); }

void AudioManager::Resume(Entity e) { components_[e].Resume(); }

void AudioManager::Stop(Entity e) { components_[e].Stop(); }

AudioViewer::AudioViewer(EntityManager& entityManager, AudioManager& audioManager)
   : ComponentViewer(entityManager), audioManager_(audioManager)
{}

void AudioViewer::DrawImGui(Entity entity)
{
	if (entity == INVALID_ENTITY) return;
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::AUDIO_SOURCE)))
	{
		using namespace ImGui;
		if (TreeNode("Audio Source"))
		{
			PushItemWidth(-1);
			if (!audioManager_.IsPlaying(entity))
			{
				if (Button("Play")) audioManager_.Play(entity);
			}
			else
			{
				if (!audioManager_.IsPaused(entity))
				{
					if (Button("Pause")) audioManager_.Pause(entity);
				}
				else
				{
					if (Button("Resume")) audioManager_.Resume(entity);
				}
			}
			SameLine();
			if (Button("Stop")) audioManager_.Stop(entity);
			PopItemWidth();

			fmod::AudioSource audioSource = audioManager_.GetComponent(entity);
			const auto eventName          = audioManager_.GetEventName(entity);
			const auto eventNames         = fmod::FmodLocator::get().GetEventNames();
			unsigned index =
				std::find(eventNames.cbegin(), eventNames.cend(), eventName) - eventNames.cbegin();
			if (Combo("Event", &index, &eventNames, LabelPos::LEFT))
				audioSource.SetEventName(eventNames[index]);

			bool playOnWakeUp = audioSource.DoPlayOnWakeUp();
			if (Checkbox("Play On Wake Up?", &playOnWakeUp, LabelPos::LEFT))
				audioSource.SetPlayOnWakeUp(playOnWakeUp);

			float volume = audioSource.GetVolume();
			if (SliderFloat("Volume", &volume, 0.0f, fmod::kMaxVolume, LabelPos::LEFT))
				audioSource.SetVolume(volume);

			float pitch = audioSource.GetPitch();
			if (SliderFloat("Pitch", &pitch, 0.1f, 2.0f, LabelPos::LEFT))
				audioSource.SetPitch(pitch);

			float spatialBlend = audioSource.GetSpatialBlend();
			if (SliderFloat("Spatial Blend", &spatialBlend, 0.0f, 1.0f, LabelPos::LEFT))
				audioSource.SetSpatialBlend(spatialBlend);

			float minDistance = audioSource.GetMinDistance();
			if (DragFloat("Min Distance", &minDistance, LabelPos::LEFT, 0.1f, 0.0f, FLT_MAX))
				audioSource.SetMinDistance(minDistance);

			float maxDistance = audioSource.GetMaxDistance();
			if (DragFloat("Max Distance", &maxDistance, LabelPos::LEFT, 0.1f, 0.0f, FLT_MAX))
				audioSource.SetMaxDistance(maxDistance);

			audioManager_.SetComponent(entity, audioSource);
			TreePop();
		}
	}
}

json AudioViewer::GetJsonFromComponent(Entity entity) const
{
	json component            = json::object();
	component["eventName"]    = audioManager_.GetEventName(entity);
	component["playOnWakeUp"] = audioManager_.DoPlayOnWakeUp(entity);

	component["volume"]       = audioManager_.GetVolume(entity);
	component["pitch"]        = audioManager_.GetPitch(entity);
	component["spatialBlend"] = audioManager_.GetSpatialBlend(entity);
	component["minDistance"]  = audioManager_.GetMinDistance(entity);
	component["maxDistance"]  = audioManager_.GetMaxDistance(entity);

	return component;
}

void AudioViewer::SetComponentFromJson(Entity entity, const json& component)
{
	audioManager_.SetEventName(entity, component["eventName"].get<std::string_view>());
	audioManager_.SetPlayOnWakeUp(entity, component["playOnWakeUp"].get<bool>());

	audioManager_.SetVolume(entity, component["volume"].get<float>());
	audioManager_.SetPitch(entity, component["pitch"].get<float>());
	audioManager_.SetSpatialBlend(entity, component["spatialBlend"].get<float>());
	audioManager_.SetMinDistance(entity, component["minDistance"].get<float>());
	audioManager_.SetMaxDistance(entity, component["maxDistance"].get<float>());
}
}    // namespace neko
