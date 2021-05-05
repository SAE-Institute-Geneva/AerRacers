#pragma once
/*
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

 Author : Feser Sebastien
 Co-Author :
 Date : 24.03.21
---------------------------------------------------------- */

#include <engine/entity.h>
#include <mathematics/vector.h>
#include <aer/ui/ui_manager.h>

namespace neko::aer
{
    class AerEngine;
    struct PlayerPositionData;


    enum class MenuStatus
    {
        MENU,
        OPTIONS,
        CREDITS,
        SELECTION,
        SLEEP

    };

    enum class MainMenuPointing
    {
        START,
        OPTIONS,
        HIGH_SCORE,
        SLEEP
    };

    enum class CreditsStatus
    {
        LEADS,
        ARTS,
        TOOLS,
        AUDIO,
        EXTRAS,
        SPECIAL_THANKS
    };

    enum class OptionsPointing
    {
        AUDIO,
        SLEEP
    };

    enum class SelectionPointing
    {
        JOIN,
        SHIP_TYPE,
        READY,
        SLEEP
    };


    // enum class SelectedShipColor
    // {
    //     BLUE,
    //     RED,
    //     GREEN,
    //     YELLOW
    // };

    // enum class SelectedShip
    // {
    //     ROSSO,
    //     CORTESE
    // };
    enum class SelectedModel
    {
        ROSSO_1,
        ROSSO_2,
        ROSSO_3,
        ROSSO_4
    };

    // struct ShipSkins
    // {
    //     SelectedShip selectedShip = SelectedShip::ROSSO;
    //     SelectedShipColor selectedShipColor = SelectedShipColor::BLUE;
    // };


    class MenuManager : public SystemInterface, public RenderCommandInterface
    {
    public:
        MenuManager(AerEngine& engine);

        void Init() override;
        void Update(seconds dt) override;
        void Render() override;
        void Destroy() override;

        //Start
        void StartMenu();

        //Main Menu
        void PressStartButton();
        void PressOptionsButton();
        void PressHighscoreButton();

        //Selection
        //void PlayerSelection(PlayerId playerId);
        void PlayerJoin();
        void BeginGame();

        //Options

        void SaveSoundOptions();

        //Score


        //Audio
        void PlaySelectSound();
        void PlayBipSound();
        
    private:
        const int kMaxPlayer_ = 4;

        AerEngine& engine_;
        MenuStatus menuStatus_ = MenuStatus::MENU;
        MainMenuPointing mainMenuPointing_ = MainMenuPointing::START;
        CreditsStatus creditsStatus_ = CreditsStatus::LEADS;
        std::array<SelectionPointing, 4> selectionPointing_;

        //Credits
        std::string creditsSebText_ = "Sébastien Feser \n Lead Project \n Lead Gameplay Programmer \n Lead Designer";
        std::string creditsSimonText_ = "Simon Canas \n Lead Tool \n Lead Engine Programmer";
        std::string creditsStephenText_ = "Stephen Grosjean \n Dev Ops \n Game Designer \n Gameplay Programmer";
        std::string creditsLucaText_ = "Luca Floreau \n Producer \n Lead Programmer \n Gameplay Programmer";

        UiText creditsSebUiText_;
        UiText creditsSimonUiText_;
        UiText creditsStephenUiText_;
        UiText creditsLucaUiText_;

        //ImagesDataString
        std::string menuBackgroundUIPath_ = "sprites/ui/centered/background.png";
        std::string menuPlayButtonUIPath_ = "sprites/ui/centered/play.png";
        std::string menuOptionsButtonUIPath_ = "sprites/ui/centered/options.png";
        std::string menuHighScoreUIPath_ = "sprites/ui/centered/highscore.png";
        std::string menuCloud1UIPath_ = "sprites/ui/centered/nuageshaut.png";
        std::string menuCloud2UIPath_ = "sprites/ui/centered/nuagesmilieu.png";
        std::string menuCloud3UIPath_ = "sprites/ui/centered/nuagesbas.png";
        std::string aerRacersTextUIPath_ = "sprites/ui/centered/logo.png";

