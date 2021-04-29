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
    enum class SelectedModel;
    

    enum GameState
    {
        WATING,
        RACING,
        END
    };

struct VictoryData
{
    int index;
    float time;
};

    class GameManager : public SystemInterface, public RenderCommandInterface
    {
    public:
        GameManager(AerEngine& engine);
        void Init() override;
        void StartGameManager(int currentPlayerCount);
        void StartGameManager(int currentPlayerCount, std::array<neko::aer::SelectedModel, 4> selected_models);
        void Update(seconds dt) override;
        void UpdateTime(seconds dt);
        void Render() override;
        void SpawnPlayers();
        void SpawnPlayers(std::array<SelectedModel, 4> selected_models);
        void StartWPManager();
        void StartCountDown();
        void WaitForStart();
        void StartTimer();
        void UnFreezePlayers();
        void UpdateGame();
        void ShowEndScore(PlayerId player_id);
        void EndGame();
        void RestartGame();

        void StartUi();
        void SetMiddleUiText(PlayerId player_id, std::string text);
        void UpdateTimerUiText();
        void UpdateLapsUiText();
        void UpdatePlacementUiText();
        void CheckIfEveryoneHasFinished();

        void GoBackToMenu();

        void Destroy() override;
    private:
        AerEngine& engine_;

        //const int waypointsToWin = 108;
        GameState game_state_;
        PlayerPositionData* playerPositionData;
        
        neko::seconds time = neko::seconds(0);
        bool gameManagerStarted = false;
        bool hasPlayedStartSound = false;
        std::array<bool, 4> hasWin = {
            false,
            false,
            false,
            false
        };

        std::array<UiText, 4> middleTextUi;
        std::array<UiText, 4> timerUi_;
        std::array<UiText, 4> lapsUi_;
        std::array<UiText, 4> placementUi;
        std::array<UiText, 4> endGameText {
            UiText(FontLoaded::LOBSTER, "", Vec2i(0,0), UiAnchor::CENTER, 1, 1, Color::cyan),
            UiText(FontLoaded::LOBSTER, "", Vec2i(0,0), UiAnchor::CENTER, 2, 1, Color::cyan),
            UiText(FontLoaded::LOBSTER, "",Vec2i(0,0), UiAnchor::CENTER, 3, 1, Color::cyan),
            UiText(FontLoaded::LOBSTER, "", Vec2i(0,0), UiAnchor::CENTER, 4, 1, Color::cyan)
        };
        std::array<std::string, 4> positionsText{ "st", "nd", "rd", "th" };
        std::vector<VictoryData> victoryDatas;


        std::string placement1stPath_ = "sprites/ui/centered/1st.png";
        std::string placement2ndPath_ = "sprites/ui/centered/2nd.png";
        std::string placement3rdPath_ = "sprites/ui/centered/3rd.png";
        std::string placement4thPath_ = "sprites/ui/centered/4th.png";
        std::string lap1Path_ = "sprites/ui/centered/1outof3laps.png";
        std::string lap2Path_ = "sprites/ui/centered/2outof3laps.png";
        std::string lap3Path_ = "sprites/ui/centered/3outof3laps.png";
        std::string lapsBackgroundPath_ = "sprites/ui/centered/lap_nuages.png";
        std::string timeBackgroundPath_ = "sprites/ui/centered/timebag_background.png";


        
        std::array<UiImage, 4> placement1stInGameUI_;
        std::array<UiImage, 4> placement2ndInGameUI_;
        std::array<UiImage, 4> placement3rdInGameUI_;
        std::array<UiImage, 4> placement4thInGameUI_;
        std::array<UiImage, 4> lap1InGameUI_;
        std::array<UiImage, 4> lap2InGameUI_;
        std::array<UiImage, 4> lap3InGameUI_;
        std::array<UiImage, 4> lapsBackgroundInGameUI_;
        std::array<UiImage, 4> timeBackgroundGameUI_;

        UiText globalText;
        bool endedGame = false;

        const float startTimer = 10.0f;
        const float endTimer = 10.0f;
        const int wpToFinish = 29;
        const float placementSizeMultiplier = 0.3f;
        const float lapsSizeMultiplier = 0.3f;
        const float timeBackgroundMultiplier_ = 0.5f;
        int playerCount = 4;
        const float uiPositionMultiplier = 0.25f;

        const std::array<Vec3f, 4> spawns =
        {
            //Vec3f(206, 43, -481),
            //Vec3f(156, 43, -471),
            //Vec3f(106, 43, -461),
            //Vec3f(56, 43, -451)
            Vec3f(-650.0f, 1185.0f, -3000.0f),
            Vec3f(-700.0f, 1185.0f, -3000.0f),
            Vec3f(-750.0f, 1185.0f, -3000.0f),
            Vec3f(-800.0f, 1185.0f, -3000.0f)
        };

        Entity audioEntity = INVALID_ENTITY;
        //TODO: Sound MoveInMenu
        //TODO: Sound Validation
    };
}