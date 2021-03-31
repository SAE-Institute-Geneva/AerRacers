#pragma once
#include <fmod.h>

#ifdef NEKO_ASSERT
namespace neko::fmod
{
/// Checks and prints for errors in FMOD functions
/// \param err: the FMOD error code
/// \param file: the file in which to check for error
/// \param line: the line at which to check for error
void CheckFmodError(FMOD_RESULT err, const char* file, int line);
}
#define fmodCheckError(err) CheckFmodError(err, __FILE__, __LINE__)
#else
#define fmodCheckError(err) void(0);
#endif
