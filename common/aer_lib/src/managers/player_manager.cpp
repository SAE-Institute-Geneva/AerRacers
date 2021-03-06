
#include "aer/managers/player_manager.h"

#include "aer/managers/manager_container.h"
#include <aer/log.h>
#include "aer/aer_engine.h"

#include "engine/engine.h"
#include <aer/log.h>
#include "aer/aer_engine.h"
#include "engine/resource_locations.h"

namespace neko::aer
{
    PlayerManager::PlayerManager(ComponentManagerContainer& cContainer, AerEngine& engine)
        : modelManager_(engine.GetResourceManagerContainer().modelManager),
        cContainer_(cContainer),
        cameraControllerManager_(cContainer.cameraControllerManager),
        shipControllerManager_(cContainer.shipControllerManager),
        shipInputManager_(cContainer.shipInputManager),
        engine_(engine)
    {
        playerComponents_.resize(INIT_PLAYER_NMB, PlayerComponent());
    }

    PlayerId PlayerManager::CreatePlayer(Vec3f pos, bool cortese, std::uint16_t coloriIndex, EulerAngles euler)
    {
        if (playerCount_ >= INIT_PLAYER_NMB) {
            LogError("Max Player Create");
            return playerCount_ - 1;
        }
        const auto& config = BasicEngine::GetInstance()->GetConfig();
        cContainer_.sceneManager.AddTag("Ship");
        cContainer_.sceneManager.AddTag("Camera");

        //Player Hierarchy//
        //ShipEntity
            //ShipModel
                //ShipArt
                //LeftRotorAnchor
                    //LeftRotorModel
                //RightRotorAnchor
                    //RightRotorModel

        //ShipEntity
        Entity shipEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.entityManager.AddComponentType(shipEntity,
            EntityMask(ComponentType::PLAYER_COMPONENT));
        cContainer_.entityManager.SetEntityName(shipEntity, "ship");
        TagLocator::get().SetEntityTag(shipEntity, "Ship");
        cContainer_.transform3dManager.AddComponent(shipEntity);
        cContainer_.transform3dManager.SetGlobalPosition(shipEntity, pos);
        cContainer_.transform3dManager.SetGlobalRotation(shipEntity, euler);
        physics::RigidDynamicData rigidDynamic;
        rigidDynamic.useGravity               = false;
        rigidDynamic.colliderType             = physics::ColliderType::BOX;
        rigidDynamic.angularDamping           = 3.0f;
        rigidDynamic.freezeRotation           = Vec3<bool>(true, false, true);
        rigidDynamic.boxColliderData.size     = Vec3f(9.0f, 1.0f, 5.5f);
        rigidDynamic.material.bounciness      = 0.0f;
        rigidDynamic.material.staticFriction  = 0.0f;
        rigidDynamic.material.dynamicFriction = 0.0f;
        cContainer_.rigidDynamicManager.AddRigidDynamic(shipEntity, rigidDynamic);

        PlayerComponent playerComponent;
        playerComponent.shipEntity   = shipEntity;
        playerComponent.playerNumber = playerComponents_.size();
        playerComponent.playerSpawn  = pos;


        //ShipModel
        Entity shipModelEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipModelEntity);
        cContainer_.entityManager.SetEntityParent(shipModelEntity, shipEntity);
        cContainer_.transform3dManager.SetRelativeScale(shipModelEntity, Vec3f::one * 0.2f);
        cContainer_.transform3dManager.SetRelativePosition(shipModelEntity, Vec3f::zero);
        
        if (cortese) {

            //ShipArt
            Entity shipArtEntity = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipArtEntity);
            cContainer_.entityManager.SetEntityParent(shipArtEntity, shipModelEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipArtEntity, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipArtEntity, Vec3f::one);
            cContainer_.renderManager.AddComponent(shipArtEntity);
            cContainer_.renderManager.SetModel(shipArtEntity,
                shipModels_[coloriIndex]);

