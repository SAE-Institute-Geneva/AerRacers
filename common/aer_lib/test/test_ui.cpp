/* ----------------------------------------------------
 MIT License
 Copyright (c) 2020 SAE Institute Switzerland AG
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 Author : Floreau Luca
 Co-Author :
 Date : 13.03.2021
---------------------------------------------------------- */
#include <gtest/gtest.h>

#include "engine/resource_locations.h"

#include "aer/aer_engine.h"

#ifdef NEKO_GLES3
#include "gl/graphics.h"
#else
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"
#endif

namespace neko::aer
{
class TestUiManager : public SystemInterface, public RenderCommandInterface, public DrawImGuiInterface
{
public:
	TestUiManager(AerEngine& engine)
	   : engine_(engine), cContainer_(engine.GetComponentManagerContainer()),
		 slidingBar_(),
		 anchorBl_(),
		 anchorBr_(),
		 anchorTl_(),
		 anchorTr_(),
		 movingImage_()
	{
		Init();
	}

	void Init() override
	{
		Entity testEntity = cContainer_.entityManager.CreateEntity();
		cContainer_.transform3dManager.AddComponent(testEntity);
		cContainer_.transform3dManager.SetRelativePosition(testEntity, Vec3f::zero);
		cContainer_.renderManager.AddComponent(testEntity);
		cContainer_.renderManager.SetModel(testEntity, GetModelsFolderPath() + "cube/cube.obj");

		// Images
		slidingBar_     = UiImage(GetSpritesFolderPath() + "wall.jpg",
            Vec2i::zero,
            Vec2u(250, 100),
            UiAnchor::CENTER);
		anchorTr_       = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(-150, -150),
            Vec2u::one * 150,
            UiAnchor::TOP_RIGHT);
		anchorTl_       = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(150, -150),
            Vec2u::one * 150,
            UiAnchor::TOP_LEFT);
		anchorBl_       = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(150, 150),
            Vec2u::one * 150,
            UiAnchor::BOTTOM_LEFT);
		anchorBr_       = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(-150, 150),
            Vec2u::one * 150,
            UiAnchor::BOTTOM_RIGHT);
		movingImage_    = UiImage(GetSpritesFolderPath() + "wall.jpg",
            Vec2i::up * 180,
            Vec2u(250, 100),
            UiAnchor::CENTER);

		// Text
		textCenter_ = UiText(FontLoaded::LOBSTER,
            "Center",
            Vec2i::zero,
            UiAnchor::CENTER,
            0,
            1.0f,
            Color::blue);
		textBl_         = UiText(FontLoaded::LOBSTER,
            "BL",
            Vec2i(100, 100),
            UiAnchor::BOTTOM_LEFT,
            0,
            1.0f,
            Color::cyan);
		textBr_         = UiText(FontLoaded::LOBSTER,
            "BR",
            Vec2i(-100, 100),
            UiAnchor::BOTTOM_RIGHT,
            0,
            1.0f,
            Color::yellow);
		textTl_         = UiText(FontLoaded::LOBSTER,
            "TL",
            Vec2i(100, -100),
            UiAnchor::TOP_LEFT,
            0,
            1.0f,
            Color::red);
		textTr_         = UiText(FontLoaded::LOBSTER,
            "TR",
            Vec2i(-100, -100),
            UiAnchor::TOP_RIGHT,
            0,
            1.0f,
            Color::green);
		movingText_     = UiText(FontLoaded::LOBSTER,
            std::to_string(updateCount_),
            Vec2i::up * 180,
            UiAnchor::CENTER,
            0,
            1.0f,
            Color::blue);

		// Register UI elements
		auto& uiManager = UiManagerLocator::get();
		uiManager.AddUiImage(&slidingBar_);
		uiManager.AddUiImage(&anchorTr_);
		uiManager.AddUiImage(&anchorTl_);
		uiManager.AddUiImage(&anchorBl_);
		uiManager.AddUiImage(&anchorBr_);
		uiManager.AddUiImage(&movingImage_);
		uiManager.AddUiText(&textCenter_);
		uiManager.AddUiText(&textBl_);
		uiManager.AddUiText(&textBr_);
		uiManager.AddUiText(&textTl_);
		uiManager.AddUiText(&textTr_);
		uiManager.AddUiText(&movingText_);
	}

	void Update(seconds dt) override
	{
		updateCount_ += dt.count();

		const Vec2f newPos = (Vec2f::up * 0.5f).Rotate(radian_t(updateCount_)) * 700.0f;
		movingText_.SetPosition(Vec2i(newPos));
		movingText_.SetText(std::to_string(updateCount_));
		movingImage_.SetPosition(Vec2i(newPos));

		const std::uint8_t screen = static_cast<int>(updateCount_) % 5;
		slidingBar_.SetCropping(Vec2f(Abs(Sin(radian_t(updateCount_))), 1.0f));
		slidingBar_.SetScreenId(screen);
		anchorTr_.SetScreenId(screen);
		anchorTl_.SetScreenId(screen);
		anchorBl_.SetScreenId(screen);
		anchorBr_.SetScreenId(screen);
		textBl_.SetScreenId(screen);
		textBr_.SetScreenId(screen);
		textTl_.SetScreenId(screen);
		textTr_.SetScreenId(screen);
		textCenter_.SetScreenId(screen);
		textCenter_.SetCropping(Vec2f(Abs(Sin(radian_t(updateCount_))), 1.0f));
		movingImage_.SetCropping(Vec2f(Abs(Sin(radian_t(updateCount_))), 1.0f));

		//if (updateCount_ > kEngineDuration_) engine_.Stop();
	}

	void Render() override {}

	void Destroy() override {}

	void DrawImGui() override {}

	~TestUiManager() override = default;

