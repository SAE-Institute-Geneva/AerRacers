#include "fmod/fmod_engine.h"

namespace neko::fmod
{
AudioSource::AudioSource(std::string_view eventName, bool playOnWakeUp)
   : eventName_(eventName), playOnWakeUp_(playOnWakeUp)
{}

void AudioSource::Play(const Vec3f& worldPos)
{
	if (!eventName_.empty())
	{
		if (instance_) FmodLocator::get().PlayEvent(*this, worldPos);
		else instance_ = FmodLocator::get().PlayEvent(*this, worldPos);
	}

	state_ = State::PLAYING;
}

void AudioSource::PlayOnce(const Vec3f& worldPos) const
{
	FmodLocator::get().PlayEventOnce(*this, worldPos);
}

void AudioSource::Pause()
{
	if (instance_) FmodLocator::get().PauseEvent(instance_);
	state_ = State::PAUSED;
}

void AudioSource::Resume()
{
	if (instance_) FmodLocator::get().ResumeEvent(instance_);
	state_ = State::PLAYING;
}

void AudioSource::Stop()
{
	if (instance_)
	{
		FmodLocator::get().StopEvent(instance_, true);
		instance_ = nullptr;
	}

	state_ = State::STOPPED;
}

bool AudioSource::IsPlaying()
{
	bool isPlaying = false;
	if (state_ == State::PAUSED) return isPlaying;
	if (instance_) isPlaying = FmodLocator::get().IsPlaying(instance_);

	state_ = isPlaying ? State::PLAYING : State::STOPPED;
	return isPlaying;
}

bool AudioSource::IsPaused()
{
	bool isPaused = false;
	if (instance_) isPaused = FmodLocator::get().IsPaused(instance_);

	state_ = isPaused ? State::PAUSED : State::PLAYING;
	return isPaused;
}

void AudioSource::Update3DAttributes(
	const Vec3f& position, const Vec3f& velocity, const Vec3f& forward, const Vec3f& up)
{
	if (state_ != State::PLAYING) return;
	if (instance_)
	{
		FmodLocator::get().SetEvent3DAttributes(instance_, position, velocity, forward, up);
	}
}

void AudioSource::SetVolume(float volumeDb)
{
	volume_ = volumeDb;
	if (state_ != State::PLAYING) return;
	if (instance_) FmodLocator::get().SetEventVolume(instance_, volume_);
}

void AudioSource::SetPitch(float pitch)
{
	pitch_ = pitch;
	if (state_ != State::PLAYING) return;
	if (instance_) FmodLocator::get().SetEventPitch(instance_, pitch_);
}

void AudioSource::SetSpatialBlend(float spatialBlend)
{
	spatialBlend_ = spatialBlend;
	if (instance_) FmodLocator::get().SetEventSpatialBlend(instance_, spatialBlend);
}

void AudioSource::SetMinDistance(float minDistance)
{
	minDistance_ = minDistance;
	if (instance_) FmodLocator::get().SetEventMinDistance(instance_, minDistance_);
}

void AudioSource::SetMaxDistance(float maxDistance)
{
	maxDistance_ = maxDistance;
	if (instance_) FmodLocator::get().SetEventMaxDistance(instance_, maxDistance_);
}
}    // namespace neko::fmod