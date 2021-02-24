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
#include "vk/vk_include.h"

namespace neko::vk
{
constexpr std::array<VkSampleCountFlagBits, 6> kStageFlagBits = {
	VK_SAMPLE_COUNT_64_BIT,
	VK_SAMPLE_COUNT_32_BIT,
	VK_SAMPLE_COUNT_16_BIT,
	VK_SAMPLE_COUNT_8_BIT,
	VK_SAMPLE_COUNT_4_BIT,
	VK_SAMPLE_COUNT_2_BIT,
};

struct QueueFamilyIndices
{
	std::uint32_t graphicsFamily = INVALID_INDEX;
	std::uint32_t presentFamily  = INVALID_INDEX;

	[[nodiscard]] bool IsComplete() const
	{
		return graphicsFamily != INVALID_INDEX && presentFamily != INVALID_INDEX;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice
{
public:
	explicit PhysicalDevice() = default;

	operator const VkPhysicalDevice&() const { return gpu_; }

	void Init();

	[[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface) const;

	[[nodiscard]] const QueueFamilyIndices& GetQueueFamilyIndices() const
	{
		return queueFamilyIndices_;
	}

	[[nodiscard]] VkSampleCountFlagBits GetMsaaSamples() const;
	[[nodiscard]] std::uint32_t GetMemoryType(std::uint32_t typeBits,
		VkMemoryPropertyFlags properties,
		VkBool32* memTypeFound = nullptr) const;

private:
	[[nodiscard]] QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface) const;

	[[nodiscard]] bool CheckDeviceExtensionSupport() const;
	[[nodiscard]] bool IsDeviceSuitable(VkSurfaceKHR surface) const;

	VkPhysicalDevice gpu_ {};
	QueueFamilyIndices queueFamilyIndices_ {};
};
}    // namespace neko::vk