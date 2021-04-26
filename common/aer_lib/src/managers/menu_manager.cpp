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
        Color4 colorBlue = Color::blue;
        Color4 colorGreen = Color::green;
        Color4 colorRed = Color::red;
        Color4 colorYellow = Color::yellow;
        colorBlue = Color4(colorBlue.x * 0.5f, colorBlue.y * 0.5f, colorBlue.z * 0.5f, 1.0);
        colorGreen = Color4(colorGreen.x * 0.5f, colorGreen.y * 0.5f, colorGreen.z * 0.5f, 1.0);
        colorRed = Color4(colorRed.x * 0.5f, colorRed.y * 0.5f, colorRed.z * 0.5f, 1.0);
        colorYellow = Color4(colorYellow.x * 0.5f, colorYellow.y * 0.5f, colorYellow.z * 0.5f, 1.0);

        colorBlueUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(-0.15f, -0.5f) + playerScreenOffsets[i], Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorBlue);
        colorGreenUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(-0.05f, -0.5f) + playerScreenOffsets[i], Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorGreen);
        colorRedUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(0.05f, -0.5f) + playerScreenOffsets[i], Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorRed);
        colorYellowUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2f(0.15f, -0.5f) + playerScreenOffsets[i], Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorYellow);
        readyUi_[i] = UiText(FontLoaded::ROBOTO, "Ready", Vec2f(0.0f, 0.0f) + playerScreenOffsets[i], UiAnchor::CENTER, 0, 2.0f, Color::white);
    }

    auto& uiManager = UiManagerLocator::get();
    uiManager.AddUiImage(&menuBackGroundUI);
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
        uiManager.AddUiImage(&colorGreenUi_[i]);
        uiManager.AddUiImage(&colorRedUi_[i]);
        uiManager.AddUiImage(&colorYellowUi_[i]);
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
            colorBlueUi_[i].SetColor(Color::clear);
            colorGreenUi_[i].SetColor(Color::clear);
            colorRedUi_[i].SetColor(Color::clear);
            colorYellowUi_[i].SetColor(Color::clear);
            readyUi_[i].SetText("");
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

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) >= 0.7 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::OPTIONS;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) < 0.7)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) <= -0.7 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) > -0.7)
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
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) >= 0.7 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) < 0.7)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) <= -0.7 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::OPTIONS;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) > -0.7)
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
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) >= 0.7 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) < 0.7)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) <= -0.7 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) > -0.7)
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
                colorBlueUi_[i].SetColor(Color4(Color::blue.x, Color::blue.y, Color::blue.z, 0.5f));
                colorGreenUi_[i].SetColor(Color4(Color::green.x, Color::green.y, Color::green.z, 0.5f));
                colorRedUi_[i].SetColor(Color4(Color::red.x, Color::red.y, Color::red.z, 0.5f));
                colorYellowUi_[i].SetColor(Color4(Color::yellow.x, Color::yellow.y, Color::yellow.z, 0.5f));


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


                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
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
                    else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                    {
                        isDpadLeft_[i] = false;
                    }

                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
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
                    else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                    {
                        isDpadRight_[i] = false;
                    }



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


                    // Color4 colorBlue = Color::blue;
                    // Color4 colorGreen = Color::green;
                    // Color4 colorRed = Color::red;
                    // Color4 colorYellow = Color::yellow;
                    // colorBlue = Color4(colorBlue.x * 0.5f, colorBlue.y * 0.5f, colorBlue.z * 0.5f, 1.0);
                    // colorGreen = Color4(colorGreen.x * 0.5f, colorGreen.y * 0.5f, colorGreen.z * 0.5f, 1.0);
                    // colorRed = Color4(colorRed.x * 0.5f, colorRed.y * 0.5f, colorRed.z * 0.5f, 1.0);
                    // colorYellow = Color4(colorYellow.x * 0.5f, colorYellow.y * 0.5f, colorYellow.z * 0.5f, 1.0);


                    colorBlueUi_[i].SetColor(Color4(Color::blue.x, Color::blue.y, Color::blue.z, 0.5f));
                    colorGreenUi_[i].SetColor(Color4(Color::green.x, Color::green.y, Color::green.z, 0.5f));
                    colorRedUi_[i].SetColor(Color4(Color::red.x, Color::red.y, Color::red.z, 0.5f));
                    colorYellowUi_[i].SetColor(Color4(Color::yellow.x, Color::yellow.y, Color::yellow.z, 0.5f));

                    switch (shipSkins[i].selectedShipColor)
                    {
                    case SelectedShipColor::BLUE:
                        colorBlueUi_[i].SetColor(Color::blue);
                        break;
                    case SelectedShipColor::GREEN:
                        colorGreenUi_[i].SetColor(Color::green);
                        break;
                    case SelectedShipColor::RED:
                        colorRedUi_[i].SetColor(Color::red);
                        break;
                    case SelectedShipColor::YELLOW:
                        colorYellowUi_[i].SetColor(Color::yellow);
                        break;
                    }

                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
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
                    else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                    {
                        isDpadLeft_[i] = false;
                    }

                    if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
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
                    else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                    {
                        isDpadRight_[i] = false;
                    }



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
                            menuBackGroundUI.SetEnable(false);
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