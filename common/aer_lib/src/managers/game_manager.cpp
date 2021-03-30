#include <aer/managers/game_manager.h>

namespace neko::aer
{
    GameManager::GameManager(AerEngine& engine) : engine_(engine)
    {
        
    }

    void GameManager::Init()
    {
        if (enableGameManager)
        {
            Camera3D* camera = GizmosLocator::get().GetCamera();
            camera->fovY = degree_t(80.0f);
            camera->nearPlane = 0.1f;
            camera->farPlane = 1'000'000.0f;
            engine_.GetCameras().SetCameras(*camera);
            const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
            engine_.GetComponentManagerContainer().sceneManager.LoadScene(
                config.dataRootPath +
                "scenes/WaypointTest.aerscene");
            SpawnPlayers();
            StartWPManager();
            StartCountDown();
            game_state_ = GameState::WATING;
        }
    }

    void GameManager::Update(seconds dt)
    {
        if (enableGameManager)
        {
            switch (game_state_)
            {
            case GameState::WATING:
                time -= dt;
                //TODO: Don't allow player to move
                WaitForStart();
                break;
            case GameState::RACING:
                time += dt;
                UpdateGame();
                break;
            case GameState::END:
                EndGame();
                break;
            }
        }
    }

    void GameManager::SpawnPlayers()
    {
        for (int i = 0; i < playerCount; i++)
        {
            engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i], EulerAngles(0,180,0));
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

        for (int i = 0; i < playerCount; i++)
        {
            if (engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->waypointsCount[i] > wpToFinish)
            {
                ShowEndScore(i);
                hasWin[i] = true;
            }
        }

        CheckIfEveryoneHasFinished();
    }

    void GameManager::CheckIfEveryoneHasFinished()
    {
        for (int i = 0; i < playerCount; i++)
        {
            if (hasWin[i] == false)
            {
                break;
            }
            else if (i == playerCount - 1)
            {
                game_state_ = GameState::END;
            }
        }
    }


    void GameManager::ShowEndScore(PlayerId player_id)
    {
        //engine_.GetCameras().GetCamera(player_id);

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