#include "aer/ui/ui_manager.h"

#include "aer/aer_engine.h"

#include "engine/resource_locations.h"

namespace neko::aer
{
UiManager::UiManager(AerEngine& aerEngine)
   : aerEngine_(aerEngine), fontManager_(aerEngine.GetFilesystem())
{}

void UiManager::Init()
{
	UiManagerLocator::provide(this);
	uiImages_.reserve(MAX_UI_ELEMENTS);
	uiTexts_.reserve(MAX_UI_ELEMENTS);

#ifdef NEKO_GLES3
	const auto& config = aerEngine_.GetConfig();
	preRender_         = Job {[this, config]()
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			InitQuad();
			uiImageShader_.LoadFromFile(GetGlShadersFolderPath() + "ui_image.vert",
				GetGlShadersFolderPath() + "ui_image.frag");

			fontManager_.Init();
			lobsterId_ = fontManager_.LoadFont(GetFontsFolderPath() + kLobsterName, 36);
			robotoId_  = fontManager_.LoadFont(GetFontsFolderPath() + kRobotoName, 36);

            uiImageShader_.BindUbo(gl::kUboUiProjSize, gl::kUboUiProjBinding);

            SetWindowSize(Vec2f(config.windowSize));
            glCheckError();
        }};

	RendererLocator::get().AddPreRenderJob(&preRender_);
#else

	fontManager_.Init();
	lobsterId_ = fontManager_.LoadFont(GetFontsFolderPath() + kLobsterName, 36);
	robotoId_  = fontManager_.LoadFont(GetFontsFolderPath() + kRobotoName, 36);
#endif
}

void UiManager::Update(seconds) {}

void UiManager::Render(std::uint8_t playerNmb)
{
#ifdef NEKO_GLES3
	if (uiImageShader_.GetProgram() == 0) return;

	uiImageShader_.Bind();
	glDisable(GL_DEPTH_TEST);

	const Vec2i winSize = Vec2i(aerEngine_.GetConfig().windowSize);
	glViewport(0, 0, winSize.x, winSize.y);

	for (auto& image : uiImages_)
	{
		auto& textureManager = aerEngine_.GetResourceManagerContainer().textureManager;
		if (image->GetFlags() & UiFlag::INITIALIZED && image->GetFlags() & UiFlag::ENABLED)
		{
			image->Draw(textureManager, playerNmb, uiImageShader_);
			DrawQuad();
		}
		else if (!(image->GetFlags() & UiFlag::INITIALIZED))
		{
			image->Init(textureManager);
			image->AddFlag(UiFlag::INITIALIZED);
		}
	}

	for (auto& text : uiTexts_)
	{
		if (text->GetFlags() & UiFlag::ENABLED)
		{
			text->Draw(fontManager_, GetFontId(text->GetFont()), playerNmb);
		}
	}

	glEnable(GL_DEPTH_TEST);
#endif
}

void UiManager::OnEvent(const SDL_Event& event)
{
	if (event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		const auto& config = aerEngine_.GetConfig();
#ifdef NEKO_GLES3
		SetWindowSize(Vec2f(config.windowSize));
#endif
		for (auto& element : uiImages_) element->AddFlag(UiFlag::DIRTY);
	}
}

void UiManager::Destroy()
{
	for (auto image : uiImages_) image->Destroy();
	uiImages_.clear();
	uiTexts_.clear();

#ifdef NEKO_GLES3
	uiImageShader_.Destroy();
	fontManager_.Destroy();
#endif
}

void UiManager::AddUiImage(UiImage* image)
{
	auto it = std::find(uiImages_.begin(), uiImages_.end(), image);
	if (it == uiImages_.end()) uiImages_.push_back(image);
	else LogDebug("UiText already draw");
}

void UiManager::AddUiText(UiText* text)
{
	auto it = std::find(uiTexts_.begin(), uiTexts_.end(), text);
	if (it == uiTexts_.end()) uiTexts_.push_back(text);
	else LogDebug("UiText already draw");
}

FontId UiManager::GetFontId(FontLoaded fontLoaded) const
{
	switch (fontLoaded)
	{
		case FontLoaded::LOBSTER: return lobsterId_;
		case FontLoaded::ROBOTO: return robotoId_;
		default: return INVALID_FONT_ID;
	}
}

void UiManager::SetWindowSize(const Vec2f windowSize)
{
	windowSize_ = windowSize;
	projection_ = Transform3d::Orthographic(0.0f, windowSize.x, 0.0f, windowSize.y);

#ifdef NEKO_GLES3
	uiImageShader_.SetUbo(gl::kUboUiProjSize, 0, &projection_, gl::kUboUiProjBinding);

	fontManager_.SetWindowSize(windowSize);
#endif
}

#ifdef NEKO_GLES3
void UiManager::InitQuad()
{
	Vec4f vertices[4] = {
		{-0.5f,  0.5f, 0.0f, 1.0f},    // Top Left
		{-0.5f, -0.5f, 0.0f, 0.0f},    // Bottom Left
		{ 0.5f, -0.5f, 1.0f, 0.0f},    // Bottom Right
		{ 0.5f,  0.5f, 1.0f, 1.0f},    // Top Right
	};

	unsigned indices[6] = {0, 1, 3, 1, 2, 3};

	glCheckError();
	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenBuffers(1, &quad_.VBO[0]);
	glGenBuffers(1, &quad_.EBO);
	glGenVertexArrays(1, &quad_.VAO);
	glBindVertexArray(quad_.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, quad_.VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4f), nullptr);
	glEnableVertexAttribArray(0);
	glCheckError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glCheckError();
}

void UiManager::DrawQuad() const
{
	glBindVertexArray(quad_.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
#endif
}    // namespace neko::aer
