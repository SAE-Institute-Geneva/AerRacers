#pragma once
#include "vk/buffers/buffer.h"
#include "vk/core/logical_device.h"
#include "vk/commands/command_pool.h"

namespace neko::vk
{
class IndexBuffer : public Buffer
{
public:
    void Init();
};
}
