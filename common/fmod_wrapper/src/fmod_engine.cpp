#include "fmod/fmod_engine.h"

#include "engine/resource_locations.h"

#include "sdl_engine/sdl_camera.h"

namespace neko::fmod
{
FmodEngine::FmodEngine()
{
	FMOD::System_Create(&system_);
	FmodLocator::provide(this);
}

void FmodEngine::Init()
{
	// Create main StudioSystem
	fmodCheckError(FMOD::Studio::System::create(&studioSystem_));

	// Get low level core system
	fmodCheckError(studioSystem_->getCoreSystem(&system_));
	fmodCheckError(system_->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

	// Init StudioSystem
	fmodCheckError(studioSystem_->initialize(
		kMaxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));

	system_->set3DSettings(1, 1, 1);

	// Load banks
	const auto banks = GetBankPaths();
	for (const auto& bank : banks)
		LoadBank(bank, FMOD_STUDIO_LOAD_BANK_NORMAL);
	LoadEventNames();
}

void FmodEngine::Update(seconds)
{
	// Update studio system
	fmodCheckError(studioSystem_->update());

	//const Camera3D& camera = sdl::MultiCameraLocator::get().GetCamera(0);
	//SetAudioListener(camera.position, Vec3f::zero, -camera.reverseDirection, camera.GetUp());
}

void FmodEngine::Destroy()
{
	// Destroy studio system
	fmodCheckError(studioSystem_->unloadAll());
	fmodCheckError(studioSystem_->release());

	for (auto bank : banks_) fmodCheckError(bank.second->unload());
}

// ---------- Resources loading
void FmodEngine::LoadBank(std::string_view bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	const auto it = banks_.find(bankName.data());
	if (it != banks_.end()) return;

	FMOD::Studio::Bank* bank = nullptr;
	fmodCheckError(studioSystem_->loadBankFile(bankName.data(), flags, &bank));

	if (bank)
	{
		banks_[bankName.data()] = bank;
		logDebug(fmt::format("The FMOD bank [{}] has successfully been loaded", bankName));
	}
}

void FmodEngine::LoadEventNames()
{
	for (const auto& bank : banks_)
	{
		//Get the number of event
		int count;
		fmodCheckError(bank.second->getEventCount(&count));

		//If == 0, continue because there is no events in the bank
		if (count == 0) continue;

		//Get the event description list
		auto** events = new FMOD::Studio::EventDescription*[count];
		memset(events, 0, sizeof(&events));
		fmodCheckError(bank.second->getEventList(events, count, &count));

		//Foreach event, get the path and keep it in eventNames
		for (int i = 0; i < count; i++)
		{
			char eventPath[256];
			int len = 0;
			fmodCheckError(events[i]->getPath(eventPath, sizeof(eventPath) - 1, &len));
			std::string name = eventPath;
			eventNames_.push_back(name);
		}

		logDebug(
			fmt::format("{} events have been loaded from the FMOD bank [{}]", count, bank.first));
		delete[] events;
	}

	for (auto& eventName : eventNames_)
		eventName = eventName.substr(7, eventName.size());
	std::sort(eventNames_.begin(), eventNames_.end());

	logDebug(fmt::format("{} total events have been loaded", eventNames_.size()));
}
// ----------

// ---------- Audio Playing
FmodInstance* FmodEngine::PlayEvent(const AudioSource& audioSource, const Vec3f& pos)
{
	FmodInstance* instance = nullptr;
	if (!audioSource.instance_)
	{
		FMOD::Studio::EventDescription* description = nullptr;
		studioSystem_->getEvent((kEventPrefix + audioSource.eventName_).c_str(), &description);

		int count = 0;
		description->getInstanceCount(&count);
		if (count >= kMaxInstance) return nullptr;

		fmodCheckError(description->createInstance(&instance));
		fmodCheckError(instance->setVolume(audioSource.GetVolume() / kMaxVolume));
		fmodCheckError(instance->setPitch(audioSource.GetPitch()));

		FMOD_3D_ATTRIBUTES attributes;
		attributes.position = Vec3ToFmod(pos);
		attributes.up       = {0, 1, 0};
		attributes.forward  = {0, 0, 1};
		fmodCheckError(instance->set3DAttributes(&attributes));

		fmodCheckError(instance->setProperty(
			FMOD_STUDIO_EVENT_PROPERTY_MINIMUM_DISTANCE, audioSource.GetMinDistance()));
		fmodCheckError(instance->setProperty(
			FMOD_STUDIO_EVENT_PROPERTY_MAXIMUM_DISTANCE, audioSource.GetMaxDistance()));
	}
	else
	{
		instance = audioSource.instance_;
	}

	fmodCheckError(instance->start());
	return instance;
}

void FmodEngine::PlayEventOnce(const AudioSource& audioSource, const Vec3f& pos)
{
	auto instance = PlayEvent(audioSource, pos);
	fmodCheckError(
		instance->setCallback(EventInstanceStopped, FMOD_STUDIO_EVENT_CALLBACK_SOUND_STOPPED));
}

void FmodEngine::PauseEvent(FmodInstance* instance)
{
	fmodCheckError(instance->setPaused(true));
}

void FmodEngine::ResumeEvent(FmodInstance* instance)
{
	fmodCheckError(instance->setPaused(false));
}

void FmodEngine::StopEvent(FmodInstance* instance, bool isStoppingImmediate)
{
	const auto mode =
		isStoppingImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;

	fmodCheckError(instance->stop(mode));
	fmodCheckError(instance->release());
}
// ----------

// ---------- Sound Parameters
float FmodEngine::GetEventParameter(std::string_view eventName, std::string_view eventParameterName)
{
	FMOD::Studio::EventDescription* description = nullptr;
	studioSystem_->getEvent((kEventPrefix + std::string(eventName)).c_str(), &description);

	FmodInstance* instance = nullptr;
	fmodCheckError(description->createInstance(&instance));

	float value = 0;
	fmodCheckError(instance->getParameterByName(eventParameterName.data(), &value));
	return value;
}

FMOD_3D_ATTRIBUTES FmodEngine::GetEvent3DAttributes(FmodInstance* instance) const
{
	FMOD_3D_ATTRIBUTES attributes;
	fmodCheckError(instance->get3DAttributes(&attributes));
	return attributes;
}

void FmodEngine::SetEventParameter(
	std::string_view eventName, std::string_view eventParameterName, float parameterValue)
{
	FMOD::Studio::EventDescription* description = nullptr;
	studioSystem_->getEvent((kEventPrefix + std::string(eventName)).c_str(), &description);

	FmodInstance* instance = nullptr;
	fmodCheckError(description->createInstance(&instance));

	fmodCheckError(instance->setParameterByName(eventParameterName.data(), parameterValue));
}

void FmodEngine::SetEventVolume(FmodInstance* instance, float volumeDb)
{
	instance->setVolume(volumeDb / kMaxVolume);
}

void FmodEngine::SetEventPitch(FmodInstance* instance, float pitch)
{
	instance->setPitch(pitch);
}

void FmodEngine::SetEvent3DAttributes(FmodInstance* instance, const FMOD_3D_ATTRIBUTES& attributes)
{
	fmodCheckError(instance->set3DAttributes(&attributes));
}

void FmodEngine::SetEvent3DAttributes(FmodInstance* instance,
	const Vec3f& position,
	const Vec3f& velocity,
	const Vec3f& forward,
	const Vec3f& up)
{
	FMOD_3D_ATTRIBUTES attributes {};
	attributes.position = Vec3ToFmod(position);
	attributes.velocity = Vec3ToFmod(velocity);
	attributes.forward  = Vec3ToFmod(forward);
	attributes.up       = Vec3ToFmod(up);
	fmodCheckError(instance->set3DAttributes(&attributes));
}

void FmodEngine::SetEventMinDistance(FmodInstance* instance, float minDistance)
{
	fmodCheckError(instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MINIMUM_DISTANCE, minDistance));
}

void FmodEngine::SetEventMaxDistance(FmodInstance* instance, float maxDistance)
{
	fmodCheckError(instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MAXIMUM_DISTANCE, maxDistance));
}

