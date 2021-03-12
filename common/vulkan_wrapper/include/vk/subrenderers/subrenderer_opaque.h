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

namespace neko::vk
{
constexpr std::string_view kProjName    = "proj";
constexpr std::string_view kViewName    = "view";
constexpr std::string_view kModelName   = "model";
constexpr std::string_view kViewPosName = "viewPos";
constexpr StringHash kProjHash          = HashString(kProjName);
constexpr StringHash kViewHash          = HashString(kViewName);
constexpr StringHash kModelHash         = HashString(kModelName);
constexpr StringHash kViewPosHash       = HashString(kViewPosName);

class SubrendererOpaque final : public RenderPipeline
{
public:
	explicit SubrendererOpaque(PipelineStage stage);
	void Destroy() const override;

	void Render(const CommandBuffer& commandBuffer) override;

	constexpr static int GetSubrendererIndex()
	{
		return static_cast<int>(SubrendererIndex::OPAQUE_S);
	}

private:
	static void ChooseViewport(const CommandBuffer& cmdBuffer,
		const VkRect2D& renderArea,
		std::uint8_t viewportCount,
		std::uint8_t viewportIndex) ;

	bool CmdRender(const CommandBuffer& commandBuffer,
		ForwardDrawCmd& modelDrawCommand,
		UniformHandle& uniformScene,
		const Mesh& mesh,
		const Material& mat);

	std::array<UniformHandle, 4> uniformScenes_ {
		UniformHandle(true),
		UniformHandle(true),
		UniformHandle(true),
		UniformHandle(true),
	};
};
}    // namespace neko::vk
