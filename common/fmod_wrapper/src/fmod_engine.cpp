#include "fmod/fmod_engine.h"

namespace neko::fmod
{
FmodEngine::FmodEngine()
{
	FMOD::System_Create(&system_);
}
}