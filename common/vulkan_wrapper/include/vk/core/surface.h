#pragma once

#include "utilities/service_locator.h"
#include "vk/vulkan_include.h"
#include "vk/core/instance.h"
#include "vk/core/physical_device.h"

namespace neko::vk
{
class ISurface
{
public:
    virtual explicit operator const VkSurfaceKHR &() const = 0;
    virtual const VkSurfaceKHR& GetSurface() = 0;
};

class NullSurface : public ISurface
{
public:
    explicit operator const VkSurfaceKHR&() const override
    {
        neko_assert(false, "Vulkan Surface is null!")
        return nullptr;
    }

    const VkSurfaceKHR& GetSurface() override
    {
        neko_assert(false, "Vulkan Surface is null!")
        return nullptr;
    }
};

class Surface : public ISurface
{
public:
    Surface(const Instance& instance, SDL_Window* window);
    ~Surface();

    explicit operator const VkSurfaceKHR &() const override { return surface_; }
    const VkSurfaceKHR& GetSurface() override { return surface_; }
private:
    const Instance& instance_;

    VkSurfaceKHR surface_{};
};

using SurfaceLocator = Locator<ISurface, NullSurface>;
}
