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
#include "vk/framebuffers/renderpass.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/images/image2d.h"
#include "vk/images/image_depth.h"

namespace neko::vk
{
class Framebuffers
{
public:
	Framebuffers(std::uint32_t width,
		std::uint32_t height,
		const RenderStage& renderStage,
		const RenderPass& renderPass,
		const Swapchain& swapchain,
		const ImageDepth& depthStencil,
		VkSampleCountFlagBits samples);

	const VkFramebuffer& operator[](const size_t index) const { return framebuffers_[index]; }
	VkFramebuffer& operator[](const size_t index) { return framebuffers_[index]; }

	void Destroy() const;

	[[nodiscard]] std::size_t GetFramebufferCount() const { return framebuffers_.size(); }
	[[nodiscard]] const std::vector<VkFramebuffer>& GetFramebuffers() const
	{
		return framebuffers_;
	}

	[[nodiscard]] const Image2d& GetAttachment(std::uint32_t index) const
	{
		return *imageAttachments_[index];
	}

private:
	std::vector<std::unique_ptr<Image2d>> imageAttachments_;
	std::vector<VkFramebuffer> framebuffers_;
};
}    // namespace neko::vk