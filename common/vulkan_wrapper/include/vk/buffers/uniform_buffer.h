#pragma once
#include "mathematics/matrix.h"

#include "vk/buffers/buffer.h"
#include "vk/core/logical_device.h"

namespace neko::vk
{
struct UniformBufferObject
{
    Mat4f model = Mat4f::Identity;
    Mat4f view = Mat4f::Identity;
    Mat4f proj = Mat4f::Identity;
};

struct UniformBuffer : public Buffer
{
    void Init(const PhysicalDevice& gpu, const LogicalDevice& device);
};
}