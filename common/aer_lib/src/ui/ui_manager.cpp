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
	uiImages_.reserve(kMaxUiElements);
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
	uiImages_.clear();
}

void UiManager::Render()
{
	if (uiImageShader_.GetProgram() == 0) return;
	uiImageShader_.Bind();
	uiImageShader_.SetInt("tex", 0);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	const auto& config = aerEngine_.GetConfig();
	for (auto& image : uiImages_)
	{
		if (!(image->flags & UiElement::INIT)) {
			image->Init(aerEngine_.GetResourceManagerContainer().textureManager);
			image->flags |= UiElement::INIT;
		}
		image->Draw(aerEngine_.GetResourceManagerContainer().textureManager, config.windowSize, uiImageShader_);
	}
	glCullFace(GL_BACK);
	fontManager_.Render();
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
			element->flags |= UiElement::DIRTY;
		}
	}
}

void UiManager::Destroy()
{
	uiImages_.clear();
	fontManager_.Destroy();
}

void UiManager::RenderUiImage(UiImage* image) {
	uiImages_.push_back(image);
}

void UiManager::RenderUiText(FontLoaded fontLoaded,
    const std::string_view& text,
    const Vec2f& position,
    UiAnchor anchor,
    float scale,
    const Color4& color) {
    switch (fontLoaded) {
        case FontLoaded::LOBSTER:
			fontManager_.RenderText(lobsterId_, text, position, TextAnchor(anchor), scale, color); break;
        case FontLoaded::ROBOTO:
			fontManager_.RenderText(robotoId_, text, position, TextAnchor(anchor), scale, color); break;
        default: ;
    }
}
}
