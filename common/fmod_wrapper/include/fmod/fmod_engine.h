#pragma once
#include <map>

#include "engine/assert.h"
#include "engine/engine.h"
#include "utils/service_locator.h"

#include "fmod/source.h"

namespace neko::fmod
{
using BankMap      = std::map<std::string, FMOD::Studio::Bank*>;
using FmodInstance = FMOD::Studio::EventInstance;

/// The number of sounds that can play simultaneously
constexpr std::uint8_t kMaxInstance = 5u;
constexpr std::uint8_t kMaxChannels = 32u;
constexpr const char* kEventPrefix  = "event:/";

// ---------- Utilities
inline float DecibelsToVolume(float db) { return powf(10.0f, 0.05f * db); }
inline float VolumeToDecibels(float volume) { return 20.0f * log10f(volume); }
inline FMOD_VECTOR Vec3ToFmod(const Vec3f& vec3)
{
	FMOD_VECTOR fmodVector;
	fmodVector.x = vec3.x;
	fmodVector.y = vec3.z;
	fmodVector.z = vec3.y;
	return fmodVector;
}

inline Vec3f FmodToVec3(const FMOD_VECTOR& vec3)
{
	Vec3f vector;
	vector.x = vec3.x;
	vector.y = vec3.z;
	vector.z = vec3.y;
	return vector;
}
// ----------

/// A wrapper around FMOD objects
class IFmodEngine
{
public:
	// ---------- Audio Playing
	/// Plays an event at the provided audio source
	/// \param audioSource: the audio source to use to play the event
	/// \param worldPos: the initial position of the audio source
	virtual FmodInstance* PlayEvent(const AudioSource&, const Vec3f&) = 0;

	/// Plays an event at the provided audio source
	/// \param audioSource: the audio source to use to play the event
	/// \param worldPos: the initial position of the audio source
	virtual void PlayEventOnce(const AudioSource&, const Vec3f&)      = 0;

	/// Pauses a specific event
	/// \param instance: the instance of the event
	virtual void PauseEvent(FmodInstance*)      = 0;

	/// Resumes a paused event
	/// \param instance: the instance of the event
	virtual void ResumeEvent(FmodInstance*)     = 0;

	/// Stops an event
	/// \param instance: the instance of the event
	/// \param isStoppingImmediate: whether the stop the event immediatly or not
	virtual void StopEvent(FmodInstance*, bool) = 0;
	// ----------

	// ---------- Sound Parameters
	/// \return All the events loaded from the FMOD banks
	[[nodiscard]] virtual const std::vector<std::string>& GetEventNames() const = 0;

	/// \return The value of the event custom parameter
	/// \param eventName: the name of the event
	/// \param eventParameterName: the name of the parameter
	[[nodiscard]] virtual float GetEventParameter(std::string_view, std::string_view) = 0;

	/// \return The 3D attributes of the specified event instance
	/// \param instance: the instance of the event
	virtual FMOD_3D_ATTRIBUTES GetEvent3DAttributes(FmodInstance*) const = 0;

	/// \param eventName: the name of the event
	/// \param eventParameterName: the name of the parameter
	/// \param parameterValue: the value to give the parameter
	virtual void SetEventParameter(std::string_view, std::string_view, float)   = 0;
	virtual void SetEventVolume(FmodInstance*, float)                           = 0;
	virtual void SetEventPitch(FmodInstance*, float)                            = 0;
	virtual void SetEvent3DAttributes(FmodInstance*, const FMOD_3D_ATTRIBUTES&) = 0;
	virtual void SetEvent3DAttributes(
		FmodInstance*, const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&) = 0;

	virtual void SetEventMinDistance(FmodInstance*, float)  = 0;
	virtual void SetEventMaxDistance(FmodInstance*, float)  = 0;

	[[nodiscard]] virtual bool IsPlaying(const FmodInstance*) const = 0;
	[[nodiscard]] virtual bool IsPaused(const FmodInstance*) const  = 0;

