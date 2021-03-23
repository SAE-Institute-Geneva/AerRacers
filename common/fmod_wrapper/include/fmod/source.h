#pragma once
#include <fmod_studio.hpp>

namespace neko::fmod
{
static constexpr float kMaxVolume = 100.0f;

/// An object that emits sound
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

	/// Plays then event and registers the instance
	/// \param attributes: the position at which to start the event
	void Play(const Vec3f& worldPos);

	/// Plays the event indepedently of the audio source
	/// \param attributes: the position at which to start the event
	void PlayOnce(const Vec3f& worldPos) const;

	/// Pauses the event (must have been started with Play and not PlayOnce)
	void Pause();

	/// Resumes the paused event (must have been started with Play and not PlayOnce)
	void Resume();

	/// Stops the event (must have been started with Play and not PlayOnce)
	void Stop();

	/// Updates the 3D attributes of the event
	/// \param attributes: the 3D attributes to copy
	void Update3DAttributes(const FMOD_3D_ATTRIBUTES& attributes);

	/// Updates the 3D attributes of the event
	/// \param position: the position of the audio source
	/// \param velocity: the velocity of the audio source
	/// \param forward: the direction the audio source is facing
	/// \param up: the up vector of the audio source, must be orthogonal to the forward
	void Update3DAttributes(
		const Vec3f& position, const Vec3f& velocity, const Vec3f& forward, const Vec3f& up);

	[[nodiscard]] bool IsPlaying();
	[[nodiscard]] bool IsPaused();
	[[nodiscard]] bool DoPlayOnWakeUp() const { return playOnWakeUp_; }

	[[nodiscard]] FMOD_3D_ATTRIBUTES Get3DAttributes() const;
	[[nodiscard]] std::string_view GetEventName() const { return eventName_; }
	[[nodiscard]] float GetVolume() const { return volume_; }
	[[nodiscard]] float GetPitch() const { return pitch_; }
	[[nodiscard]] float GetMinDistance() const { return minDistance_; }
	[[nodiscard]] float GetMaxDistance() const { return maxDistance_; }

	void SetEventName(std::string_view eventName) { eventName_ = eventName; }
	void SetPlayOnWakeUp(bool playOnWakeUp) { playOnWakeUp_ = playOnWakeUp; }
	void SetVolume(float volumeDb);
	void SetPitch(float pitch);
	void SetMinDistance(float minDistance);
	void SetMaxDistance(float maxDistance);

private:
	friend class FmodEngine;

	/// The name of the FMOD event (without the "event:/" prefix)
	std::string eventName_ {};

	/// The instance of event
	FMOD::Studio::EventInstance* instance_ {};

	/// Whether to play the event when the audio source is loaded
	bool playOnWakeUp_ {};

	/// The volume of the sound in percentage
	float volume_ = kMaxVolume;

	/// Pitch in FMOD isn't the actual pitch, but rather the speed at which to play the sound
	float pitch_ = 1.0f;

	/// The minimum distance at which to hear the sound
	float minDistance_ = 0.0f;

	/// The maximum distance at which to hear the sound
	float maxDistance_ = 500.0f;

	State state_ = State::STOPPED;
};
}
