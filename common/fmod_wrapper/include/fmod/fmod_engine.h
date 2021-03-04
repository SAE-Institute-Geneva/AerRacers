#pragma once
#include "fmod_studio.hpp"

namespace neko::fmod
{
class FmodEngine
{
public:
	FmodEngine();

private:
	FMOD::System* system_ {};
};
}
