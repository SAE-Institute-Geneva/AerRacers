#pragma once
/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <string>
#include <memory>
 //Externals includes
#include <json.hpp>
#include "mathematics/rect.h"

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace neko
{
bool IsJsonValueNumeric(const json::value_type& jsonValue);

bool CheckJsonExists(const json& jsonObject, const std::string& parameterName);

bool CheckJsonParameter(const json& jsonObject, const std::string& parameterName, json::value_t expectedType);

bool CheckJsonNumber(const json& jsonObject, const std::string& parameterName);

Vec2f GetVector2FromJson(const json& jsonObject, const std::string& parameterName);

Vec3f GetVector3FromJson(const json& jsonObject, const std::string& parameterName);

Vec4f GetVector4FromJson(const json& jsonObject, const std::string& parameterName);

json GetJsonFromVector2(const Vec2f& vector);

json GetJsonFromVector3(const Vec3f& vector);

json GetJsonFromVector4(const Vec4f& vector);

Rect2f GetRectFromJson(const json& jsonObject, std::string parameterName);

json LoadJson(const std::string_view jsonPath);
}