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
    explicit Surface(const Instance& instance);

    void Init(const sdl::VulkanWindow* window);
    void Destroy();

    explicit operator const VkSurfaceKHR &() const { return surface_; }
    const VkSurfaceKHR& GetSurface() { return surface_; }
private:
    const Instance& instance_;

    VkSurfaceKHR surface_{};
};
}
