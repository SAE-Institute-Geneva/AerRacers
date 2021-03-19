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

#include <utils/json_utility.h>
#include <utils/file_utility.h>
#include <fstream>
#include <string>
#include "engine/log.h"
#include <sstream>
#include <fmt/format.h>

#include "engine/engine.h"

namespace neko
{

bool IsJsonValueNumeric(const json::value_type& jsonValue)
{
    return jsonValue.type() == json::value_t::number_float ||
           jsonValue.type() == json::value_t::number_integer ||
           jsonValue.type() == json::value_t::number_unsigned;
}

bool CheckJsonExists(const json& jsonObject, const std::string& parameterName)
{
	return jsonObject.find(parameterName) != jsonObject.end();
}

bool CheckJsonParameter(
	const json& jsonObject, const std::string& parameterName, json::value_t expectedType)
{
	return CheckJsonExists(jsonObject, parameterName) &&
	       jsonObject[parameterName].type() == expectedType;
}

bool CheckJsonNumber(const json& jsonObject, const std::string& parameterName)
{
	return CheckJsonParameter(jsonObject, parameterName, json::value_t::number_float) ||
	       CheckJsonParameter(jsonObject, parameterName, json::value_t::number_integer) ||
	       CheckJsonParameter(jsonObject, parameterName, json::value_t::number_unsigned);
}

Vec2f GetVector2FromJson(const json& jsonObject, const std::string& parameterName)
{
	Vec2f vector;
	if (CheckJsonParameter(jsonObject, parameterName, json::value_t::array))
	{
		if (jsonObject[parameterName].size() == 2)
		{
			auto vectorJson = jsonObject[parameterName];
			if (IsJsonValueNumeric(vectorJson[0])) vector.x = vectorJson[0];
			if (IsJsonValueNumeric(vectorJson[1])) vector.y = vectorJson[1];
		}
	}
	else if (CheckJsonParameter(jsonObject, parameterName, json::value_t::object))
	{
		auto vectorJson = jsonObject[parameterName];
		if (IsJsonValueNumeric(vectorJson["x"])) vector.x = vectorJson["x"];
		if (IsJsonValueNumeric(vectorJson["y"])) vector.y = vectorJson["y"];
	}
	return vector;
}

Vec3f GetVector3FromJson(const json& jsonObject, const std::string& parameterName)
{
	Vec3f vector;
	if (CheckJsonParameter(jsonObject, parameterName, json::value_t::object))
	{
		auto vectorJson = jsonObject[parameterName];
		if (IsJsonValueNumeric(vectorJson["x"])) vector.x = vectorJson["x"];
		if (IsJsonValueNumeric(vectorJson["y"])) vector.y = vectorJson["y"];
		if (IsJsonValueNumeric(vectorJson["z"])) vector.z = vectorJson["z"];
	}

	return vector;
}

Vec4f GetVector4FromJson(const json& jsonObject, const std::string& parameterName)
{
	Vec4f vector;
	if (CheckJsonParameter(jsonObject, parameterName, json::value_t::object))
	{
		auto vectorJson = jsonObject[parameterName];
		if (IsJsonValueNumeric(vectorJson["x"])) vector.x = vectorJson["x"];
		if (IsJsonValueNumeric(vectorJson["y"])) vector.y = vectorJson["y"];
		if (IsJsonValueNumeric(vectorJson["z"])) vector.z = vectorJson["z"];
		if (IsJsonValueNumeric(vectorJson["w"])) vector.w = vectorJson["w"];
	}

	return vector;
}

json GetJsonFromVector2(const Vec2f& vector)
{
	json vectorJson = json::object();
	vectorJson["x"] = vector.x;
	vectorJson["y"] = vector.y;
	return vectorJson;
}

json GetJsonFromVector3(const Vec3f& vector)
{
    json vectorJson = json::object();
    vectorJson["x"] = vector.x;
    vectorJson["y"] = vector.y;
    vectorJson["z"] = vector.z;
    return vectorJson;
}

json GetJsonFromVector4(const Vec4f& vector)
{
    json vectorJson = json::object();
    vectorJson["x"] = vector.x;
    vectorJson["y"] = vector.y;
    vectorJson["z"] = vector.z;
    vectorJson["w"] = vector.w;
    return vectorJson;
}

json LoadJson(const std::string_view jsonPath)
{

    json jsonContent;
   
    if (!neko::BasicEngine::GetInstance()->GetFilesystem().FileExists(jsonPath))
    {
        logDebug(fmt::format("[Error] File does not exist: {}", jsonPath));
        return jsonContent;
    }

    const auto jsonStrContent = LoadFile(jsonPath.data());
    jsonContent = json::parse(jsonStrContent, nullptr, false);

    return jsonContent;
}

}
