#pragma once
#include "vk/buffers/buffer.h"

namespace neko::vk
{
class IndexBuffer : public Buffer
{
public:
    void Init(const std::uint16_t indices[], std::size_t indexNum);
};
}
