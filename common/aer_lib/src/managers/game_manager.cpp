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
        ComponentManagerContainer& cContainer = engine_.GetComponentManagerContainer();
        audioEntity_ = cContainer.entityManager.CreateEntity();
        cContainer.transform3dManager.AddComponent(audioEntity_);
        cContainer.audioManager.AddComponent(audioEntity_);
        cContainer.audioManager.SetPlayOnWakeUp(audioEntity_, false);
        cContainer.audioManager.SetEventName(audioEntity_, "sfx/menu_bleep");
        cContainer.audioManager.SetMaxDistance(audioEntity_, 40.0f);
        cContainer.audioManager.SetVolume(audioEntity_, 50.0f);
        if (engine_.GetMode() == ModeEnum::GAME)
        {
            musicEntity_ = cContainer.entityManager.CreateEntity();
            cContainer.transform3dManager.AddComponent(musicEntity_);
            cContainer.audioManager.AddComponent(musicEntity_);
            cContainer.audioManager.SetPlayOnWakeUp(musicEntity_, true);
            cContainer.audioManager.SetEventName(musicEntity_, "music/game");
            cContainer.audioManager.SetMaxDistance(musicEntity_, 40.0f);
            cContainer.audioManager.SetVolume(musicEntity_, 50.0f);
        }
        cContainer.audioManager.Init();
    }


    void GameManager::StartGameManager(int currentPlayerCount)
    {
        playerCount = currentPlayerCount;
        // Camera3D* camera = GizmosLocator::get().GetCamera();
        // camera->fovY = degree_t(80.0f);
        // camera->nearPlane = 0.1f;
        // camera->farPlane = 1'000'000.0f;
        // engine_.GetCameras().SetCameras(*camera);
        SpawnPlayers();
        StartWPManager();
        StartCountDown();
        StartUi();
        gameManagerStarted = true;
        game_state_ = GameState::WATING;
    }

    void GameManager::StartGameManager(int currentPlayerCount, std::array<SelectedModel, 4> selected_models)
    {
        playerCount = currentPlayerCount;
        // const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        // engine_.GetComponentManagerContainer().sceneManager.LoadScene(
        //     config.dataRootPath + "scenes/LevelDesign05-04WP.aerscene");
        SpawnPlayers(selected_models);
        StartWPManager();
        StartCountDown();
        StartUi();
        gameManagerStarted = true;
        game_state_ = GameState::WATING;
        for (int i = 0; i < playerCount; i++)
        {
            hasWin[i] = false;
        }
    }

    void GameManager::Update(seconds dt)
    {
        if (gameManagerStarted)
        {
            if (gameManagerStarted)
            {
                ComponentManagerContainer& cContainer = engine_.GetComponentManagerContainer();

                for (int i = 0; i < playerCount; i++)
                {
                    timeBackgroundGameUI_[i].SetEnable(false);
                }
                switch (game_state_)
                {
                case GameState::WATING:
                    time -= dt;
                    //TODO: Don't allow player to move
                    WaitForStart();
                    for (int i = 0; i < playerCount; i++)
                    {
                        timeBackgroundGameUI_[i].SetEnable(true);
                    }
                    break;
                case GameState::RACING:
                    if (!hasPlayedStartSound) {
                        hasPlayedStartSound = true; cContainer.audioManager.Play(audioEntity_);
                    }
                    if (time.count() > 3.0f)
                    {
                        for (int i = 0; i < playerCount; i++)
                        {
                            SetMiddleUiText(i, "");
                        }
                    }
                    time += dt;
                    UpdateGame();
                    UpdateTimerUiText();
                    break;
                case GameState::END:
                    EndGame();
                    time += dt;
                    break;
                }
            }
            UpdateLapsUiText();
            UpdatePlacementUiText();
            //Todo: delete
            //auto& inputlocator = sdl::InputLocator::get();
            // if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
            // {
            //     GoBackToMenu();
            // }
        }
    }

    void GameManager::Render()
    {
        
    }

    void GameManager::SpawnPlayers()
    {
        for (int i = 0; i < playerCount; i++)
        {
            engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i], false, 0, EulerAngles(0, 180, 0));
        }
    }

    void GameManager::SpawnPlayers(std::array<SelectedModel, 4> selected_models)
    {
        for (int i = 0; i < playerCount; i++)
        {
            switch (selected_models[i])
            {
            case SelectedModel::ROSSO_1:
                engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i], false, 3, EulerAngles(0, 180, 0));
                break;
            case SelectedModel::ROSSO_2:
                engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i], false, 2, EulerAngles(0, 180, 0));
                break;
            case SelectedModel::ROSSO_3:
                engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i], false, 1, EulerAngles(0, 180, 0));
                break;
            case SelectedModel::ROSSO_4:
                engine_.GetComponentManagerContainer().playerManager.CreatePlayer(spawns[i], false, 0, EulerAngles(0, 180, 0));
                break;
            }
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
        if (!endedGame)
        {
            time = neko::seconds(0);
            endedGame = true;
        }
        for (int i = 0; i < playerCount; i++)
        {
            middleTextUi[i].SetText("");
            endGameText[i].SetText(std::to_string(i + 1) + positionsText[i] + ": Player " + std::to_string(victoryDatas[i].index + 1) + " (Time: " + fmt::format("{:.2f}", victoryDatas[i].time) + ")");
        }
        if (endedGame && time.count() > 5.0f)
        {
            endedGame = false;
            GoBackToMenu();
        }
    }


    void GameManager::RestartGame()
    {
        
    }

    void GameManager::StartUi()
    {
        const auto& config = BasicEngine::GetInstance()->GetConfig();
        auto& uiManager = UiManagerLocator::get();
        for(int i = 0; i < playerCount; i++)
        {
            middleTextUi[i] = UiText(FontLoaded::LOBSTER, "Ready?", Vec2i(Vec2f(0.0f, 0.0f) * Vec2f(config.windowSize)), UiAnchor::CENTER, i + 1, 4.0f, Color::white);
            timerUi_[i] = UiText(FontLoaded::LOBSTER, fmt::format("{:.2f}", neko::seconds(0).count()), Vec2i(Vec2f(1.5f, -1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), UiAnchor::TOP_LEFT, i + 1, 2.0f, Color::white);
            lapsUi_[i] = UiText(FontLoaded::LOBSTER, "0/0", Vec2i(Vec2f(-1.0f, -1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), UiAnchor::TOP_RIGHT, i + 1, 2.0f, Color::white);;
            placementUi[i] = UiText(FontLoaded::LOBSTER, "0th", Vec2i(Vec2f(-1.0f, 1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), UiAnchor::BOTTOM_RIGHT, i + 1, 2.0f, Color::white);
            placement1stInGameUI_[i].SetEnable(false);
            placement2ndInGameUI_[i].SetEnable(false);
            placement3rdInGameUI_[i].SetEnable(false);
            placement4thInGameUI_[i].SetEnable(false);
            lap1InGameUI_[i].SetEnable(false);
            lap2InGameUI_[i].SetEnable(false);
            lap3InGameUI_[i].SetEnable(false);
            lapsBackgroundInGameUI_[i].SetEnable(false);
            timeBackgroundGameUI_[i].SetEnable(false);

            placement1stInGameUI_[i] = UiImage(config.dataRootPath + placement1stPath_, Vec2i(Vec2f(-1.0f, 1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(516 * placementSizeMultiplier,492 * placementSizeMultiplier), UiAnchor::BOTTOM_RIGHT, i + 1, Color::white);
            placement2ndInGameUI_[i] = UiImage(config.dataRootPath + placement2ndPath_, Vec2i(Vec2f(-1.0f, 1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(672 * placementSizeMultiplier,552 * placementSizeMultiplier), UiAnchor::BOTTOM_RIGHT, i + 1, Color::white);
            placement3rdInGameUI_[i] = UiImage(config.dataRootPath + placement3rdPath_, Vec2i(Vec2f(-1.0f, 1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(642 * placementSizeMultiplier,527 * placementSizeMultiplier), UiAnchor::BOTTOM_RIGHT, i + 1, Color::white);
            placement4thInGameUI_[i] = UiImage(config.dataRootPath + placement4thPath_, Vec2i(Vec2f(-1.0f, 1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(600 * placementSizeMultiplier,528 * placementSizeMultiplier), UiAnchor::BOTTOM_RIGHT, i + 1, Color::white);
            lap1InGameUI_[i] = UiImage(config.dataRootPath + lap1Path_, Vec2i(Vec2f(-1.0f, -1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(426 * lapsSizeMultiplier, 408 * lapsSizeMultiplier), UiAnchor::TOP_RIGHT, i + 1, Color::white);
            lap2InGameUI_[i] = UiImage(config.dataRootPath + lap2Path_, Vec2i(Vec2f(-1.0f, -1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(510 * lapsSizeMultiplier,366 * lapsSizeMultiplier), UiAnchor::TOP_RIGHT, i + 1, Color::white);
            lap3InGameUI_[i] = UiImage(config.dataRootPath + lap3Path_, Vec2i(Vec2f(-1.0f, -1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(444 * lapsSizeMultiplier, 300 * lapsSizeMultiplier), UiAnchor::TOP_RIGHT, i + 1, Color::white);
            lapsBackgroundInGameUI_[i] = UiImage(config.dataRootPath + lapsBackgroundPath_, Vec2i(Vec2f(-1.0f, -1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(816 * lapsSizeMultiplier,324 * lapsSizeMultiplier), UiAnchor::TOP_RIGHT, i + 1, Color::white);
            timeBackgroundGameUI_[i] = UiImage(config.dataRootPath + timeBackgroundPath_, Vec2i(Vec2f(1.5f, -1.0f) * Vec2f(config.windowSize) * uiPositionMultiplier), Vec2u(780 * timeBackgroundMultiplier_,240 * timeBackgroundMultiplier_), UiAnchor::TOP_LEFT, i + 1, Color::white);

            placement1stInGameUI_[i].SetEnable(false);
            placement2ndInGameUI_[i].SetEnable(false);
            placement3rdInGameUI_[i].SetEnable(false);
            placement4thInGameUI_[i].SetEnable(false);
            lap1InGameUI_[i].SetEnable(false);
            lap2InGameUI_[i].SetEnable(false);
            lap3InGameUI_[i].SetEnable(false);
            lapsBackgroundInGameUI_[i].SetEnable(false);
            timeBackgroundGameUI_[i].SetEnable(false);

            uiManager.AddUiImage(&placement1stInGameUI_[i]);
            uiManager.AddUiImage(&placement2ndInGameUI_[i]);
            uiManager.AddUiImage(&placement3rdInGameUI_[i]);
            uiManager.AddUiImage(&placement4thInGameUI_[i]);
            uiManager.AddUiImage(&lapsBackgroundInGameUI_[i]);
            uiManager.AddUiImage(&lap1InGameUI_[i]);
            uiManager.AddUiImage(&lap2InGameUI_[i]);
            uiManager.AddUiImage(&lap3InGameUI_[i]);
            uiManager.AddUiImage(&timeBackgroundGameUI_[i]);

            // middleTextUi[i] = UiText(FontLoaded::LOBSTER, "Ready?", Vec2i((Vec2f(0.0f, 0.0f)) * Vec2f(config.windowSize)), UiAnchor::CENTER, i + 1, 4.0f, Color::cyan);
            // timerUi_[i] = UiText(FontLoaded::LOBSTER, fmt::format("{:.2f}", neko::seconds(0).count()), Vec2i((Vec2f(1.0f, -1.0f) * uiPositionMultiplier) * Vec2f(config.windowSize)), UiAnchor::TOP_LEFT, i + 1, 2.0f, Color::cyan);
            // lapsUi_[i] = UiText(FontLoaded::LOBSTER, "0/0", Vec2i((Vec2f(-1.0f, -1.0f) * uiPositionMultiplier) * Vec2f(config.windowSize)), UiAnchor::TOP_RIGHT, i + 1, 2.0f, Color::cyan);;
            // placementUi[i] = UiText(FontLoaded::LOBSTER, "0th", Vec2i((Vec2f(-1.0f, 1.0f) * uiPositionMultiplier) * Vec2f(config.windowSize)), UiAnchor::BOTTOM_RIGHT, i + 1, 2.0f, Color::cyan);;


            uiManager.AddUiText(&middleTextUi[i]);
            uiManager.AddUiText(&timerUi_[i]);
            //uiManager.AddUiText(&lapsUi_[i]);
            //uiManager.AddUiText(&placementUi[i]);
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
            timeBackgroundGameUI_[i].SetEnable(true);
            timerUi_[i].SetText(fmt::format("{:.2f}", time.count()));
        }

    }


    void GameManager::UpdateLapsUiText()
    {
        for (int i = 0; i < playerCount; i++)
        {
            lap1InGameUI_[i].SetEnable(false);
            lap2InGameUI_[i].SetEnable(false);
            lap3InGameUI_[i].SetEnable(false);
            if (engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->waypointsCount[i] > wpToFinish)
            {
                
            }
            else if (engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->waypointsCount[i] > wpToFinish * 2/3)
            {
                lapsBackgroundInGameUI_[i].SetEnable(true);
                lap3InGameUI_[i].SetEnable(true);
            }
            else if (engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->waypointsCount[i] > wpToFinish/3)
            {
                lapsBackgroundInGameUI_[i].SetEnable(true);
                lap2InGameUI_[i].SetEnable(true);
            }
            else
            {
                lapsBackgroundInGameUI_[i].SetEnable(true);
                lap1InGameUI_[i].SetEnable(true);
            }
        }
    }

    void GameManager::UpdatePlacementUiText()
    {
        for (int i = 0; i < playerCount; i++)
        {
            placement1stInGameUI_[i].SetEnable(false);
            placement2ndInGameUI_[i].SetEnable(false);
            placement3rdInGameUI_[i].SetEnable(false);
            placement4thInGameUI_[i].SetEnable(false);
            switch (engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->racePlacement[i])
            {
            case 1:
                placement1stInGameUI_[i].SetEnable(true);
                break;
            case 2:
                placement2ndInGameUI_[i].SetEnable(true);
                break;
            case 3:
                placement3rdInGameUI_[i].SetEnable(true);
                break;
            case 4:
                placement4thInGameUI_[i].SetEnable(true);
                break;
            }
            placementUi[i].SetText(std::to_string(engine_.GetComponentManagerContainer().waypointManager.GetPlayerPositionData()->racePlacement[i]));
        }
    }

    void GameManager::GoBackToMenu()
    {
        gameManagerStarted = false;
        engine_.GetComponentManagerContainer().playerManager.RespawnPlayers();
        for (int i = 0; i < playerCount; i++)
        {
            placement1stInGameUI_[i].SetEnable(false);
            placement2ndInGameUI_[i].SetEnable(false);
            placement3rdInGameUI_[i].SetEnable(false);
            placement4thInGameUI_[i].SetEnable(false);
            lap1InGameUI_[i].SetEnable(false);
            lap2InGameUI_[i].SetEnable(false);
            lap3InGameUI_[i].SetEnable(false);
            lapsBackgroundInGameUI_[i].SetEnable(false);
            timeBackgroundGameUI_[i].SetEnable(false);
            timerUi_[i].SetEnable(false);
            endGameText[i].SetEnable(false);
        }
        engine_.GetComponentManagerContainer().waypointManager;
        engine_.GetComponentManagerContainer().menuManager.StartMenu();
    }

    void GameManager::Destroy()
    {

    }
}