	virtual FMOD_3D_ATTRIBUTES GetAudioListener() const = 0;

	/// Sets the 3D attributes of the listener
	virtual void SetAudioListener(const FMOD_3D_ATTRIBUTES&) = 0;
	virtual void SetAudioListener(const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&) = 0;
	// ----------

	virtual const std::vector<std::string>& GetAllEventNames() = 0;
};

class NullFmodEngine : public IFmodEngine
{
public:
	// ---------- Audio Playing
	FmodInstance* PlayEvent(const AudioSource&, const Vec3f&) override { return nullptr; }
	void PlayEventOnce(const AudioSource&, const Vec3f&) override {}

	void PauseEvent(FmodInstance*) override {}
	void ResumeEvent(FmodInstance*) override {}
	void StopEvent(FmodInstance*, bool) override {}
	// ----------

	// ---------- Sound Parameters
	[[nodiscard]] const std::vector<std::string>& GetEventNames() const override
	{
		neko_assert(false, "FMOD Engine is null!");
	}

	float GetEventParameter(std::string_view, std::string_view) override { return 0.0f; }
	FMOD_3D_ATTRIBUTES GetEvent3DAttributes(FmodInstance*) const override
	{ return {}; }

	void SetEventParameter(std::string_view, std::string_view, float) override {}
	void SetEventVolume(FmodInstance*, float) override {}
	void SetEventPitch(FmodInstance*, float) override {}
	void SetEvent3DAttributes(FmodInstance*, const FMOD_3D_ATTRIBUTES&) override {}
	void SetEvent3DAttributes(
		FmodInstance*, const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&) override
	{}

	void SetEventMinDistance(FmodInstance*, float) override {}
	void SetEventMaxDistance(FmodInstance*, float) override {}

	[[nodiscard]] bool IsPlaying(const FmodInstance*) const override { return false; }
	[[nodiscard]] bool IsPaused(const FmodInstance*) const override { return false; }

	[[nodiscard]] FMOD_3D_ATTRIBUTES GetAudioListener() const override { return {}; }

	void SetAudioListener(const FMOD_3D_ATTRIBUTES&) override {}
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
	FmodInstance* PlayEvent(const AudioSource& audioSource, const Vec3f& pos) override;
	void PlayEventOnce(const AudioSource& audioSource, const Vec3f& pos) override;

	void PauseEvent(FmodInstance* instance) override;
	void ResumeEvent(FmodInstance* instance) override;
	void StopEvent(FmodInstance* instance, bool isStoppingImmediate) override;
	// ----------

	// ---------- Sound Parameters
	[[nodiscard]] const std::vector<std::string>& GetEventNames() const override { return eventNames_; }

	float GetEventParameter(
		std::string_view eventName, std::string_view eventParameterName) override;
	FMOD_3D_ATTRIBUTES GetEvent3DAttributes(FmodInstance* instance) const override;

	void SetEventParameter(std::string_view eventName,
		std::string_view eventParameterName,
		float parameterValue) override;
	void SetEventVolume(FmodInstance* instance, float volumeDb) override;
	void SetEventPitch(FmodInstance* instance, float pitch) override;
	void SetEvent3DAttributes(FmodInstance* instance, const FMOD_3D_ATTRIBUTES& attributes) override;
	void SetEvent3DAttributes(FmodInstance* instance,
		const Vec3f& position,
		const Vec3f& velocity,
		const Vec3f& forward,
		const Vec3f& up) override;

	void SetEventMinDistance(FmodInstance* instance, float minDistance) override;
	void SetEventMaxDistance(FmodInstance* instance, float maxDistance) override;

	[[nodiscard]] bool IsPlaying(const FmodInstance* instance) const override;
	[[nodiscard]] bool IsPaused(const FmodInstance* instance) const override;

	FMOD_3D_ATTRIBUTES GetAudioListener() const override;

	void SetAudioListener(const FMOD_3D_ATTRIBUTES& attributes) override;
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
