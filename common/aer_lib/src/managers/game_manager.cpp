#include <aer/managers/game_manager.h>

namespace neko::aer
{
    GameManager::GameManager(AerEngine& engine) : engine_(engine)
    {
        
    }

    void GameManager::Init()
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath +
            "scenes/WaypointTest.aerscene");
        gameManagerStarted = false;
    }

    void GameManager::StartGameManager()
    {
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->fovY = degree_t(80.0f);
        camera->nearPlane = 0.1f;
        camera->farPlane = 1'000'000.0f;
        engine_.GetCameras().SetCameras(*camera);
        SpawnPlayers();
        StartWPManager();
        StartCountDown();
        StartUi();
        gameManagerStarted = true;
        game_state_ = GameState::WATING;
    }

    void GameManager::Update(seconds dt)
    {
        if (gameManagerStarted)
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
        for (int i = 0; i < playerCount; i++)
        {
            middleTextUi[i].SetText("test");
            middleTextUi[i].SetScreenId(i);
        }
    }

    void GameManager::Render()
    {
        
    }


    void GameManager::SpawnPlayers()
    {
        for (int i = 0; i < playerCount; i++)
        {
            engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i], EulerAngles(0,180,0));
            engine_.GetComponentManagerContainer().playerManager.SetCanMove(i, false);
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
            UnFreezePlayers();
            game_state_ = GameState::RACING;
        }
    }

    void GameManager::StartTimer()
    {
        time = neko::seconds(0);
    }

    void GameManager::UnFreezePlayers()
    {
        for (int i = 0; i < playerCount; i++)
        {
            engine_.GetComponentManagerContainer().playerManager.SetCanMove(i, true);
        }
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

    void GameManager::StartUi()
    {
        auto& uiManager = UiManagerLocator::get();
        //globalText =
        for(int i = 0; i < playerCount; i++)
        {
            middleTextUi[i] = UiText(FontLoaded::LOBSTER, "Ready?", Vec2f(0.5f, 0.5f), UiAnchor::CENTER, i, 1.0f, Color::cyan);
            uiManager.AddUiText(&middleTextUi[i]);
        }

    }

    void GameManager::SetMiddleUiText(PlayerId player_id, std::string text)
    {
        
    }

    void GameManager::UpdateTimerUiText(PlayerId player_id)
    {
        
    }

    void GameManager::UpdateLapsUiText(PlayerId player_id)
    {
        
    }

    void GameManager::UpdatePlacementUiText(PlayerId player_id)
    {
        
    }


    void GameManager::Destroy()
    {

    }
}