            //ShipArt
            Entity shipArtDetailEntity = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipArtDetailEntity);
            cContainer_.entityManager.SetEntityParent(shipArtDetailEntity, shipArtEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipArtDetailEntity, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipArtDetailEntity, Vec3f::one);
            cContainer_.renderManager.AddComponent(shipArtDetailEntity);
            cContainer_.renderManager.SetModel(shipArtDetailEntity,
                config.dataRootPath +
                "models/ship/cortese/details/low_cortese_elements_resize.obj");

            //RightRotorAnchor
            Entity shipRightRotorAnchor = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipRightRotorAnchor);
            cContainer_.entityManager.SetEntityParent(shipRightRotorAnchor, shipModelEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipRightRotorAnchor,
                Vec3f(-16.75f, 0, 4));
            cContainer_.transform3dManager.SetRelativeScale(shipRightRotorAnchor, Vec3f::one);

            //RightRotorModel
            Entity shipRightRotorModel = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipRightRotorModel);
            cContainer_.entityManager.SetEntityParent(shipRightRotorModel, shipRightRotorAnchor);
            cContainer_.transform3dManager.SetRelativePosition(shipRightRotorModel, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipRightRotorModel, Vec3f::one);
            cContainer_.transform3dManager.SetRelativeRotation(shipRightRotorModel,
                EulerAngles(degree_t(0), degree_t(0), degree_t(0)));
            cContainer_.renderManager.AddComponent(shipRightRotorModel);
            cContainer_.renderManager.SetModel(shipRightRotorModel,
                config.dataRootPath +
                "models/ship/cortese/helice_d/low_helice_d_origin_resize.obj");

            //LeftRotorAnchor
            Entity shipLeftRotorAnchor = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipLeftRotorAnchor);
            cContainer_.entityManager.SetEntityParent(shipLeftRotorAnchor, shipModelEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipLeftRotorAnchor,
                Vec3f(16.75f, 0, 4));
            cContainer_.transform3dManager.SetRelativeScale(shipLeftRotorAnchor, Vec3f::one);

            //LeftRotorModel
            Entity shipLeftRotorModel = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipLeftRotorModel);
            cContainer_.entityManager.SetEntityParent(shipLeftRotorModel, shipLeftRotorAnchor);
            cContainer_.transform3dManager.SetRelativePosition(shipLeftRotorModel, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipLeftRotorModel, Vec3f::one);
            cContainer_.renderManager.AddComponent(shipLeftRotorModel);
            cContainer_.renderManager.SetModel(shipLeftRotorModel,
                config.dataRootPath +
                "models/ship/cortese/helice_g/low_helice_g_origin_resize.obj");

            playerComponent.shipModelEntity  = shipModelEntity;
            playerComponent.rightRotorAnchor = shipRightRotorAnchor;
            playerComponent.rightRotorModel  = shipRightRotorModel;
            playerComponent.leftRotorAnchor  = shipLeftRotorAnchor;
            playerComponent.leftRotorModel   = shipLeftRotorModel;
        } else {

            //ShipArt
            Entity shipArtEntity = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipArtEntity);
            cContainer_.entityManager.SetEntityParent(shipArtEntity, shipModelEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipArtEntity, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipArtEntity, Vec3f::one);
            cContainer_.renderManager.AddComponent(shipArtEntity);
            cContainer_.renderManager.SetModel(shipArtEntity,
                shipModels_[coloriIndex + 2]);

            //ShipArt
            Entity shipArtDetailEntity = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipArtDetailEntity);
            cContainer_.entityManager.SetEntityParent(shipArtDetailEntity, shipArtEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipArtDetailEntity, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipArtDetailEntity, Vec3f::one);
            cContainer_.renderManager.AddComponent(shipArtDetailEntity);
            cContainer_.renderManager.SetModel(shipArtDetailEntity,
                config.dataRootPath + "models/ship/ilroso/details/details_low.obj");

            //RightRotorAnchor
            Entity shipRightRotorAnchor = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipRightRotorAnchor);
            cContainer_.entityManager.SetEntityParent(shipRightRotorAnchor, shipModelEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipRightRotorAnchor,
                Vec3f(-12.5f, 3.5f, -1.5f));
            cContainer_.transform3dManager.SetRelativeScale(shipRightRotorAnchor, Vec3f::one);

            //RightRotorModel
            Entity shipRightRotorModel = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipRightRotorModel);
            cContainer_.entityManager.SetEntityParent(shipRightRotorModel, shipRightRotorAnchor);
            cContainer_.transform3dManager.SetRelativePosition(shipRightRotorModel, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipRightRotorModel, Vec3f::one);
            cContainer_.renderManager.AddComponent(shipRightRotorModel);
            cContainer_.renderManager.SetModel(shipRightRotorModel,
                config.dataRootPath +
                "models/ship/ilroso/helice_d/helice_droit.obj");

            //LeftRotorAnchor
            Entity shipLeftRotorAnchor = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipLeftRotorAnchor);
            cContainer_.entityManager.SetEntityParent(shipLeftRotorAnchor, shipModelEntity);
            cContainer_.transform3dManager.SetRelativePosition(shipLeftRotorAnchor,
                Vec3f(12.5f, 3.5f, -1.5f));
            cContainer_.transform3dManager.SetRelativeScale(shipLeftRotorAnchor, Vec3f::one);

            //LeftRotorModel
            Entity shipLeftRotorModel = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipLeftRotorModel);
            cContainer_.entityManager.SetEntityParent(shipLeftRotorModel, shipLeftRotorAnchor);
            cContainer_.transform3dManager.SetRelativePosition(shipLeftRotorModel, Vec3f::zero);
            cContainer_.transform3dManager.SetRelativeScale(shipLeftRotorModel, Vec3f::one);
            cContainer_.renderManager.AddComponent(shipLeftRotorModel);
            cContainer_.renderManager.SetModel(shipLeftRotorModel,
                config.dataRootPath +
                "models/ship/ilroso/helice_g/helice_gauche.obj");

            playerComponent.shipModelEntity = shipModelEntity;
            playerComponent.rightRotorAnchor = shipRightRotorAnchor;
            playerComponent.rightRotorModel = shipRightRotorModel;
            playerComponent.leftRotorAnchor = shipLeftRotorAnchor;
            playerComponent.leftRotorModel = shipLeftRotorModel;
        }

        //Audio
        Entity audioEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(audioEntity);
        //cContainer_.entityManager.SetEntityParent(audioEntity, playerComponent.shipEntity);
        cContainer_.audioManager.AddComponent(audioEntity);
        cContainer_.audioManager.SetPlayOnWakeUp(audioEntity, false);
        cContainer_.audioManager.SetMaxDistance(audioEntity, 40.0f);
        cContainer_.audioManager.SetVolume(audioEntity, 50.0f);
        cContainer_.audioManager.Init();

        Entity motorAudioEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(motorAudioEntity);
        //cContainer_.entityManager.SetEntityParent(motorAudioEntity, playerComponent.shipEntity);
        cContainer_.audioManager.AddComponent(motorAudioEntity);
        cContainer_.audioManager.SetEventName(motorAudioEntity, "sfx/ship_engine");
        cContainer_.audioManager.SetPlayOnWakeUp(motorAudioEntity, false);
        cContainer_.audioManager.SetMaxDistance(motorAudioEntity, 40.0f);
        cContainer_.audioManager.SetVolume(motorAudioEntity, 20.0f);
        cContainer_.audioManager.Init();

        playerComponent.audioEntity = audioEntity;
        playerComponent.engineAudioEntity = motorAudioEntity;

        std::vector<sdl::JoystickId> controllers = sdl::InputLocator::get().GetControllerIdVector();
        if (controllers.size() > playerCount_)
            playerComponent.linkedJoystick = controllers[playerCount_];
        playerComponents_[playerCount_] = playerComponent;
        shipControllerManager_.InitComponent(playerCount_);

        playerCount_++;

        return playerComponent.playerNumber;
    }

    PlayerComponent PlayerManager::GetPlayerComponent(PlayerId playerId)
    {
        return playerComponents_[playerId];
    }


    Entity PlayerManager::GetShipEntity(PlayerId playerId)
    {
        return playerComponents_[playerId].shipEntity;
    }

    void PlayerManager::LoadShipModels()
    {
        if (!shipModels_.empty()) return;
        shipModels_.push_back(modelManager_.LoadModel(GetModelsFolderPath() + "ship/cortese/corps/blue/low_cortese_corps_resize.obj"));
        shipModels_.push_back(modelManager_.LoadModel(GetModelsFolderPath() + "ship/cortese/corps/red/low_cortese_corps_resize.obj"));
        shipModels_.push_back(modelManager_.LoadModel(GetModelsFolderPath() + "ship/ilroso/corps/blue/objet_central_low.obj"));
        shipModels_.push_back(modelManager_.LoadModel(GetModelsFolderPath() + "ship/ilroso/corps/greyred/objet_central_low.obj"));
        shipModels_.push_back(modelManager_.LoadModel(GetModelsFolderPath() + "ship/ilroso/corps/redblack/objet_central_low.obj"));
        shipModels_.push_back(modelManager_.LoadModel(GetModelsFolderPath() + "ship/ilroso/corps/redwhite/objet_central_low.obj"));
        modelManager_.LoadModel(GetModelsFolderPath() + "ship/ilroso/helice_g/helice_gauche.obj");
        modelManager_.LoadModel(GetModelsFolderPath() + "ship/ilroso/helice_d/helice_droit.obj");
        modelManager_.LoadModel(GetModelsFolderPath() + "ship/ilroso/details/details_low.obj");
        modelManager_.LoadModel(GetModelsFolderPath() + "ship/cortese/helice_g/low_helice_g_origin_resize.obj");
        modelManager_.LoadModel(GetModelsFolderPath() + "ship/cortese/helice_d/low_helice_d_origin_resize.obj");
        modelManager_.LoadModel(GetModelsFolderPath() + "ship/cortese/details/low_cortese_elements_resize.obj");
    }

    void PlayerManager::Update(seconds dt)
    {
        for (PlayerId playerId = 0; playerId < GetPlayerCount(); ++playerId) {
            std::vector<sdl::JoystickId> controllers = sdl::InputLocator::get().GetControllerIdVector();
            if (controllers.size() > playerId)
            {
                playerComponents_[playerId].linkedJoystick = controllers[playerId];
            }
            else if (!controllers.empty()) {
                playerComponents_[playerId].linkedJoystick = controllers[0];
            }
            else {
                playerComponents_[playerId].linkedJoystick = 0;
            }
            if (cContainer_.transform3dManager.GetGlobalPosition(playerComponents_[playerId].shipEntity).y < kYDespawnPosition_)
            {
                cContainer_.rigidDynamicManager.MovePosition(playerComponents_[playerId].shipEntity, playerComponents_[playerId].playerSpawn);
            }
        }
        
    }

