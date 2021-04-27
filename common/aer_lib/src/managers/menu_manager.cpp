#include <aer/managers/menu_manager.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <aer/aer_engine.h>
namespace neko::aer
{
MenuManager::MenuManager(AerEngine& engine) : engine_(engine)
{
    
}

void MenuManager::Init()
{

    const auto& config = BasicEngine::GetInstance()->GetConfig();
    std::cout << config.windowSize << std::endl;
    startTextUi = UiText(FontLoaded::ROBOTO, "Start", Vec2i(Vec2f(0.0f, 1.0f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    optionsTextUi = UiText(FontLoaded::ROBOTO, "Options", Vec2i(Vec2f(0.0f, 0.75f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    highscoreTextUi = UiText(FontLoaded::ROBOTO, "HighScore", Vec2i(Vec2f(0.0f, 0.5f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    menuStatus_ = MenuStatus::SLEEP;

    useMenu = false;

    for (int i = 0; i < 4; i++)
    {
        joinUi_[i] = UiText(FontLoaded::ROBOTO, "Join", Vec2i((Vec2f(0.0f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        leftArrowUi_[i] = UiText(FontLoaded::ROBOTO, "<-", Vec2i((Vec2f(-0.2f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        rightArrowUi_[i] = UiText(FontLoaded::ROBOTO, "->", Vec2i((Vec2f(+0.2f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        // selectedShipUi_[i] = ;
        // selectedColorUi_[i] = ;
        colorBlueUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2i((Vec2f(0.0f,0.0f) + playerScreenOffsets[i])*Vec2f(config.windowSize)), Vec2u(100,100), UiAnchor::CENTER, 0, Color::blue);
        // colorGreenUi_[i] = ;
        // colorRedUi_[i] = ;
        // colorYellowUi_[i] = ;
        readyUi_[i] = UiText(FontLoaded::ROBOTO, "Ready", Vec2i((Vec2f(0.0f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
    }

    auto& uiManager = UiManagerLocator::get();
    uiManager.AddUiText(&startTextUi);
    startTextUi.SetEnable(false);
    uiManager.AddUiText(&optionsTextUi);
    optionsTextUi.SetEnable(false);
    uiManager.AddUiText(&highscoreTextUi);
    highscoreTextUi.SetEnable(false);
    for (int i = 0; i < 4; i++)
    {
        uiManager.AddUiText(&joinUi_[i]);
        joinUi_[i].SetEnable(false);
        uiManager.AddUiText(&leftArrowUi_[i]);
        leftArrowUi_[i].SetEnable(false);
        uiManager.AddUiText(&rightArrowUi_[i]);
        rightArrowUi_[i].SetEnable(false);
        uiManager.AddUiText(&readyUi_[i]);
        readyUi_[i].SetEnable(false);
        uiManager.AddUiImage(&colorBlueUi_[i]);
        colorBlueUi_[i].SetEnable(false);
    }
}

void MenuManager::Update(seconds dt)
{
    if (useMenu)
    {
        auto& inputlocator = sdl::InputLocator::get();

        startTextUi.SetEnable(false);
        optionsTextUi.SetEnable(false);
        highscoreTextUi.SetEnable(false);

        for (int i = 0; i < 4; i++)
        {
            joinUi_[i].SetEnable(false);
            leftArrowUi_[i].SetEnable(false);
            rightArrowUi_[i].SetEnable(false);
            // selectedShipUi_[i] = ;
            // selectedColorUi_[i] = ;
            colorBlueUi_[i].SetEnable(false);
            // colorGreenUi_[i] = ;
            // colorRedUi_[i] = ;
            // colorYellowUi_[i] = ;
            readyUi_[i].SetEnable(false);
        }


        switch (menuStatus_)
        {
        case MenuStatus::MENU:
            startTextUi.SetText("Start");
            optionsTextUi.SetText("Options");
            highscoreTextUi.SetText("HighScore");
            startTextUi.SetEnable(true);
            optionsTextUi.SetEnable(true);
            highscoreTextUi.SetEnable(true);
            switch (mainMenuPointing_)
            {
            case MainMenuPointing::START:
                startTextUi.SetColor(Color::white);
                optionsTextUi.SetColor(Color::grey);
                highscoreTextUi.SetColor(Color::grey);

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadDown)
                {
                    mainMenuPointing_ = MainMenuPointing::OPTIONS;
                    isDpadDown = true;
                }
                else if (isDpadDown && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadDown = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadUp)
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadUp = true;
                }
                else if (isDpadUp && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadUp = false;
                }

                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    menuStatus_ = MenuStatus::SELECTION;
                    for (int i = 0; i < 4; i++)
                    {
                        selectionPointing_[i] = SelectionPointing::JOIN;
                    }
                    selectionPointing_[0] = SelectionPointing::SHIP_TYPE;
                }

                break;
            case MainMenuPointing::HIGH_SCORE:
                startTextUi.SetColor(Color::grey);
                optionsTextUi.SetColor(Color::grey);
                highscoreTextUi.SetColor(Color::white);
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadDown)
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadDown = true;
                }
                else if (isDpadDown && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadDown = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadUp)
                {
                    mainMenuPointing_ = MainMenuPointing::OPTIONS;
                    isDpadUp = true;
                }
                else if (isDpadUp && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadUp = false;
                }

                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    menuStatus_ = MenuStatus::HIGHSCORE;
                }

                break;
            case MainMenuPointing::OPTIONS:
                startTextUi.SetColor(Color::grey);
                optionsTextUi.SetColor(Color::white);
                highscoreTextUi.SetColor(Color::grey);
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadDown)
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadDown = true;
                }
                else if (isDpadDown && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadDown = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadUp)
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadUp = true;
                }
                else if (isDpadUp && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadUp = false;
                }

                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    menuStatus_ = MenuStatus::OPTIONS;
                }

                break;
            case MainMenuPointing::SLEEP:
                break;
            }
            break;
        case MenuStatus::OPTIONS:
            if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
            {
                menuStatus_ = MenuStatus::MENU;
            }
            break;
        case MenuStatus::HIGHSCORE:
            if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
            {
                menuStatus_ = MenuStatus::MENU;
            }
            break;
        case MenuStatus::SELECTION:
            for (int i = 0; i < 4; i++)
            {
                switch (selectionPointing_[i])
                {
                case SelectionPointing::JOIN:
                    joinUi_[i].SetText("Press A to Join");
                    joinUi_[i].SetEnable(true);

                    break;
                case SelectionPointing::SHIP_TYPE:
                    leftArrowUi_[i].SetText("<-");
                    leftArrowUi_[i].SetEnable(true);
                    rightArrowUi_[i].SetText("->");
                    rightArrowUi_[i].SetEnable(true);
                    if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN && i == 0)
                    {
                        menuStatus_ = MenuStatus::MENU;
                    }
                    else if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::JOIN;
                    }
                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::SHIP_COLOR;
                    }
                    break;
                case SelectionPointing::SHIP_COLOR:
                    colorBlueUi_[i].SetColor(Color::blue);
                    colorBlueUi_[i].SetEnable(true);

                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::SHIP_TYPE;
                    }
                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::READY;
                    }
                    break;
                case SelectionPointing::READY:
                    readyUi_[i].SetText("Ready");
                    readyUi_[i].SetEnable(true);
                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::SHIP_COLOR;
                    }
                    if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN && i == 0)
                    {
                        bool everyoneIsReady = true;
                        for (int i = 0; i < 4; i++)
                        {
                            if (selectionPointing_[i] != SelectionPointing::READY && selectionPointing_[i] != SelectionPointing::JOIN)
                            {
                                everyoneIsReady = false;
                            }
                        }
                        if (everyoneIsReady)
                        {
                            int playerCount = 0;
                            for (int i = 0; i < 4; i++)
                            {
                                if (selectionPointing_[i] == SelectionPointing::READY)
                                {
                                    playerCount++;
                                }
                            }
                            engine_.GetComponentManagerContainer().gameManager.StartGameManager(2);
                            menuStatus_ = MenuStatus::SLEEP;
                        }
                    }
                    break;
                }
            }
            break;
        case MenuStatus::SLEEP:
            break;
        }
    }
}

void MenuManager::Render()
{

}

void MenuManager::Destroy()
{
    
}



void MenuManager::StartMenu()
{
    menuStatus_ = MenuStatus::MENU;
    mainMenuPointing_ = MainMenuPointing::START;

    useMenu = true;
}

void MenuManager::PressStartButton()
{
    
}

void MenuManager::PressOptionsButton()
{
    
}

void MenuManager::PressHighscoreButton()
{
    
}

// void MenuManager::PlayerSelection(PlayerId playerId)
// {
//     
// }

void MenuManager::PlayerJoin()
{
    
}

void MenuManager::BeginGame()
{
    
}

void MenuManager::SaveSoundOptions()
{
    
}


}