bool FmodEngine::IsPlaying(const FmodInstance* instance) const
{
	if (!instance) return false;

	FMOD_STUDIO_PLAYBACK_STATE state;
	fmodCheckError(instance->getPlaybackState(&state));
	return state == FMOD_STUDIO_PLAYBACK_PLAYING;
}

bool FmodEngine::IsPaused(const FmodInstance* instance) const
{
	if (!instance) return false;

	bool isPaused = false;
	instance->getPaused(&isPaused);
	return isPaused;
}

FMOD_3D_ATTRIBUTES FmodEngine::GetAudioListener() const
{
	FMOD_3D_ATTRIBUTES attributes;
	fmodCheckError(studioSystem_->getListenerAttributes(0, &attributes));
	return attributes;
}

void FmodEngine::SetAudioListener(const FMOD_3D_ATTRIBUTES& attributes)
{
	studioSystem_->setListenerAttributes(0, &attributes);
}

void FmodEngine::SetAudioListener(
	const Vec3f& position, const Vec3f& velocity, const Vec3f& forward, const Vec3f& up)
{
	FMOD_3D_ATTRIBUTES attributes;
	attributes.position = Vec3ToFmod(position);
	attributes.forward  = Vec3ToFmod(forward);
	attributes.up       = Vec3ToFmod(up);
	attributes.velocity = Vec3ToFmod(velocity);

	studioSystem_->setListenerAttributes(0, &attributes);
}
// ----------

FMOD_RESULT FmodEngine::EventInstanceStopped(
	FMOD_STUDIO_EVENT_CALLBACK_TYPE, FMOD_STUDIO_EVENTINSTANCE* instance, void*)
{
	auto eventInstance = reinterpret_cast<FmodInstance*>(instance);
	return eventInstance->release();
}
}    // namespace neko::fmod