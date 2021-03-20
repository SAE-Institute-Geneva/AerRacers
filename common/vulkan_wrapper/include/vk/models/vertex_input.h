#pragma once
/* ----------------------------------------------------
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

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "mathematics/vector.h"

#include "vk/vk_include.h"

namespace neko::vk
{
class VertexInput
{
public:
	explicit VertexInput(std::uint32_t binding                               = 0,
		const VkVertexInputBindingDescription& bindingDescriptions           = {},
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {});

	//To sort by bindings
	bool operator<(const VertexInput& other) const;

	[[nodiscard]] std::uint32_t GetBinding() const { return binding_; }

	[[nodiscard]] const VkVertexInputBindingDescription& GetBindingDescription() const
	{
		return bindingDescription_;
	}

	[[nodiscard]] const std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions()
		const
	{
		return attributeDescription_;
	}

private:
	std::uint32_t binding_;
	VkVertexInputBindingDescription bindingDescription_;
	std::vector<VkVertexInputAttributeDescription> attributeDescription_;
};

struct Vertex
{
	Vertex() = default;
	Vertex(const Vec3f& pos, const Vec3f& norm, const Vec2f& uv);

	bool operator==(const Vertex& other) const;

	static VertexInput GetVertexInput(std::uint32_t binding = 0);

	Vec3f position  = Vec3f::zero;
	Vec3f normal    = Vec3f::zero;
	Vec2f texCoords = Vec2f::zero;
	Vec3f tangent   = Vec3f::zero;
	Vec3f bitangent = Vec3f::zero;
};
}    // namespace neko::vk
