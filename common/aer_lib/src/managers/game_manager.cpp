#include <aer/managers/game_manager.h>

namespace neko::aer
{
    GameManager::GameManager(AerEngine& engine) : engine_(engine)
    {
        
    }

    void GameManager::Init()
    {
        game_state_ = GameState::WATING;
        SpawnPlayers();
        StartWPManager();
        StartCountDown();
    }

    void GameManager::Update(seconds dt)
    {
        switch (game_state_)
        {
        case GameState::WATING:
            time -= dt;
            WaitForStart();
            break;
        case GameState::RACING:
            time += dt;
            break;
        case GameState::END:
            break;
        }
    }

    void GameManager::SpawnPlayers()
    {
        for (int i = 0; i < playerCount; i++)
        {
            engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i]);
        }
    }

    void GameManager::StartWPManager()
    {
        engine_.GetComponentManagerContainer().waypointManager.StartDetection();
    }

    void GameManager::StartCountDown()
    {
        time = neko::seconds(startTimer);
    }
       
    void GameManager::WaitForStart()
    {
        if (time <= neko::seconds(0))
        {
            StartTimer();
            game_state_ = GameState::RACING;
        }
    }

    void GameManager::StartTimer()
    {
        time = neko::seconds(0);
    }


    void GameManager::UpdateGame()
    {
        for (PlayerId playerWPCount : engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->waypointsCount)
        {
            if (playerWPCount > waypointsToWin)
            {
                ShowEndScore(playerWPCount);
            }
        }
    }


    void GameManager::ShowEndScore(PlayerId player_id)
    {
        
    }

    void GameManager::EndGame()
    {
        
    }


    void GameManager::RestartGame()
    {
        
    }


    void GameManager::Destroy()
    {

    }
}