#pragma once
#include "vk/buffers/buffer.h"

namespace neko::vk
{
class IndexBuffer : public Buffer
{
public:
    void Init(const uint16_t indices[], size_t indexNum);
};
}
