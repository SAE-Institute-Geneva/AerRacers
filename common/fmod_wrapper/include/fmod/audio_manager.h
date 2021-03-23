#pragma once
#include "engine/transform.h"

#include "fmod/source.h"

namespace neko
{
class AudioManager
   : public ComponentManager<fmod::AudioSource, EntityMask(ComponentType::AUDIO_SOURCE)>,
	 public SystemInterface
{
public:
	using ComponentManager::ComponentManager;
	AudioManager(EntityManager& entityManager, Transform3dManager& transformManager);

	void Init() override;
	void Update(seconds) override {}
	void Destroy() override;

	void SetComponent(Entity entity, const fmod::AudioSource& component) override;

	void Play(Entity e);
	void PlayOnce(Entity e) const;

	void Pause(Entity e);
	void Resume(Entity e);
	void Stop(Entity e);

	void Update3DAttributes(Entity e,
		const Vec3f& position,
		const Vec3f& velocity,
		const Vec3f& forward,
		const Vec3f& up)
	{
		components_[e].Update3DAttributes(position, velocity, forward, up);
	}

	[[nodiscard]] bool IsPlaying(Entity e) { return components_[e].IsPlaying(); }
	[[nodiscard]] bool IsPaused(Entity e) { return components_[e].IsPaused(); }
	[[nodiscard]] bool DoPlayOnWakeUp(Entity e) const { return components_[e].DoPlayOnWakeUp(); }

	[[nodiscard]] std::string_view GetEventName(Entity e) const { return components_[e].GetEventName(); }
	[[nodiscard]] float GetVolume(Entity e) const { return components_[e].GetVolume(); }
	[[nodiscard]] float GetPitch(Entity e) const { return components_[e].GetPitch(); }
	[[nodiscard]] float GetSpatialBlend(Entity e) const { return components_[e].GetSpatialBlend(); }
	[[nodiscard]] float GetMinDistance(Entity e) const { return components_[e].GetMinDistance(); }
	[[nodiscard]] float GetMaxDistance(Entity e) const { return components_[e].GetMaxDistance(); }

	void SetEventName(Entity e, std::string_view eventName) { components_[e].SetEventName(eventName); }
	void SetPlayOnWakeUp(Entity e, bool playOnWakeUp) { components_[e].SetPlayOnWakeUp(playOnWakeUp); }
	void SetVolume(Entity e, float volume) { components_[e].SetVolume(volume); }
	void SetPitch(Entity e, float pitch) { components_[e].SetPitch(pitch); }
	void SetSpatialBlend(Entity e, float spatialBlend) { components_[e].SetSpatialBlend(spatialBlend); }
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
