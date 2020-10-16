#include "vk/shapes/quad.h"

namespace neko::vk
{
void RenderQuad::Init()
{
    vertexBuffer.Init(kVertices, IM_ARRAYSIZE(kVertices));
    indexBuffer.Init(kIndices, IM_ARRAYSIZE(kIndices));
}

void RenderQuad::Draw() const
{
	/*VkBuffer vertexBuffers[] = {VkBuffer(vertexBuffer)};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers_[i], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers_[i], VkBuffer(indexBuffer_), 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(commandBuffers_[i], static_cast<uint32_t>(IM_ARRAYSIZE(Indices)), 1, 0, 0, 0);*/
}

void RenderQuad::Destroy()
{
    vertexBuffer.Destroy();
    indexBuffer.Destroy();
}
}