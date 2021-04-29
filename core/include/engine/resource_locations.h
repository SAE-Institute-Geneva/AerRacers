#pragma once
#include "engine/engine.h"

namespace neko
{
constexpr const char* kBanksFolderName = "banks/";
constexpr const char* kFontsFolderName = "fonts/";
constexpr const char* kModelsFolderName = "models/";
constexpr const char* kScenesFolderName = "scenes/";
constexpr const char* kShadersFolderName = "shaders/";
constexpr const char* kGlShadersFolderName = "shaders/opengl/";
constexpr const char* kVkShadersFolderName = "shaders/vulkan/";
constexpr const char* kSpritesFolderName = "sprites/";

std::string GetBanksFolderPath();
std::vector<std::string> GetBankPaths();

std::string GetFontsFolderPath();
std::string GetModelsFolderPath();
std::string GetScenesFolderPath();
std::string GetShadersFolderPath();
std::string GetGlShadersFolderPath();
std::string GetVkShadersFolderPath();
std::string GetSpritesFolderPath();
}
