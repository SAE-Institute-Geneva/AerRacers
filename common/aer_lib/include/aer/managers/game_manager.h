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
#include <aer/aer_engine.h>

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

    class GameManager : public SystemInterface
    {
    public:
        GameManager(AerEngine& engine);
        void Init() override;
        void Update(seconds dt) override;
        void SpawnPlayers();
        void StartWPManager();
        void StartCountDown();
        void WaitForStart();
        void StartTimer();
        void UpdateGame();
        void ShowEndScore(PlayerId player_id);
        void EndGame();
        void RestartGame();
        void Destroy() override;
    private:
        AerEngine& engine_;
        const uint8_t waypointsToWin = 108;
        GameState game_state_;
        PlayerPositionData* playerPositionData;
    };
}