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
    menuBackGroundUI = UiImage(config.dataRootPath + "sprites/ui/background.png", Vec2f().zero, Vec2u(1920, 1080), UiAnchor::CENTER, 0, Color::white);
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
        colorGreenUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(0.0f, 0.0f) + playerScreenOffsets[i], Vec2u(100, 100), UiAnchor::CENTER, 0, Color::green);
        colorRedUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(0.0f, 0.0f) + playerScreenOffsets[i], Vec2u(100, 100), UiAnchor::CENTER, 0, Color::red);
        colorYellowUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(0.0f, 0.0f) + playerScreenOffsets[i], Vec2u(100, 100), UiAnchor::CENTER, 0, Color::yellow);
        readyUi_[i] = UiText(FontLoaded::ROBOTO, "Ready", Vec2f(0.0f, 0.0f) + playerScreenOffsets[i], UiAnchor::CENTER, 0, 2.0f, Color::white);
    }

    auto& uiManager = UiManagerLocator::get();
    uiManager.AddUiImage(&menuBackGroundUI);
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
        uiManager.AddUiImage(&colorGreenUi_[i]);
        uiManager.AddUiImage(&colorRedUi_[i]);
        uiManager.AddUiImage(&colorYellowUi_[i]);
    }
}

void MenuManager::Update(seconds dt)
{
    if (useMenu)
    {
        auto& inputlocator = sdl::InputLocator::get();

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
            colorGreenUi_[i].SetColor(Color::clear);
            colorRedUi_[i].SetColor(Color::clear);
            colorYellowUi_[i].SetColor(Color::clear);
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

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::OPTIONS;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadUp_[0] = false;
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
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::OPTIONS;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadUp_[0] = false;
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
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                {
                    isDpadUp_[0] = false;
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


                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadLeft_[i])
                    {
                        if (shipSkins[i].selectedShip == SelectedShip::ROSSO)
                        {
                            shipSkins[i].selectedShip = SelectedShip::CORTESE;
                        }
                        else
                        {
                            shipSkins[i].selectedShip = SelectedShip::ROSSO;
                        }
                        isDpadLeft_[i] = true;
                    }
                    else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                    {
                        isDpadLeft_[i] = false;
                    }

                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadRight_[i])
                    {
                        if (shipSkins[i].selectedShip == SelectedShip::ROSSO)
                        {
                            shipSkins[i].selectedShip = SelectedShip::CORTESE;
                        }
                        else
                        {
                            shipSkins[i].selectedShip = SelectedShip::ROSSO;
                        }
                        isDpadRight_[i] = true;
                    }
                    else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                    {
                        isDpadRight_[i] = false;
                    }



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
                    colorGreenUi_[i].SetColor(Color::green);
                    colorRedUi_[i].SetColor(Color::red);
                    colorYellowUi_[i].SetColor(Color::yellow);


                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == -1 && !isDpadLeft_[i])
                    {
                        isDpadLeft_[i] = true;
                        switch (shipSkins[i].selectedShipColor)
                        {
                        case SelectedShipColor::BLUE:
                            shipSkins[i].selectedShipColor = SelectedShipColor::YELLOW;
                            break;
                        case SelectedShipColor::GREEN:
                            shipSkins[i].selectedShipColor = SelectedShipColor::BLUE;
                            break;
                        case SelectedShipColor::RED:
                            shipSkins[i].selectedShipColor = SelectedShipColor::GREEN;
                            break;
                        case SelectedShipColor::YELLOW:
                            shipSkins[i].selectedShipColor = SelectedShipColor::RED;
                            break;
                        }
                    }
                    else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                    {
                        isDpadLeft_[i] = false;
                    }

                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 1 && !isDpadRight_[i])
                    {
                        isDpadRight_[i] = true;
                        switch (shipSkins[i].selectedShipColor)
                        {
                        case SelectedShipColor::BLUE:
                            shipSkins[i].selectedShipColor = SelectedShipColor::GREEN;
                            break;
                        case SelectedShipColor::GREEN:
                            shipSkins[i].selectedShipColor = SelectedShipColor::RED;
                            break;
                        case SelectedShipColor::RED:
                            shipSkins[i].selectedShipColor = SelectedShipColor::YELLOW;
                            break;
                        case SelectedShipColor::YELLOW:
                            shipSkins[i].selectedShipColor = SelectedShipColor::BLUE;
                            break;
                        }
                    }
                    else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::PAD_VERTICAL) == 0)
                    {
                        isDpadRight_[i] = false;
                    }




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