private:
	float updateCount_           = 0;
	const float kEngineDuration_ = 3.0f;

	AerEngine& engine_;

	ComponentManagerContainer& cContainer_;

	//Test anchor
	UiImage slidingBar_;
	UiImage anchorBl_;
	UiImage anchorBr_;
	UiImage anchorTl_;
	UiImage anchorTr_;
	UiImage movingImage_;
	UiText textCenter_;
	UiText textBl_;
	UiText textBr_;
	UiText textTl_;
	UiText textTr_;
	UiText movingText_;
};

TEST(UIManager, TestWithEngine)
{
	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1280, 720);
	config.flags      = Configuration::NONE;

	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::EDITOR);
#ifdef NEKO_GLES3
	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
#else
	sdl::VulkanWindow window;
	vk::VkRenderer renderer(&window);
	renderer.SetRenderer(std::make_unique<neko::vk::RendererEditor>());
#endif

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();

	TestUiManager testUiManager(engine);
	engine.RegisterSystem(testUiManager);
	engine.RegisterOnDrawUi(testUiManager);

	engine.EngineLoop();

#ifdef EASY_PROFILE_USE
	profiler::dumpBlocksToFile("UiManager_Neko_Profile.prof");
#endif
}

struct Menu
{
	Menu() : buttonPrev(), buttonNext() {}
	UiImage backGround;
	UiImage buttonPrev;
	UiImage buttonNext;
	UiText text;
	UiText textNext;
	UiText textPrevious;

	void SetEnable(bool enable)
	{
		backGround.SetEnable(enable);
		buttonPrev.SetEnable(enable);
		buttonNext.SetEnable(enable);
		text.SetEnable(enable);
		textNext.SetEnable(enable);
		textPrevious.SetEnable(enable);
	}
};

class TestUiMenu : public SystemInterface, public RenderCommandInterface, public DrawImGuiInterface
{
public:
	explicit TestUiMenu(AerEngine& engine)
	   : engine_(engine), cContainer_(engine.GetComponentManagerContainer()), currentMenu_(&menuA_)
	{
		Init();
	}

