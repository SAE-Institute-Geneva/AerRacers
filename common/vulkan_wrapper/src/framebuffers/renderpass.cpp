#include "vk/framebuffers/renderpass.h"

namespace neko::vk
{
RenderPass::RenderPass(const RenderStage& renderStage,
                      VkFormat depthFormat,
                      VkFormat surfaceFormat,
                      VkSampleCountFlagBits samples)
{
    const auto& vkObj = VkObjectsLocator::get();
    const auto& attachments = renderStage.GetAttachments();
    const auto& renderStageSubpasses = renderStage.GetSubpasses();

    std::vector<VkAttachmentDescription> attachmentDescriptions;
    attachmentDescriptions.reserve(attachments.size());
    for (const auto& attachment : attachments)
    {
        auto attachmentSamples = attachment.multisampling ? samples : VK_SAMPLE_COUNT_1_BIT;
        VkAttachmentDescription attachmentDescription = {};
        attachmentDescription.samples = attachmentSamples;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        if (attachment.type == Attachment::Type::IMAGE)
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        else if (attachment.type == Attachment::Type::DEPTH)
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        switch (attachment.type)
        {
            case Attachment::Type::IMAGE:
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentDescription.format = attachment.format;
                break;
            case Attachment::Type::DEPTH:
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachmentDescription.format = depthFormat;
                break;
            case Attachment::Type::SWAPCHAIN:
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                attachmentDescription.format = surfaceFormat;
                break;
            case Attachment::Type::NONE:
                break;
            default:;
        }

        attachmentDescriptions.emplace_back(attachmentDescription);
    }

    std::vector<std::unique_ptr<SubpassDescription>> subpasses;
    subpasses.reserve(renderStageSubpasses.size());
    std::vector<VkSubpassDependency> dependencies;
    dependencies.reserve(renderStageSubpasses.size());

    for (const auto& subpassType : renderStageSubpasses)
    {
        std::vector<VkAttachmentReference> subpassColorAttachments;
        subpassColorAttachments.reserve(subpassType.attachmentBindings.size());

        std::uint32_t depthAttachment;
        for (const auto& attachmentBinding : subpassType.attachmentBindings)
        {
            const auto& attachment = renderStage.GetAttachment(attachmentBinding);
            if (attachment.type == Attachment::Type::DEPTH)
            {
                depthAttachment = attachment.binding;
                continue;
            }

            VkAttachmentReference attachmentReference{};
            attachmentReference.attachment = attachment.binding;
            attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            subpassColorAttachments.emplace_back(attachmentReference);
        }

        subpasses.emplace_back(std::make_unique<SubpassDescription>(
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        subpassColorAttachments,
                        depthAttachment));

        // Subpass dependencies.
        VkSubpassDependency subpassDependency = {};
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        if (subpassType.binding == renderStage.GetSubpasses().size())
        {
            subpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            subpassDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        }
        else subpassDependency.dstSubpass = subpassType.binding;

        if (subpassType.binding == 0)
        {
            subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependency.srcStageMask =
                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            subpassDependency.dstStageMask =
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            subpassDependency.dstAccessMask =
                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }
        else subpassDependency.srcSubpass = subpassType.binding - 1;

        dependencies.emplace_back(subpassDependency);
    }

    std::vector<VkSubpassDescription> subpassDescriptions;
    subpassDescriptions.reserve(subpasses.size());

    for (const auto& subpass : subpasses)
        subpassDescriptions.emplace_back(subpass->GetSubpassDescription());


    // Creates the render pass.
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<std::uint32_t>(attachmentDescriptions.size());
    renderPassInfo.pAttachments = attachmentDescriptions.data();
    renderPassInfo.subpassCount = static_cast<std::uint32_t>(subpassDescriptions.size());
    renderPassInfo.pSubpasses = subpassDescriptions.data();
    renderPassInfo.dependencyCount = static_cast<std::uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    const VkResult res = vkCreateRenderPass(VkDevice(vkObj.device), &renderPassInfo, nullptr, &renderPass_);
    neko_assert(res == VK_SUCCESS, "Failed to create render pass!")
}

void RenderPass::Destroy() const
{
    const auto& vkObj = VkObjectsLocator::get();
    vkDestroyRenderPass(VkDevice(vkObj.device), renderPass_, nullptr);
}
}