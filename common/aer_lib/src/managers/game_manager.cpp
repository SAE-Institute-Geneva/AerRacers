#include <aer/managers/game_manager.h>

namespace neko::aer
{
    GameManager::GameManager(AerEngine& engine) : engine_(engine)
    {
        
    }

    void GameManager::Init()
    {
        game_state_ = GameState::WATING;
        playerPositionData = engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData();
    }

    void GameManager::Update(seconds dt)
    {
        switch (game_state_)
        {
        case GameState::WATING:
            game_state_ = GameState::RACING;
            break;
        case GameState::RACING:
            for (WaypointsCount element : playerPositionData->waypointsCount)
            {
                if (element > waypointsToWin)
                {
                    game_state_ = GameState::END;
                }
            }
            break;
        case GameState::END:
            break;
        }
    }

    void GameManager::Destroy()
    {

    }
}