Vec3f PlayerManager::GetPlayerPosition(PlayerId playerId)
{
    return cContainer_.transform3dManager.GetGlobalPosition(playerComponents_[playerId].shipEntity);
}
bool PlayerManager::GetCanMove(PlayerId playerId)
{
    return shipControllerManager_.GetCanMove(playerId);
}

void PlayerManager::SetCanMove(PlayerId playerId, bool value)
{
    shipControllerManager_.SetCanMove(playerId, value);
}

void PlayerManager::DeletePlayers()
{
    for (int i = 0; i < cContainer_.playerManager.playerCount_; i++) {
        SetCanMove(i, false);
        cContainer_.entityManager.DestroyEntity(GetShipEntity(i), true);
        cContainer_.audioManager.Stop(playerComponents_[i].audioEntity);
        cContainer_.audioManager.Stop(playerComponents_[i].engineAudioEntity);
        cContainer_.entityManager.DestroyEntity(playerComponents_[i].audioEntity, true);
        cContainer_.entityManager.DestroyEntity(playerComponents_[i].engineAudioEntity, true);
    }
    playerComponents_.clear();
    playerComponents_.resize(INIT_PLAYER_NMB, PlayerComponent());
    playerCount_ = 0;
}


void PlayerManager::Destroy() { }

void PlayerManager::Init()
    {
    }
}
