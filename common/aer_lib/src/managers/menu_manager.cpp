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
    startTextUi_ = UiText(FontLoaded::ROBOTO, "Start", Vec2i(Vec2f(0.0f, 1.0f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    optionsTextUi_ = UiText(FontLoaded::ROBOTO, "Options", Vec2i(Vec2f(0.0f, 0.75f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    highscoreTextUi_ = UiText(FontLoaded::ROBOTO, "Credits", Vec2i(Vec2f(0.0f, 0.5f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    menuBackGroundUI = UiImage(config.dataRootPath + "sprites/ui/background.png", Vec2i(Vec2f().zero), config.windowSize, UiAnchor::CENTER, 0, Color::white);
    menuBackGroundUI.SetEnable(false);
    menuStatus_ = MenuStatus::SLEEP;
    creditsStatus_ = CreditsStatus::LEADS;

    creditsSebUiText_ = UiText(FontLoaded::ROBOTO, creditsSebText_, Vec2i(Vec2f(-0.75, 0) * Vec2f(config.windowSize)), UiAnchor::CENTER,0, 2.0f, Color::white);
    creditsSimonUiText_ = UiText(FontLoaded::ROBOTO, creditsSimonText_, Vec2i(Vec2f(-0.75, 0) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);;
    creditsStephenUiText_ = UiText(FontLoaded::ROBOTO, creditsStephenText_, Vec2i(Vec2f(-0.75, 0) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);;
    creditsLucaUiText_ = UiText(FontLoaded::ROBOTO, creditsLucaText_, Vec2i(Vec2f(-0.75, 0) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);;

    useMenu = false;

    for (int i = 0; i < 4; i++)
    {
        joinUi_[i] = UiText(FontLoaded::ROBOTO, "Join", Vec2i((Vec2f(0.0f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        leftArrowUi_[i] = UiText(FontLoaded::ROBOTO, "<-", Vec2i((Vec2f(-0.2f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        rightArrowUi_[i] = UiText(FontLoaded::ROBOTO, "->", Vec2i((Vec2f(+0.2f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
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

        colorBlueUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2i((Vec2f(-0.15f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorBlue);
        colorGreenUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2i((Vec2f(-0.05f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorGreen);
        colorRedUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2i((Vec2f(0.05f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorRed);
        colorYellowUi_[i] = UiImage(config.dataRootPath + "sprites/ui/square.png", Vec2i((Vec2f(0.15f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorYellow);
        readyUi_[i] = UiText(FontLoaded::ROBOTO, "Ready", Vec2i((Vec2f(0.0f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);


        selectionBackgroundUI_[i] = UiImage(config.dataRootPath + blueprint1UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);
        readyBackground_[i] = UiImage(config.dataRootPath + ReadyBackgroundUIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);
        readyText_[i] = UiImage(config.dataRootPath + ReadyTextUIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);
        joinText_[i] = UiImage(config.dataRootPath + pressStartTextUIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);

        rosso1UI_[i] = UiImage(config.dataRootPath + Rosso1UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))),Vec2u(1920 * shipModelsUiMultiplier,1080 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        rosso2UI_[i] = UiImage(config.dataRootPath + Rosso2UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1920 * shipModelsUiMultiplier, 1080 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        rosso3UI_[i] = UiImage(config.dataRootPath + Rosso3UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1920 * shipModelsUiMultiplier, 1080 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        rosso4UI_[i] = UiImage(config.dataRootPath + Rosso4UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1920 * shipModelsUiMultiplier, 1080 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        shipSkins[i] = SelectedModel::ROSSO_1;
        //arrowsUi_[i] = UiImage(config.dataRootPath);
    }

    auto& uiManager = UiManagerLocator::get();
    uiManager.AddUiText(&creditsSebUiText_);
    uiManager.AddUiText(&creditsSimonUiText_);
    uiManager.AddUiText(&creditsStephenUiText_);
    uiManager.AddUiText(&creditsLucaUiText_);

    creditsSebUiText_.SetEnable(false);
    creditsSimonUiText_.SetEnable(false);
    creditsStephenUiText_.SetEnable(false);
    creditsLucaUiText_.SetEnable(false);

    uiManager.AddUiImage(&menuBackGroundUI);
    uiManager.AddUiText(&startTextUi_);
    startTextUi_.SetEnable(false);
    uiManager.AddUiText(&optionsTextUi_);
    optionsTextUi_.SetEnable(false);
    uiManager.AddUiText(&highscoreTextUi_);
    highscoreTextUi_.SetEnable(false);
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

        rosso1UI_[i].SetEnable(false);
        rosso2UI_[i].SetEnable(false);
        rosso3UI_[i].SetEnable(false);
        rosso4UI_[i].SetEnable(false);

        uiManager.AddUiImage(&selectionBackgroundUI_[i]);
        uiManager.AddUiImage(&readyBackground_[i]);
        uiManager.AddUiImage(&readyText_[i]);
        uiManager.AddUiImage(&joinText_[i]);

        uiManager.AddUiImage(&rosso1UI_[i]);
        uiManager.AddUiImage(&rosso2UI_[i]);
        uiManager.AddUiImage(&rosso3UI_[i]);
        uiManager.AddUiImage(&rosso4UI_[i]);

        readyBackground_[i].SetEnable(false);
        readyText_[i].SetEnable(false);
        joinText_[i].SetEnable(false);
        selectionBackgroundUI_[i].SetEnable(false);

        hasStartedSceneLoading = false;
    }
}

void MenuManager::Update(seconds dt)
{
    if (useMenu)
    {
        if (!hasStartedSceneLoading)
        {
            const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
            engine_.GetResourceManagerContainer().modelManager.LoadModel(config.dataRootPath + "models/terrain_collider_v3/terrain_collider_v3.obj");
            engine_.GetComponentManagerContainer().sceneManager.LoadScene(
                config.dataRootPath + "scenes/LevelDesign05-04WP.aerscene");
            Camera3D* camera = GizmosLocator::get().GetCamera();
            camera->fovY = degree_t(80.0f);
            camera->nearPlane = 0.1f;
            camera->farPlane = 1'000'000.0f;
            engine_.GetCameras().SetCameras(*camera);
            hasStartedSceneLoading = true;

            // const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
            // engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            //     config.dataRootPath + "scenes/LevelDesign05-04WP.aerscene");
            // Camera3D* camera = GizmosLocator::get().GetCamera();
            // camera->fovY = degree_t(80.0f);
            // camera->nearPlane = 0.1f;
            // camera->farPlane = 1'000'000.0f;
            // engine_.GetCameras().SetCameras(*camera);
        }


        auto& inputlocator = sdl::InputLocator::get();
        menuBackGroundUI.SetEnable(false);
        startTextUi_.SetEnable(false);
        optionsTextUi_.SetEnable(false);
        highscoreTextUi_.SetEnable(false);

        creditsSebUiText_.SetEnable(false);
        creditsSimonUiText_.SetEnable(false);
        creditsStephenUiText_.SetEnable(false);
        creditsLucaUiText_.SetEnable(false);

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

            readyBackground_[i].SetEnable(false);
            readyText_[i].SetEnable(false);
            joinText_[i].SetEnable(false);
            selectionBackgroundUI_[i].SetEnable(false);

            rosso1UI_[i].SetEnable(false);
            rosso2UI_[i].SetEnable(false);
            rosso3UI_[i].SetEnable(false);
            rosso4UI_[i].SetEnable(false);
        }


        switch (menuStatus_)
        {
        case MenuStatus::MENU:
            startTextUi_.SetText("Start");
            optionsTextUi_.SetText("Options");
            highscoreTextUi_.SetText("Credits");
            startTextUi_.SetEnable(true);
            optionsTextUi_.SetEnable(true);
            highscoreTextUi_.SetEnable(true);
            menuBackGroundUI.SetEnable(true);
            switch (mainMenuPointing_)
            {
            case MainMenuPointing::START:
                startTextUi_.SetColor(Color::white);
                optionsTextUi_.SetColor(Color::grey);
                highscoreTextUi_.SetColor(Color::grey);

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
                startTextUi_.SetColor(Color::grey);
                optionsTextUi_.SetColor(Color::grey);
                highscoreTextUi_.SetColor(Color::white);
                menuBackGroundUI.SetEnable(true);
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
                    menuStatus_ = MenuStatus::CREDITS;
                }

                break;
            case MainMenuPointing::OPTIONS:
                startTextUi_.SetColor(Color::grey);
                optionsTextUi_.SetColor(Color::white);
                highscoreTextUi_.SetColor(Color::grey);
                menuBackGroundUI.SetEnable(true);
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
        case MenuStatus::CREDITS:
            switch (creditsStatus_)
            {
            case CreditsStatus::LEADS:
                creditsSebUiText_.SetEnable(true);
                creditsSimonUiText_.SetEnable(true);
                creditsStephenUiText_.SetEnable(true);
                creditsLucaUiText_.SetEnable(true);
                menuBackGroundUI.SetEnable(true);
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::ARTS;
                }
                break;
            case CreditsStatus::ARTS:
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::AUDIO;
                }
                break;
            case CreditsStatus::AUDIO:
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::TOOLS;
                }
                break;
            case CreditsStatus::TOOLS:
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::EXTRAS;
                }
                break;
            case CreditsStatus::EXTRAS:
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::SPECIAL_THANKS;
                }
                break;
            case CreditsStatus::SPECIAL_THANKS:
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::LEADS;
                    menuStatus_ = MenuStatus::MENU;
                }
                break;
            }
            if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
            {
                creditsStatus_ = CreditsStatus::LEADS;
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
                selectionBackgroundUI_[i].SetEnable(true);


                switch (selectionPointing_[i])
                {
                case SelectionPointing::JOIN:
                    //joinUi_[i].SetText("Press A to Join");
                    //joinUi_[i].SetEnable(true);

                    joinText_[i].SetEnable(true);
                    break;
                case SelectionPointing::SHIP_TYPE:
                    leftArrowUi_[i].SetText("<-");
                    leftArrowUi_[i].SetEnable(true);
                    rightArrowUi_[i].SetText("->");

                    switch (shipSkins[i])
                    {
                    case SelectedModel::ROSSO_1:
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_2;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_4;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso1UI_[i].SetEnable(true);
                        break;
                    case SelectedModel::ROSSO_2:
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_3;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_1;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso2UI_[i].SetEnable(true);
                        break;
                    case SelectedModel::ROSSO_3:
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_4;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_2;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso3UI_[i].SetEnable(true);
                        break;
                    case SelectedModel::ROSSO_4:
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_1;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_3;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso4UI_[i].SetEnable(true);
                        break;
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
                        selectionPointing_[i] = SelectionPointing::READY;
                    }
                    break;
                case SelectionPointing::READY:
                    //readyUi_[i].SetText("Ready");
                    //readyUi_[i].SetEnable(true);

                    readyBackground_[i].SetEnable(true);
                    readyText_[i].SetEnable(true);

                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::SHIP_TYPE;
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
                            engine_.GetComponentManagerContainer().gameManager.StartGameManager(playerCount, shipSkins);
                            //engine_.GetComponentManagerContainer().gameManager.StartGameManager(2);
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
    menuBackGroundUI.SetEnable(true);

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