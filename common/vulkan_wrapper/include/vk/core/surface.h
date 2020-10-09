#pragma once

#include "utilities/service_locator.h"
#include "vk/vulkan_window.h"
#include "vk/vulkan_include.h"
#include "vk/core/instance.h"
#include "vk/core/physical_device.h"

namespace neko::vk
{
class Surface
{
public:
    explicit Surface() = default;

    void Init(const sdl::VulkanWindow* window, const Instance& instance);
    void Destroy(const Instance& instance) const;

    explicit operator const VkSurfaceKHR &() const { return surface_; }
    const VkSurfaceKHR& GetSurface() const { return surface_; }
private:
    VkSurfaceKHR surface_{};
};
}
