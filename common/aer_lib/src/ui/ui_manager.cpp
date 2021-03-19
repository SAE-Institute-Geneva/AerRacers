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
	uiImages_.reserve(MAX_UI_ELEMENTS);
	uiTexts_.reserve(MAX_UI_ELEMENTS);
	const auto& config = aerEngine_.GetConfig();


	preRender_ = Job{ [this, config]()
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			uiImageShader_.LoadFromFile(
				config.dataRootPath + "shaders/opengl/ui_image.vert",
				config.dataRootPath + "shaders/opengl/ui_image.frag");
			fontManager_.Init();
			lobsterId_ = fontManager_.LoadFont(config.dataRootPath + kLobsterPath_, 36);
			robotoId_ = fontManager_.LoadFont(config.dataRootPath + kRobotoPath_, 36);

			const auto& config = aerEngine_.GetConfig();
			//const Vec2u windowSize = config.windowSize / Vec2u(2, 1);
			fontManager_.SetWindowSize(Vec2f(config.windowSize));
			glCheckError();
		} };

	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void UiManager::Update(seconds dt)
{
	auto& inputManager = sdl::InputLocator::get();
}

void UiManager::Render(uint8_t playerNmb)
{
	if (uiImageShader_.GetProgram() == 0) return;
	uiImageShader_.Bind();
	uiImageShader_.SetInt("tex", 0);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	const auto& config = aerEngine_.GetConfig();
	for (auto& image : uiImages_)
	{
		if (!(image->GetFlags() & UiFlag::INIT)) {
			image->Init(aerEngine_.GetResourceManagerContainer().textureManager);
			image->AddFlag(UiFlag::INIT);
		}
		if ((image->GetFlags() & UiFlag::INIT) && (image->GetFlags() & UiFlag::ENABLE)) {
			ChangeViewport(image->GetScreenId(), playerNmb, config.windowSize);
			image->Draw(aerEngine_.GetResourceManagerContainer().textureManager, config.windowSize, uiImageShader_);
		}
	}
	glCullFace(GL_BACK);
	for (auto& text : uiTexts_)
	{
		if (text->GetFlags() & UiFlag::ENABLE) {
			ChangeViewport(text->GetScreenId(), playerNmb, config.windowSize);
			text->Draw(fontManager_, GetFontId(text->GetFont()));
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void UiManager::OnEvent(const SDL_Event& event)
{
	if(event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		const auto& config = aerEngine_.GetConfig();
		fontManager_.SetWindowSize(Vec2f(config.windowSize));
		for (auto& element : uiImages_)
		{
			element->AddFlag(UiFlag::DIRTY);
		}
	}
}

void UiManager::Destroy()
{
    for (auto image : uiImages_) {
		image->Destroy();
    }
	uiImages_.clear();
	uiTexts_.clear();
	uiImageShader_.Destroy();
	fontManager_.Destroy();
}

void UiManager::AddUiImage(UiImage* image) {
	auto it = std::find(uiImages_.begin(), uiImages_.end(), image);
	if (it == uiImages_.end())
	{
		uiImages_.push_back(image);
	} else {
		LogDebug("UiText already draw");
	}
}

void UiManager::AddUiText(UiText* text) {
	auto it = std::find(uiTexts_.begin(), uiTexts_.end(), text);
	if (it == uiTexts_.end())
	{
		uiTexts_.push_back(text);
	} else {
		LogDebug("UiText already draw");
	}
}

FontId UiManager::GetFontId(FontLoaded fontLoaded) const
{
    switch (fontLoaded) {
        case FontLoaded::LOBSTER: return lobsterId_;
            break;
        case FontLoaded::ROBOTO: return robotoId_;
            break;
        default: ;
    }
	return INVALID_FONT_ID;
}

void UiManager::ChangeViewport(const uint8_t screenId, const uint8_t playerNmb, const Vec2u& windowSize)
{
	if (screenId > playerNmb || screenId == 0) {
		glViewport(0, 0, windowSize.x, windowSize.y);
	}
	else
	{
		switch (screenId) {
		case 1: {
			switch (playerNmb) {
			case 2: {
				glViewport(0, 0, windowSize.x / 2, windowSize.y); }
				  break;
			case 3:
			case 4: {
				glViewport(0, windowSize.y / 2, windowSize.x / 2, windowSize.y / 2); }
				  break;
			default: {
				glViewport(0, 0, windowSize.x, windowSize.y); };
			}
		}
			  break;
		case 2: {
			switch (playerNmb) {
			case 2: {
				glViewport(windowSize.x / 2, 0, windowSize.x / 2, windowSize.y); }
				  break;
			case 3:
			case 4: {
				glViewport(windowSize.x / 2, windowSize.y / 2, windowSize.x / 2, windowSize.y / 2); }
				  break;
			default: {
				glViewport(0, 0, windowSize.x, windowSize.y); };
			}
		}
			  break;
		case 3: {
			glViewport(0, 0, windowSize.x / 2, windowSize.y / 2);
		}
			  break;
		case 4: {
			glViewport(windowSize.x / 2, 0, windowSize.x / 2, windowSize.y / 2);
		}
			  break;
		default:;
		}
	}
    
}
}
