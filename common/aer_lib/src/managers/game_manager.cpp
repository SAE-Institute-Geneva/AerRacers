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
    }

    void GameManager::Update(seconds dt)
    {
        switch (game_state_)
        {
        case GameState::WATING:
            break;
        case GameState::RACING:
            break;
        case GameState::END:
            break;
        }
    }

    void GameManager::SpawnPlayers()
    {

    }

    void GameManager::StartWPManager()
    {
        
    }

    void GameManager::StartCountDown()
    {
        
    }
       
    void GameManager::WaitForStart()
    {
        
    }

    void GameManager::StartTimer()
    {
        
    }


    void GameManager::UpdateGame()
    {
        
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