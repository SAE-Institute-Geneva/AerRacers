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
#include "vk/models/mesh_instance.h"

namespace neko::vk
{
struct ForwardDrawCmd
{
	ForwardDrawCmd() = default;

	Mat4f worldMatrix = Mat4f::Identity;

	sole::uuid modelId = sole::uuid();

	DescriptorHandle descriptorHandle;
	UniformHandle uniformHandle;
};

using ModelInstanceIndex = std::size_t;
using ModelForwardIndex  = int;

class ModelCommandBuffer
{
public:
	ModelCommandBuffer() = default;

	//void InitData();
	void Destroy();

	ModelInstanceIndex AddModelInstanceIndex(const ModelId& modelId);
	ModelInstanceIndex AddModelInstanceIndex(const ModelId& modelId, const Mat4f& matrix);
	ModelInstanceIndex AddModelInstanceIndex(
		const ModelId& modelId, const std::vector<Mat4f>& matrices);

	void Draw(const ForwardDrawCmd& drawCommand);
	void Draw(const Mat4f& worldMatrix, ModelInstanceIndex instanceIndex);
	void Draw(const Mat4f& worldMatrix, sole::uuid model, ModelForwardIndex forwardIndex);
	void PrepareData();

	void Clear();

	std::vector<ModelInstance>& GetModelInstances() { return modelInstances_; }
	std::vector<ForwardDrawCmd>& GetForwardModels() { return forwardDrawingCmd_; }

	void AddMatrix(ModelInstanceIndex index, const Mat4f& matrix);
	void SetMatrices(ModelInstanceIndex index, const std::vector<Mat4f>& matrices);
	void SetModelId(ModelInstanceIndex index, const ModelId& modelId);

private:
	//Data for forward rendering
	std::vector<ForwardDrawCmd> forwardDrawingCmd_ {};

	//Data for gpu instancing
	std::vector<std::vector<Mat4f>> instanceMatrices_ {};
	std::vector<ModelInstance> modelInstances_ {};
};
}    // namespace neko::vk