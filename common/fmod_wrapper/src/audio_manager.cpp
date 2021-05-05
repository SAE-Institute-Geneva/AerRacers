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
		if (components_[e].GetEventName().empty()) continue;
		const Vec3f position = transformManager_.GetGlobalPosition(e);
		if (components_[e].DoPlayOnWakeUp() && !components_[e].IsPlaying()) components_[e].Play(position);
	}
}

void AudioManager::Update(seconds)
{
	const auto audioSources =
		entityManager_.get().FilterEntities(EntityMask(ComponentType::AUDIO_SOURCE));
	if (audioSources.empty()) return;
	ResizeIfNecessary(components_, audioSources.back(), {});
	for (auto& entity : audioSources)
	{
		if (!IsPlaying(entity)) continue;

		const Vec3f position               = transformManager_.GetGlobalPosition(entity);
		const fmod::AudioSource& component = components_[entity];
		FMOD_3D_ATTRIBUTES attributes      = component.Get3DAttributes();

		attributes.position = fmod::Vec3ToFmod(position);
		Update3DAttributes(entity, attributes);
	}
}

void AudioManager::Destroy()
{
	for (auto& component : components_) component.Stop();
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

void AudioManager::Update3DAttributes(Entity e, const FMOD_3D_ATTRIBUTES& attributes)
{
	components_[e].Update3DAttributes(attributes);
}

void AudioManager::Update3DAttributes(
	Entity e, const Vec3f& position, const Vec3f& velocity, const Vec3f& forward, const Vec3f& up)
{
	components_[e].Update3DAttributes(position, velocity, forward, up);
}

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
			const ImVec2 buttonSize =
				ImVec2((GetContentRegionAvailWidth() - GetStyle().ItemSpacing.x) / 2.0f , 0.0f);

			PushItemWidth(-1);
			if (!audioManager_.IsPlaying(entity))
			{
				if (Button("Play", buttonSize)) audioManager_.Play(entity);
			}
			else
			{
				if (!audioManager_.IsPaused(entity))
				{
					if (Button("Pause", buttonSize)) audioManager_.Pause(entity);
				}
				else
				{
					if (Button("Resume", buttonSize)) audioManager_.Resume(entity);
				}
			}
			SameLine();
			if (Button("Stop", buttonSize)) audioManager_.Stop(entity);
			PopItemWidth();

			fmod::AudioSource audioSource = audioManager_.GetComponent(entity);
			auto& fmodEngine              = fmod::FmodLocator::get();
			const auto eventName          = audioManager_.GetEventName(entity);
			const auto& eventNames        = fmodEngine.GetEventNames();
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

			float minDistance = audioSource.GetMinDistance();
			if (DragFloat("Min Distance", &minDistance, LabelPos::LEFT, 0.1f, 0.0f, FLT_MAX))
				audioSource.SetMinDistance(minDistance);

			float maxDistance = audioSource.GetMaxDistance();
			if (DragFloat("Max Distance", &maxDistance, LabelPos::LEFT, 0.1f, 0.0f, FLT_MAX))
				audioSource.SetMaxDistance(maxDistance);

			audioManager_.SetComponent(entity, audioSource);

			const auto description = fmod::FmodLocator::get().GetEventDescription(eventName);
			if (TreeNode("Event Parameters"))
			{
				int parameterCount;
				description->getParameterDescriptionCount(&parameterCount);
				for (int i = 0; i < parameterCount; ++i)
				{
					FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
					description->getParameterDescriptionByIndex(i, &parameter);

					float parameterVal = audioManager_.GetParameter(entity, parameter.id);
					if (DragFloat(parameter.name,
							&parameterVal,
							LabelPos::LEFT,
							0.01f,
							parameter.minimum,
							parameter.maximum))
					{
						audioManager_.SetParameter(entity, parameter.id, parameterVal);
					}
				}
				TreePop();
			}
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
	audioManager_.SetMinDistance(entity, component["minDistance"].get<float>());
	audioManager_.SetMaxDistance(entity, component["maxDistance"].get<float>());
}
}    // namespace neko
