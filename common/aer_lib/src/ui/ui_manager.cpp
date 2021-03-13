#include "aer/ui/ui_manager.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
UiManager::UiManager(AerEngine& aerEngine) : aerEngine_(aerEngine)
{
}
void UiManager::Init() {
	UiManagerLocator::provide(this);
	uiElements_.reserve(kMaxUiElements);
	const auto& config = aerEngine_.GetConfig();
	preRender_ = Job{ [this, config]()
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			uiShader_.LoadFromFile(
				config.dataRootPath + "shaders/opengl/base_ui.vert",
				config.dataRootPath + "shaders/opengl/base.frag");
		} };

	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void UiManager::Update(seconds dt)
{
}

void UiManager::AddUiElement(UiElement* uiElement)
{
	const auto& config = aerEngine_.GetConfig();
	if (uiElement->textureId == INVALID_TEXTURE_ID)
		uiElement->textureId = aerEngine_.GetResourceManagerContainer().textureManager.LoadTexture(
			uiElement->texturePath, Texture::DEFAULT);
	uiElement->Init(config.windowSize);
	uiElements_.push_back(uiElement);
}

void UiManager::Render()
{
	if (uiShader_.GetProgram() == 0) return;
	uiShader_.Bind();
	uiShader_.SetInt("tex", 0);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	const auto& config = aerEngine_.GetConfig();
	for (auto& element : uiElements_)
	{
		if (element->flags & UiElement::DIRTY)
		{
			element->Update(config.windowSize);
			element->flags &= ~UiElement::DIRTY;
		}
		element->Draw(aerEngine_.GetResourceManagerContainer().textureManager, config.windowSize);
	}
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void UiManager::OnEvent(const SDL_Event& event)
{
	if(event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		for (auto& element : uiElements_)
		{
			element->flags |= UiElement::DIRTY;
		}
	}
}

void UiManager::Destroy()
{
	uiElements_.clear();
}
}
