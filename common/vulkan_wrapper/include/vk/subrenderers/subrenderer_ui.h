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
#include "vk/commands/model_command_buffer.h"
#include "vk/pipelines/render_pipeline.h"
#include "vk/ui/quad_ui.h"

namespace neko::vk
{
constexpr static std::string_view kUboUiProjName   = "UiScene";
constexpr static std::string_view kUboUiObjectName = "UiObject";
constexpr static StringHash kUboUiProjHash         = HashString(kUboUiProjName);
constexpr static StringHash kUboUiObjectHash       = HashString(kUboUiObjectName);

constexpr std::string_view kProjUiName  = "proj";
constexpr std::string_view kModelUiName = "model";
constexpr StringHash kProjUiHash        = HashString(kProjName);
constexpr StringHash kModelUiHash       = HashString(kModelUiName);
class SubrendererUi final : public RenderPipeline
{
public:
	explicit SubrendererUi(PipelineStage stage);
	void Destroy() const override;

	void Render(const CommandBuffer& commandBuffer) override;

	constexpr static int GetSubrendererIndex()
	{
		return static_cast<int>(SubrendererIndex::UI);
	}

	UniformHandle& GetUniformScene() { return uniformScene_; }

private:
	bool CmdRender(
		const CommandBuffer& commandBuffer, ForwardDrawCmd& drawCmd, const Material& mat);

	UniformHandle uniformScene_;
	RenderQuadUi quadUi_;
};
}
