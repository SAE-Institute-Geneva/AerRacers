#pragma once
#include <fmod_studio.hpp>

namespace neko::fmod
{
static constexpr float kMaxVolume = 100.0f;
class AudioSource
{
public:
	enum State : std::uint8_t
	{
		STOPPED = 0,
		PLAYING,
		PAUSED
	};

	AudioSource() = default;
	AudioSource(std::string_view eventName, bool playOnWakeUp = false);

	void Play(const Vec3f& worldPos);
	void PlayOnce(const Vec3f& worldPos) const;

	void Pause();
	void Resume();
	void Stop();

	void Update3DAttributes(
		const Vec3f& position, const Vec3f& velocity, const Vec3f& forward, const Vec3f& up);

	[[nodiscard]] bool IsPlaying();
	[[nodiscard]] bool IsPaused();
	[[nodiscard]] bool DoPlayOnWakeUp() const { return playOnWakeUp_; }

	[[nodiscard]] std::string_view GetEventName() const { return eventName_; }
	[[nodiscard]] float GetVolume() const { return volume_; }
	[[nodiscard]] float GetPitch() const { return pitch_; }
	[[nodiscard]] float GetSpatialBlend() const { return spatialBlend_; }
	[[nodiscard]] float GetMinDistance() const { return minDistance_; }
	[[nodiscard]] float GetMaxDistance() const { return maxDistance_; }

	void SetEventName(std::string_view eventName) { eventName_ = eventName; }
	void SetPlayOnWakeUp(bool playOnWakeUp) { playOnWakeUp_ = playOnWakeUp; }
	void SetVolume(float volumeDb);
	void SetPitch(float pitch);
	void SetSpatialBlend(float spatialBlend);
	void SetMinDistance(float minDistance);
	void SetMaxDistance(float maxDistance);

private:
	friend class FmodEngine;
	std::string eventName_ {};
	FMOD::Studio::EventInstance* instance_ {};

	bool playOnWakeUp_ {};

	float volume_       = kMaxVolume;
	float pitch_        = 1.0f;
	float spatialBlend_ = 1.0f;
	float minDistance_  = 0.0f;
	float maxDistance_  = 500.0f;

	State state_ = State::STOPPED;
};
}