	void Init() override
	{
		const auto& config = BasicEngine::GetInstance()->GetConfig();
		auto& uiManager    = UiManagerLocator::get();

		// Menu A
		menuA_.backGround = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i::zero,
            config.windowSize * 0.9f,
            UiAnchor::CENTER,
            Color::red,
            0);
		menuA_.text         = UiText(FontLoaded::LOBSTER,
            "MenuA",
            Vec2i::down * 220,
            UiAnchor::TOP,
            0,
            1.0f,
            Color::white);
		menuA_.buttonNext   = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(-400, 200),
            Vec2u(200, 100),
            UiAnchor::BOTTOM_RIGHT,
            Color::white,
            0);
		menuA_.buttonPrev   = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(400, 200),
            Vec2u(200, 100),
            UiAnchor::BOTTOM_LEFT,
            Color::white,
            0);
		menuA_.textNext     = UiText(FontLoaded::LOBSTER,
            "Next",
            Vec2i(-400, 200),
            UiAnchor::BOTTOM_RIGHT,
            0,
            1.0f,
            Color::white);
		menuA_.textPrevious = UiText(FontLoaded::LOBSTER,
			"Previous",
			Vec2i(400, 200),
			UiAnchor::BOTTOM_LEFT,
			0,
			1.0f,
			Color::white);

		// Register menu A
		uiManager.AddUiImage(&menuA_.backGround);
		uiManager.AddUiImage(&menuA_.buttonPrev);
		uiManager.AddUiImage(&menuA_.buttonNext);
		uiManager.AddUiText(&menuA_.text);
		uiManager.AddUiText(&menuA_.textNext);
		uiManager.AddUiText(&menuA_.textPrevious);

		// Menu B
		menuB_.backGround   = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i::zero,
            config.windowSize * 0.9f,
            UiAnchor::CENTER,
            Color::blue,
            0);
		menuB_.text         = UiText(FontLoaded::LOBSTER,
            "MenuB",
            Vec2i::down * 220,
            UiAnchor::TOP,
            0,
            1.0f,
            Color::white);
		menuB_.buttonNext   = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(-400, 200),
            Vec2u(200, 100),
            UiAnchor::BOTTOM_RIGHT,
            Color::white,
            0);
		menuB_.buttonPrev   = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(400, 200),
            Vec2u(200, 100),
            UiAnchor::BOTTOM_LEFT,
            Color::white,
            0);
		menuB_.textNext     = UiText(FontLoaded::LOBSTER,
            "Next",
            Vec2i(-400, 200),
            UiAnchor::BOTTOM_RIGHT,
            0,
            1.0f,
            Color::white);
		menuB_.textPrevious = UiText(FontLoaded::LOBSTER,
			"Previous",
			Vec2i(400, 200),
			UiAnchor::BOTTOM_LEFT,
			0,
			1.0f,
			Color::white);

		// Register menu B
		uiManager.AddUiImage(&menuB_.backGround);
		uiManager.AddUiImage(&menuB_.buttonPrev);
		uiManager.AddUiImage(&menuB_.buttonNext);
		uiManager.AddUiText(&menuB_.text);
		uiManager.AddUiText(&menuB_.textNext);
		uiManager.AddUiText(&menuB_.textPrevious);

		// Menu C
		menuC_.backGround = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i::zero,
            config.windowSize * 0.9f,
            UiAnchor::CENTER,
            Color::green,
            0);
		menuC_.text       = UiText(FontLoaded::LOBSTER,
            "MenuC",
            Vec2i::down * 220,
            UiAnchor::TOP,
            0,
            1.0f,
            Color::white);
		menuC_.buttonNext   = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(-400, 200),
            Vec2u(200, 100),
            UiAnchor::BOTTOM_RIGHT,
            Color::white,
            0);
		menuC_.buttonPrev   = UiImage(GetSpritesFolderPath() + "water/Water01.jpg",
            Vec2i(400, 200),
            Vec2u(200, 100),
            UiAnchor::BOTTOM_LEFT,
            Color::white,
            0);
		menuC_.textNext     = UiText(FontLoaded::LOBSTER,
            "Next",
            Vec2i(-400, 200),
            UiAnchor::BOTTOM_RIGHT,
            0,
            1.0f,
            Color::white);
		menuC_.textPrevious = UiText(FontLoaded::LOBSTER,
			"Previous",
			Vec2i(400, 200),
			UiAnchor::BOTTOM_LEFT,
			0,
			1.0f,
			Color::white);

		// Register menu C
		uiManager.AddUiImage(&menuC_.backGround);
		uiManager.AddUiImage(&menuC_.buttonPrev);
		uiManager.AddUiImage(&menuC_.buttonNext);
		uiManager.AddUiText(&menuC_.text);
		uiManager.AddUiText(&menuC_.textNext);
		uiManager.AddUiText(&menuC_.textPrevious);

		currentMenu_ = &menuA_;
	}

	void Update(seconds dt) override
    {
		updateCount_ += dt.count();

		// Menu Switcher
		switch (currentMenuState_)
		{
			case MENU_A:
			{
				currentMenu_ = &menuA_;
				currentMenu_->SetEnable(true);
				menuB_.SetEnable(false);
				menuC_.SetEnable(false);
				if (sdl::InputLocator::get().GetKeyState(sdl::KeyCodeType::SPACE) ==
					sdl::ButtonState::DOWN)
				{
					if (left_) currentMenuState_ = MENU_C;
					else currentMenuState_ = MENU_B;
				}
			}
			break;
			case MENU_B:
			{
				currentMenu_ = &menuB_;
				currentMenu_->SetEnable(true);
				menuA_.SetEnable(false);
				menuC_.SetEnable(false);
				if (sdl::InputLocator::get().GetKeyState(sdl::KeyCodeType::SPACE) ==
					sdl::ButtonState::DOWN)
				{
					if (left_) currentMenuState_ = MENU_A;
					else currentMenuState_ = MENU_C;
				}
			}
			break;
			case MENU_C:
			{
				currentMenu_ = &menuC_;
				currentMenu_->SetEnable(true);
				menuB_.SetEnable(false);
				menuA_.SetEnable(false);
				if (sdl::InputLocator::get().GetKeyState(sdl::KeyCodeType::SPACE) ==
					sdl::ButtonState::DOWN)
				{
					if (left_) currentMenuState_ = MENU_B;
					else currentMenuState_ = MENU_A;
				}
			}
			break;
			default:;
		}

		if (left_)
		{
			// Left button
			currentMenu_->buttonPrev.SetColor(Color::white);
			currentMenu_->textPrevious.SetColor(Color::white);
			currentMenu_->buttonNext.SetColor(Color::grey);
			currentMenu_->textNext.SetColor(Color::grey);
			if (sdl::InputLocator::get().GetKeyState(sdl::KeyCodeType::RIGHT) ==
				sdl::ButtonState::DOWN)
			{
				left_ = false;
			}
		}
		else
		{
			// Right button
			currentMenu_->buttonPrev.SetColor(Color::grey);
			currentMenu_->textPrevious.SetColor(Color::grey);
			currentMenu_->buttonNext.SetColor(Color::white);
			currentMenu_->textNext.SetColor(Color::white);
			if (sdl::InputLocator::get().GetKeyState(sdl::KeyCodeType::LEFT) ==
				sdl::ButtonState::DOWN)
			{
				left_ = true;
			}
		}

		const auto& config = BasicEngine::GetInstance()->GetConfig();
		currentMenu_->backGround.SetSize(Vec2u(Vec2f(config.windowSize) * 0.9f));

		if (updateCount_ > kEngineDuration_) engine_.Stop();
	}

	void Render() override {}

	void Destroy() override {}

	void DrawImGui() override {}

private:
	float updateCount_           = 0;
	const float kEngineDuration_ = 2.0f;

	AerEngine& engine_;

	ComponentManagerContainer& cContainer_;

	enum MenuState
	{
		MENU_A,
		MENU_B,
		MENU_C
	};

	MenuState currentMenuState_ = MENU_A;
    Menu menuA_;
    Menu menuB_;
    Menu menuC_;
    Menu* currentMenu_;

    bool left_ = true;
};

TEST(UIManager, TestMenu)
{
	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1280, 720);
	config.flags      = Configuration::NONE;

	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::EDITOR);
#ifdef NEKO_GLES3
	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
#elif NEKO_VULKAN
	sdl::VulkanWindow window;
	vk::VkRenderer renderer(&window);
	renderer.SetRenderer(std::make_unique<neko::vk::RendererEditor>());
#endif

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();

	TestUiMenu testUiMenu(engine);
	engine.RegisterSystem(testUiMenu);
	engine.RegisterOnDrawUi(testUiMenu);

	engine.EngineLoop();
#ifdef EASY_PROFILE_USE
	profiler::dumpBlocksToFile("UiManager_Neko_Profile.prof");
#endif
}
}    // namespace neko::aer