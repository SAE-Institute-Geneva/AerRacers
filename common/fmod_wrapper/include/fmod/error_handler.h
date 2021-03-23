#pragma once
#include <fmod.h>

#ifdef NEKO_ASSERT
namespace neko::fmod
{
void CheckFmodError(FMOD_RESULT err, const char* file, int line);
}
#define fmodCheckError(err) CheckFmodError(err, __FILE__, __LINE__)
#else
#define fmodCheckError(err) void(0);
#endif
