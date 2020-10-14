#include "vk/shapes/quad.h"

namespace neko::vk
{
void RenderQuad::Init()
{
    vertexBuffer_.Init(Vertices, IM_ARRAYSIZE(Vertices));
    indexBuffer_.Init(Indices, IM_ARRAYSIZE(Indices));
}

void RenderQuad::Draw() const
{
    VkBuffer vertexBuffers[] = {VkBuffer(vertexBuffer_)};
    VkDeviceSize offsets[] = {0};
    /*vkCmdBindVertexBuffers(commandBuffers_[i], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers_[i], VkBuffer(indexBuffer_), 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(commandBuffers_[i], static_cast<uint32_t>(IM_ARRAYSIZE(Indices)), 1, 0, 0, 0);*/
}

void RenderQuad::Destroy()
{
    vertexBuffer_.Destroy();
    indexBuffer_.Destroy();
}
}