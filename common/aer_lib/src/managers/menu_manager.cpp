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
    startTextUi = UiText(FontLoaded::ROBOTO, "Start", Vec2f(0.0f, 1.0f), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    optionsTextUi = UiText(FontLoaded::ROBOTO, "Options", Vec2f(0.0f, 0.75f), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    highscoreTextUi = UiText(FontLoaded::ROBOTO, "HighScore", Vec2f(0.0f, 0.5f), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    menuStatus_ = MenuStatus::SLEEP;

    useMenu = false;

    for (int i = 0; i < 4; i++)
    {
        joinUi_[i] = UiText(FontLoaded::ROBOTO, "Join", Vec2f(0.0f, 0.0f) + playerScreenOffsets[i], UiAnchor::CENTER, 0, 2.0f, Color::white);
        leftArrowUi_[i] = UiText(FontLoaded::ROBOTO, "<-", Vec2f(-0.2f, 0.0f) + playerScreenOffsets[i], UiAnchor::CENTER, 0, 2.0f, Color::white);
        rightArrowUi_[i] = UiText(FontLoaded::ROBOTO, "->", Vec2f(+0.2f, 0.0f) + playerScreenOffsets[i], UiAnchor::CENTER, 0, 2.0f, Color::white);
        // selectedShipUi_[i] = ;
        // selectedColorUi_[i] = ;
        colorBlueUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(0.0f,0.0f) + playerScreenOffsets[i], Vec2u(100,100), UiAnchor::CENTER, 0, Color::blue);
        // colorGreenUi_[i] = ;
        // colorRedUi_[i] = ;
        // colorYellowUi_[i] = ;
        readyUi_[i] = UiText(FontLoaded::ROBOTO, "Ready", Vec2f(0.0f, 0.0f) + playerScreenOffsets[i], UiAnchor::CENTER, 0, 2.0f, Color::white);
    }
}

void MenuManager::Update(seconds dt)
{
    if (useMenu)
    {
        auto& inputlocator = sdl::InputLocator::get();
        auto& uiManager = UiManagerLocator::get();

        startTextUi.SetText("");
        optionsTextUi.SetText("");
        highscoreTextUi.SetText("");

        for (int i = 0; i < 4; i++)
        {
            joinUi_[i].SetText("");
            leftArrowUi_[i].SetText("");
            rightArrowUi_[i].SetText("");
            // selectedShipUi_[i] = ;
            // selectedColorUi_[i] = ;
            colorBlueUi_[i].SetColor(Color::clear);
            // colorGreenUi_[i] = ;
            // colorRedUi_[i] = ;
            // colorYellowUi_[i] = ;
            readyUi_[i].SetText("");
        }


        switch (menuStatus_)
        {
        case MenuStatus::MENU:
            startTextUi.SetText("Start");
            optionsTextUi.SetText("Options");
            highscoreTextUi.SetText("HighScore");
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

                    break;
                case SelectionPointing::SHIP_TYPE:
                    leftArrowUi_[i].SetText("<-");
                    rightArrowUi_[i].SetText("->");
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
                            engine_.GetComponentManagerContainer().gameManager.StartGameManager(playerCount);
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

        uiManager.AddUiText(&startTextUi);
        uiManager.AddUiText(&optionsTextUi);
        uiManager.AddUiText(&highscoreTextUi);
        for (int i = 0; i < 4; i++)
        {
            uiManager.AddUiText(&joinUi_[i]);
            uiManager.AddUiText(&leftArrowUi_[i]);
            uiManager.AddUiText(&rightArrowUi_[i]);
            uiManager.AddUiText(&readyUi_[i]);
            uiManager.AddUiImage(&colorBlueUi_[i]);
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