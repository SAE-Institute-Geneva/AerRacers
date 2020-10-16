#pragma once
#include "vk/buffers/buffer.h"
#include "vk/commands/command_pool.h"

namespace neko::vk
{
class IndexBuffer : public Buffer
{
public:
    void Init(const uint16_t indices[], size_t indexNum);
};
}
