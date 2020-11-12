#pragma once
#include "SDL.h"

#include "vk/vulkan_include.h"

namespace neko::vk
{
class Surface
{
public:
    explicit Surface() = default;

    void Init(SDL_Window& window);
    void SetFormat();
    void Destroy() const;

    explicit operator const VkSurfaceKHR &() const { return surface_; }
    [[nodiscard]] const VkSurfaceKHR& GetSurface() const { return surface_; }

    [[nodiscard]] VkSurfaceFormatKHR GetFormat() const { return format_; }

    void CreateSurface(SDL_Window& window);

private:
    VkSurfaceKHR surface_{};
    VkSurfaceFormatKHR format_{};
};
}