        std::string blueprint1UIPath_ = "sprites/ui/centered/blueprint01.jpg";
        std::string blueprint2UIPath_ = "sprites/ui/centered/blueprint02.jpg";
        std::string pressStartTextUIPath_ = "sprites/ui/centered/PRESS_A.png";
        std::string Rosso1UIPath_ = "sprites/ui/centered/rosso_models/Il_Roso_001.png";
        std::string Rosso2UIPath_ = "sprites/ui/centered/rosso_models/Il_Roso_002.png";
        std::string Rosso3UIPath_ = "sprites/ui/centered/rosso_models/Il_Roso_003.png";
        std::string Rosso4UIPath_ = "sprites/ui/centered/rosso_models/Il_Roso_004.png";
        std::string ReadyBackgroundUIPath_ = "sprites/ui/centered/rectangleready.png";
        std::string ReadyTextUIPath_ = "sprites/ui/centered/READY.png";

        std::string placement1stUIPath_ = "sprites/ui/centered/1st.png";
        std::string placement2ndUIPath_ = "sprites/ui/centered/2nd.png";
        std::string placement3rdUIPath_ = "sprites/ui/centered/3rd.png";
        std::string placement4thUIPath_ = "sprites/ui/centered/4th.png";

        std::string lap1UIPath_ = "sprites/ui/centered/1outof3laps.png";
        std::string lap2UIPath_ = "sprites/ui/centered/2outof3laps.png";
        std::string lap3UIPath_ = "sprites/ui/centered/3outof3laps.png";
        std::string lapsBackgroundUIPath_ = "sprites/ui/centered/lap_nuages.png";
        std::string timeBackgroundUIPath_ = "sprites/ui/centered/timebag_background.png";

        //UI

        //Menu
        UiImage logoUi_;
        UiText startTextUi_;
        UiText optionsTextUi_;
        UiText highscoreTextUi_;

        UiText CreditsText1_;
        UiText CreditsText2_;
        UiText CreditsText3_;

        std::array<UiImage, 4> readyBackground_;
        std::array<UiImage, 4> readyText_;
        std::array<UiImage, 4> joinText_;

        std::array<UiImage, 4> placement1_;
        std::array<UiImage, 4> placement2_;
        std::array<UiImage, 4> placement3_;
        std::array<UiImage, 4> placement4_;

        std::array<UiImage, 4> lap1_;
        std::array<UiImage, 4> lap2_;
        std::array<UiImage, 4> lap3_;
        std::array<UiImage, 4> lapBackground_;
        std::array<UiImage, 4> timeBackground_;

        UiImage menuBackGroundUI;
        std::array<UiImage, 3> TextCloudImages;

        //Selection

        std::array<UiImage, 4> elRossoUI_;
        std::array<UiImage, 4> corteseUI_;

        std::array<UiImage, 4> selectionBackgroundUI_;
        std::array<UiImage, 4> arrowsUi_;
        std::array<UiImage, 4> leftArrowUI_;
        std::array<UiImage, 4> rightArrowUI_;

        std::array<UiImage, 4> colorRedUi_;
        std::array<UiImage, 4> colorGreenUi_;
        std::array<UiImage, 4> colorBlueUi_;
        std::array<UiImage, 4> colorYellowUi_;

        std::array<UiImage, 4> selectedColorUi_;

        std::array<SelectedModel, 4> shipSkins;

        std::array<UiText, 4> joinUi_;
        std::array<UiText, 4> leftArrowUi_;
        std::array<UiText, 4> rightArrowUi_;
        std::array<UiImage, 4> selectedShipUi_;
        std::array<UiText, 4> readyUi_;


        std::array<UiImage, 4> rosso1UI_;
        std::array<UiImage, 4> rosso2UI_;
        std::array<UiImage, 4> rosso3UI_;
        std::array<UiImage, 4> rosso4UI_;

        std::array<Vec2f, 4> playerScreenOffsets{Vec2f(-0.5, 0.5), Vec2f(0.5, 0.5), Vec2f(-0.5, -0.5), Vec2f(0.5, -0.5)};

        const float colorsSize = 50.0f;
        const float shipModelsUiMultiplier = 0.25f;


        bool useMenu = false;
        bool hasStartedSceneLoading = false;

        std::array<bool, 4> isDpadUp_{ false, false, false, false };
        std::array<bool, 4> isDpadDown_{ false, false, false, false };
        std::array<bool, 4> isDpadLeft_{ false, false, false, false };
        std::array<bool, 4> isDpadRight_{ false, false, false, false };

        Entity bipAudioEntity_ = INVALID_ENTITY;
        Entity selectAudioEntity_ = INVALID_ENTITY;

        MainMenuPointing lastMenuPointing_;
        MenuStatus lastMenuStatus_;
        std::array<SelectedModel, 4> lastSelectedSkins_;
    };
}