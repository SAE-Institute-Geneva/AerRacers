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
#include "vk/models/mesh.h"

namespace neko::vk
{
class RenderQuadUi final
{
public:
	void Init();

	[[nodiscard]] bool DrawCmd(
		const CommandBuffer& commandBuffer, std::uint32_t instance = 1) const;

	[[nodiscard]] ResourceHash GetMaterialId() const { return materialId_; }
	void SetMaterialId(ResourceHash resourceId) { materialId_ = resourceId; }

	[[nodiscard]] static VertexInput GetVertexInput(std::uint32_t baseBinding = 0);

private:
	void InitVertices();
	void InitIndices();

	Buffer vertexBuffer_ {};
	Buffer indexBuffer_ {};

	ResourceHash materialId_ = 0;
};
}    // namespace neko::vk