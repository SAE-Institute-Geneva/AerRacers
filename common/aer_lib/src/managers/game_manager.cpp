#include <aer/managers/game_manager.h>
#include <aer/aer_engine.h>


#include <aer/aer_engine.h>

namespace neko::aer
{
    GameManager::GameManager(AerEngine& engine) : engine_(engine)
    {
        
    }

    void GameManager::Init()
    {
        gameManagerStarted = false;
    }

    void GameManager::StartGameManager(int currentPlayerCount)
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath + "scenes/LevelDesign05-04WP.aerscene");
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->fovY = degree_t(80.0f);
        camera->nearPlane = 0.1f;
        camera->farPlane = 1'000'000.0f;
        engine_.GetCameras().SetCameras(*camera);
        playerCount = currentPlayerCount;
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
                    UpdateTimerUiText();
                    break;
                case GameState::END:
                    EndGame();
                    break;
                }
            }

            UpdateLapsUiText();
            UpdatePlacementUiText();
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
            for (int i = 0; i < playerCount; i++)
            {
                SetMiddleUiText(i, "GO!!!");
            }
        }
        else if (time <= neko::seconds(1))
        {
            for (int i = 0; i < playerCount; i++)
            {
                SetMiddleUiText(i, "1");
            }
        }
        else if (time <= neko::seconds(2))
        {
            for (int i = 0; i < playerCount; i++)
            {
                SetMiddleUiText(i, "2");
            }
        }
        else if (time <= neko::seconds(3))
        {
            for (int i = 0; i < playerCount; i++)
            {
                SetMiddleUiText(i, "3");
            }
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
                if (hasWin[i] == false)
                {
                    VictoryData victoryData = VictoryData();
                    victoryData.time = time.count();
                    victoryData.index = i;
                    victoryDatas.push_back(victoryData);
                }
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
        SetMiddleUiText(player_id, "GG! Position: " + std::to_string(engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->racePlacement[player_id]));
        engine_.GetComponentManagerContainer().playerManager.SetCanMove(player_id, false);

    }

    void GameManager::EndGame()
    {
        for (int i = 0; i < playerCount; i++)
        {
            middleTextUi[i].SetText("");
            endGameText[i].SetText(std::to_string(i + 1) + positionsText[i] + ": Player " + std::to_string(victoryDatas[i].index + 1) + " (Time: " + fmt::format("{:.2f}", victoryDatas[i].time) + ")");
        }
    }


    void GameManager::RestartGame()
    {
        
    }

    void GameManager::StartUi()
    {
        auto& uiManager = UiManagerLocator::get();
        for(int i = 0; i < playerCount; i++)
        {
            middleTextUi[i] = UiText(FontLoaded::LOBSTER, "Ready?", Vec2f(0.0f, 0.0f), UiAnchor::CENTER, i + 1, 4.0f, Color::cyan);
            TimerUi[i] = UiText(FontLoaded::LOBSTER, fmt::format("{:.2f}", neko::seconds(0).count()), Vec2f(1.0f, -1.0f) * uiPositionMultiplier, UiAnchor::TOP_LEFT, i + 1, 2.0f, Color::cyan);
            LapsUi[i] = UiText(FontLoaded::LOBSTER, "0/0", Vec2f(-1.0f, -1.0f) * uiPositionMultiplier, UiAnchor::TOP_RIGHT, i + 1, 2.0f, Color::cyan);;
            placementUi[i] = UiText(FontLoaded::LOBSTER, "0th", Vec2f(-1.0f, 1.0f) * uiPositionMultiplier, UiAnchor::BOTTOM_RIGHT, i + 1, 2.0f, Color::cyan);;

            uiManager.AddUiText(&middleTextUi[i]);
            uiManager.AddUiText(&TimerUi[i]);
            uiManager.AddUiText(&LapsUi[i]);
            uiManager.AddUiText(&placementUi[i]);
            uiManager.AddUiText(&endGameText[i]);
        }
    }

    void GameManager::SetMiddleUiText(PlayerId player_id, std::string text)
    {
        middleTextUi[player_id].SetText(text);
    }

    void GameManager::UpdateTimerUiText()
    {
        for (int i = 0; i < playerCount; i++)
        {
            TimerUi[i].SetText(fmt::format("{:.2f}", time.count()));
        }

    }

    void GameManager::UpdateLapsUiText()
    {
        for (int i = 0; i < playerCount; i++)
        {

        }
    }

    void GameManager::UpdatePlacementUiText()
    {
        for (int i = 0; i < playerCount; i++)
        {
            placementUi[i].SetText(std::to_string(engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->racePlacement[i]));
        }
    }


    void GameManager::Destroy()
    {

    }
}