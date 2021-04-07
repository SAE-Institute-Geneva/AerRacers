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

#include <engine\entity.h>
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
        HIGHSCORE,
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

    enum class OptionsPointing
    {
        AUDIO,
        SLEEP
    };

    enum class SelectionPointing
    {
        JOIN,
        SHIP_TYPE,
        SHIP_COLOR,
        READY,
        SLEEP
    };


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

        
    private:
        AerEngine& engine_;
        MenuStatus menuStatus_ = MenuStatus::MENU;
        MainMenuPointing mainMenuPointing_ = MainMenuPointing::START;
        std::array<SelectionPointing, 4> selectionPointing_;
        UiText startTextUi;
        UiText optionsTextUi;
        UiText highscoreTextUi;
        std::array<UiText, 4> joinUi_;
        std::array<UiText, 4> leftArrowUi_;
        std::array<UiText, 4> rightArrowUi_;
        std::array<UiImage, 4> selectedShipUi_;
        std::array<UiImage, 4> selectedColorUi_;
        std::array<UiImage, 4> colorRedUi_;
        std::array<UiImage, 4> colorGreenUi_;
        std::array<UiImage, 4> colorBlueUi_;
        std::array<UiImage, 4> colorYellowUi_;
        std::array<UiText, 4> readyUi_;


        std::array<Vec2f, 4> playerScreenOffsets{Vec2f(-0.5, 0.5), Vec2f(0.5, 0.5), Vec2f(-0.5, -0.5), Vec2f(0.5, -0.5)};


        bool isDpadLeft = false;
        bool isDpadRight = false;
        bool isDpadUp = false;
        bool isDpadDown = false;
    };
}