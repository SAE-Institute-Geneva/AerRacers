#pragma once
#include "vk/renderers/renderer.h"

namespace neko::vk
{
class RendererEditor : public Renderer
{
public:
    RendererEditor() = default;

    void Init() override;
    void Start() override;
	void Destroy() const override;
};
}