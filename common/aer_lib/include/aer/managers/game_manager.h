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
    

    enum GameState
    {
        WATING,
        RACING,
        END
    };

    class GameManager : public SystemInterface, public RenderCommandInterface
    {
    public:
        GameManager(AerEngine& engine);
        void Init() override;
        void StartGameManager(int currentPlayerCount);
        void Update(seconds dt) override;
        void Render() override;
        void SpawnPlayers();
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

        void Destroy() override;
    private:
        AerEngine& engine_;
        //const int waypointsToWin = 108;
        GameState game_state_;
        PlayerPositionData* playerPositionData;
        neko::seconds time = neko::seconds(0);
        bool gameManagerStarted = false;

        std::array<bool, 4> hasWin = {
            false,
            false,
            false,
            false
        };

        std::array<UiText, 4> middleTextUi;
        std::array<UiText, 4> TimerUi;
        std::array<UiText, 4> LapsUi;
        std::array<UiText, 4> placementUi;
        UiText globalText;

        const float startTimer = 10.0f;
        const float endTimer = 10.0f;
        const int wpToFinish = 33;
        int playerCount = 4;
        const float uiPositionMultiplier = 0.25f;

        const std::array<Vec3f, 4> spawns =
        {
            //Vec3f(206, 43, -481),
            //Vec3f(156, 43, -471),
            //Vec3f(106, 43, -461),
            //Vec3f(56, 43, -451)
            Vec3f(-1108.0f, 185.0f, -788.0f),
            Vec3f(-1128.0f, 185.0f, -788.0f),
            Vec3f(-1148.0f, 185.0f, -788.0f),
            Vec3f(-1168.0f, 185.0f, -788.0f)
        };
    };
}