#include "aer/ui/ui_manager.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
UiManager::UiManager(AerEngine& aerEngine) :
	fontManager_(aerEngine.GetFilesystem()),
	aerEngine_(aerEngine)
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
			fontManager_.Init();
			fontId_ = fontManager_.LoadFont(config.dataRootPath + "font/8-bit-hud.ttf", 36);

			const auto& config = aerEngine_.GetConfig();
			//const Vec2u windowSize = config.windowSize / Vec2u(2, 1);
			fontManager_.SetWindowSize(Vec2f(config.windowSize));
			glCheckError();
		} };

	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void UiManager::Update(seconds dt)
{
	const auto& config = aerEngine_.GetConfig();
	fontManager_.SetWindowSize(Vec2f(config.windowSize));
	fontManager_.RenderText(fontId_, "winnerText", Vec2f::zero, TextAnchor::CENTER, 1.0f, Color::red);
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
	fontManager_.Render();
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
	fontManager_.Destroy();
}
}
