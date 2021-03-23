#pragma once
#include <map>

#include "engine/assert.h"
#include "engine/engine.h"
#include "utils/service_locator.h"

#include "fmod/source.h"

namespace neko::fmod
{
using BankMap = std::map<std::string, FMOD::Studio::Bank*>;

/// The number of sounds that can play simultaneously
constexpr std::uint8_t kMaxInstance = 5u;
constexpr std::uint8_t kMaxChannels = 32u;
constexpr const char* kEventPrefix  = "event:/";

/// A wrapper around FMOD objects
class IFmodEngine
{
public:
	// ---------- Audio Playing
	virtual FMOD::Studio::EventInstance* PlayEvent(const AudioSource&, const Vec3f&) = 0;
	virtual void PlayEventOnce(const AudioSource&, const Vec3f&)                     = 0;

	virtual void PauseEvent(FMOD::Studio::EventInstance*)      = 0;
	virtual void ResumeEvent(FMOD::Studio::EventInstance*)     = 0;
	virtual void StopEvent(FMOD::Studio::EventInstance*, bool) = 0;
	// ----------

	// ---------- Sound Parameters
	[[nodiscard]] virtual const std::vector<std::string>& GetEventNames() const = 0;

	[[nodiscard]] virtual float GetEventParameter(std::string_view, std::string_view) = 0;
	virtual void SetEventParameter(std::string_view, std::string_view, float)         = 0;

	virtual void SetEventVolume(FMOD::Studio::EventInstance*, float) = 0;
	virtual void SetEventPitch(FMOD::Studio::EventInstance*, float)  = 0;
	virtual void SetEvent3DAttributes(
		FMOD::Studio::EventInstance*, const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&) = 0;

	virtual void SetEventSpatialBlend(FMOD::Studio::EventInstance*, float) = 0;
	virtual void SetEventMinDistance(FMOD::Studio::EventInstance*, float) = 0;
	virtual void SetEventMaxDistance(FMOD::Studio::EventInstance*, float) = 0;

	[[nodiscard]] virtual bool IsPlaying(const FMOD::Studio::EventInstance*) const = 0;
	[[nodiscard]] virtual bool IsPaused(const FMOD::Studio::EventInstance*) const = 0;

	virtual void SetAudioListener(const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&) = 0;
	// ----------

	// ---------- Utilities
	static float DecibelsToVolume(float db) { return powf(10.0f, 0.05f * db); }
	static float VolumeToDecibels(float volume) { return 20.0f * log10f(volume); }
	static FMOD_VECTOR Vec3ToFmod(const Vec3f& vec3)
	{
		FMOD_VECTOR fmodVector;
		fmodVector.x = vec3.x;
		fmodVector.y = vec3.y;
		fmodVector.z = vec3.z;
		return fmodVector;
	}
	// ----------

	virtual const std::vector<std::string>& GetAllEventNames() = 0;
};

class NullFmodEngine : public IFmodEngine
{
public:
	// ---------- Audio Playing
	FMOD::Studio::EventInstance* PlayEvent(const AudioSource&, const Vec3f&) override
	{ return nullptr; }

	void PlayEventOnce(const AudioSource&, const Vec3f&) override {}

	void PauseEvent(FMOD::Studio::EventInstance*) override {}
	void ResumeEvent(FMOD::Studio::EventInstance*) override {}
	void StopEvent(FMOD::Studio::EventInstance*, bool) override {}
	// ----------

	// ---------- Sound Parameters
	[[nodiscard]] const std::vector<std::string>& GetEventNames() const override
	{
		neko_assert(false, "FMOD Engine is null!");
	}

	float GetEventParameter(std::string_view, std::string_view) override { return 0.0f; }
	void SetEventParameter(std::string_view, std::string_view, float) override {}

	void SetEventVolume(FMOD::Studio::EventInstance*, float) override {}
	void SetEventPitch(FMOD::Studio::EventInstance*, float) override {}
	void SetEvent3DAttributes(FMOD::Studio::EventInstance*,
		const Vec3f&,
		const Vec3f&,
		const Vec3f&,
		const Vec3f&) override
	{}

	void SetEventSpatialBlend(FMOD::Studio::EventInstance*, float) override {}
	void SetEventMinDistance(FMOD::Studio::EventInstance*, float) override {}
	void SetEventMaxDistance(FMOD::Studio::EventInstance*, float) override {}

	[[nodiscard]] bool IsPlaying(const FMOD::Studio::EventInstance*) const override
	{ return false; }
	[[nodiscard]] bool IsPaused(const FMOD::Studio::EventInstance*) const override
	{ return false; }

	void SetAudioListener(const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&) override {}
	// ----------

	const std::vector<std::string>& GetAllEventNames() override
	{
		neko_assert(false, "FMOD Engine is null!");
	}
};

class FmodEngine : public IFmodEngine, public SystemInterface
{
public:
	FmodEngine();

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

	// ---------- Audio Playing
	FMOD::Studio::EventInstance* PlayEvent(
		const AudioSource& audioSource, const Vec3f& pos) override;
	void PlayEventOnce(const AudioSource& audioSource, const Vec3f& pos) override;

	void PauseEvent(FMOD::Studio::EventInstance* instance) override;
	void ResumeEvent(FMOD::Studio::EventInstance* instance) override;
	void StopEvent(FMOD::Studio::EventInstance* instance, bool isStoppingImmediate) override;
	// ----------

	// ---------- Sound Parameters
	[[nodiscard]] const std::vector<std::string>& GetEventNames() const override { return eventNames_; }

	float GetEventParameter(
		std::string_view eventName, std::string_view eventParameterName) override;
	void SetEventParameter(std::string_view eventName,
		std::string_view eventParameterName,
		float parameterValue) override;

	void SetEventVolume(FMOD::Studio::EventInstance* instance, float volumeDb) override;
	void SetEventPitch(FMOD::Studio::EventInstance* instance, float pitch) override;
	void SetEvent3DAttributes(FMOD::Studio::EventInstance* instance,
		const Vec3f& position,
		const Vec3f& velocity,
		const Vec3f& forward,
		const Vec3f& up) override;

	void SetEventSpatialBlend(FMOD::Studio::EventInstance* instance, float spatialBlend) override;
	void SetEventMinDistance(FMOD::Studio::EventInstance* instance, float minDistance) override;
	void SetEventMaxDistance(FMOD::Studio::EventInstance* instance, float maxDistance) override;

	[[nodiscard]] bool IsPlaying(const FMOD::Studio::EventInstance* instance) const override;
	[[nodiscard]] bool IsPaused(const FMOD::Studio::EventInstance* instance) const override;

	void SetAudioListener(const Vec3f& position,
		const Vec3f& velocity,
		const Vec3f& forward,
		const Vec3f& up) override;
	// ----------

	const std::vector<std::string>& GetAllEventNames() override { return eventNames_; }

private:
	// ---------- Resources loading
	void LoadBank(std::string_view bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEventNames();
	// ----------

	static FMOD_RESULT F_CALLBACK EventInstanceStopped(FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
		FMOD_STUDIO_EVENTINSTANCE* instance,
		void* parameters);

	FMOD::System* system_ {};
	FMOD::Studio::System* studioSystem_ {};

	BankMap banks_ {};
	std::vector<std::string> eventNames_ {};
};

using FmodLocator = Locator<IFmodEngine, NullFmodEngine>;
}
