#pragma once
#include "engine/transform.h"

#include "fmod/source.h"

namespace neko
{
/// Component manager for audio sources
class AudioManager
   : public ComponentManager<fmod::AudioSource, EntityMask(ComponentType::AUDIO_SOURCE)>,
	 public SystemInterface
{
public:
	using ComponentManager::ComponentManager;
	AudioManager(EntityManager& entityManager, Transform3dManager& transformManager);

	/// Checks for audio sources to play on wake up
	void Init() override;

	/// Updates the 3D attributes of all audio sources
	void Update(seconds dt) override;

	/// Stops all sounds
	void Destroy() override;

	/// Plays an FMOD event and registers the instance
	void Play(Entity e);

	/// Plays an FMOD event indepedently of the audio source
	void PlayOnce(Entity e) const;

	/// Pauses an audio source (must have been started with Play and not PlayOnce)
	void Pause(Entity e);

	/// Resumes a paused audio source (must have been started with Play and not PlayOnce)
	void Resume(Entity e);

	/// Stops an audio source (must have been started with Play and not PlayOnce)
	void Stop(Entity e);

	/// Updates a 3D event attached to a moving object
	void Update3DAttributes(Entity e, const FMOD_3D_ATTRIBUTES& attributes);

	/// Updates a 3D event attached to a moving object
	void Update3DAttributes(Entity e,
		const Vec3f& position,
		const Vec3f& velocity,
		const Vec3f& forward,
		const Vec3f& up);

	[[nodiscard]] bool IsPlaying(Entity e) { return components_[e].IsPlaying(); }
	[[nodiscard]] bool IsPaused(Entity e) { return components_[e].IsPaused(); }
	[[nodiscard]] bool DoPlayOnWakeUp(Entity e) const { return components_[e].DoPlayOnWakeUp(); }

	[[nodiscard]] FMOD_3D_ATTRIBUTES Get3DAttributes(Entity e) const { return components_[e].Get3DAttributes(); }
	[[nodiscard]] std::string_view GetEventName(Entity e) const { return components_[e].GetEventName(); }
	[[nodiscard]] float GetVolume(Entity e) const { return components_[e].GetVolume(); }
	[[nodiscard]] float GetPitch(Entity e) const { return components_[e].GetPitch(); }
	[[nodiscard]] float GetMinDistance(Entity e) const { return components_[e].GetMinDistance(); }
	[[nodiscard]] float GetMaxDistance(Entity e) const { return components_[e].GetMaxDistance(); }

	void SetEventName(Entity e, std::string_view eventName) { components_[e].SetEventName(eventName); }
	void SetPlayOnWakeUp(Entity e, bool playOnWakeUp) { components_[e].SetPlayOnWakeUp(playOnWakeUp); }
	void SetVolume(Entity e, float volume) { components_[e].SetVolume(volume); }
	void SetPitch(Entity e, float pitch) { components_[e].SetPitch(pitch); }
	void SetMinDistance(Entity e, float minDistance) { components_[e].SetMinDistance(minDistance); }
	void SetMaxDistance(Entity e, float maxDistance) { components_[e].SetMaxDistance(maxDistance); }

private:
	Transform3dManager& transformManager_;
};

class AudioViewer : public ComponentViewer
{
public:
	AudioViewer(EntityManager& entityManager, AudioManager& audioManager);

	void DrawImGui(Entity entity) override;

	[[nodiscard]] json GetJsonFromComponent(Entity entity) const override;
	void SetComponentFromJson(Entity entity, const json& component) override;

private:
	AudioManager& audioManager_;
};
}
