#include "aer/ui/ui_manager.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
UiManager::UiManager(AerEngine& aerEngine) : aerEngine_(aerEngine)
{
}

void UiManager::InitRender()
{
	RendererLocator::get().Render(this);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const auto& config = aerEngine_.GetConfig();
	uiShader_.LoadFromFile(
		config.dataRootPath + "shaders/opengl/base_ui.vert",
		config.dataRootPath + "shaders/opengl/base.frag");

	uiElements_.reserve(kMaxUiElements);
}

void UiManager::Init() {}

void UiManager::Update(seconds dt)
{
	RendererLocator::get().Render(this);
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
