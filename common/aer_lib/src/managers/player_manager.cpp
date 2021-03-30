
#include "aer/managers/player_manager.h"
#include "aer/managers/manager_container.h"
#include "engine/engine.h"
#include <aer\log.h>

namespace neko::aer
{
    PlayerManager::PlayerManager(ComponentManagerContainer& cContainer)
        : cContainer_(cContainer),
        cameraControllerManager_(cContainer.cameraControllerManager),
        shipControllerManager_(cContainer.shipControllerManager),
        shipInputManager_(cContainer.shipInputManager)
    {
        playerComponents_.resize(INIT_PLAYER_NMB, PlayerComponent());
    }

    PlayerId PlayerManager::CreatePlayer(Vec3f pos, EulerAngles euler)
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
        cContainer_.entityManager.AddComponentType(shipEntity, EntityMask(ComponentType::PLAYER_COMPONENT));
        cContainer_.entityManager.SetEntityName(shipEntity, "ship");
        TagLocator::get().SetEntityTag(shipEntity, "Ship");
        cContainer_.transform3dManager.AddComponent(shipEntity);
        cContainer_.transform3dManager.SetGlobalPosition(shipEntity, pos);
        cContainer_.transform3dManager.SetGlobalRotation(shipEntity, euler);
        physics::RigidDynamicData rigidDynamic;
        rigidDynamic.useGravity = false;
        rigidDynamic.colliderType = physics::ColliderType::BOX;
        rigidDynamic.angularDamping = 3.0f;
        rigidDynamic.freezeRotation = Vec3<bool>(true, false, true);
        rigidDynamic.boxColliderData.size = Vec3f(9.0f, 1.0f, 5.5f);
        rigidDynamic.material.bounciness = 0.0f;
        rigidDynamic.material.staticFriction = 0.0f;
        rigidDynamic.material.dynamicFriction = 0.0f;
        cContainer_.rigidDynamicManager.AddRigidDynamic(shipEntity, rigidDynamic);
        shipControllerManager_.InitComponent(playerCount_);

        //ShipModel
        Entity shipModelEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipModelEntity);
        cContainer_.entityManager.SetEntityParent(shipModelEntity, shipEntity);
        cContainer_.transform3dManager.SetRelativeScale(shipModelEntity, Vec3f::one * 0.01f);
        cContainer_.transform3dManager.SetRelativePosition(shipModelEntity, Vec3f::zero);
        cContainer_.transform3dManager.SetRelativeRotation(shipModelEntity, EulerAngles(degree_t(0), degree_t(180), degree_t(0)));

        //ShipArt
        Entity shipArtEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipArtEntity);
        cContainer_.entityManager.SetEntityParent(shipArtEntity, shipModelEntity);
        cContainer_.transform3dManager.SetRelativePosition(shipArtEntity, Vec3f::zero);
        cContainer_.transform3dManager.SetRelativeScale(shipArtEntity, Vec3f::one);
        cContainer_.transform3dManager.SetRelativeRotation(shipModelEntity, EulerAngles(degree_t(0), degree_t(0), degree_t(0)));
        cContainer_.renderManager.AddComponent(shipArtEntity);
        cContainer_.renderManager.SetModel(shipArtEntity, config.dataRootPath + "models/ship/low_cortese_corps.obj");

        //RightRotorAnchor
        Entity shipRightRotorAnchor = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipRightRotorAnchor);
        cContainer_.entityManager.SetEntityParent(shipRightRotorAnchor, shipModelEntity);
        cContainer_.transform3dManager.SetRelativePosition(shipRightRotorAnchor, pos + Vec3f::left * 7);
        cContainer_.transform3dManager.SetGlobalScale(shipRightRotorAnchor, Vec3f(2, 1, 2));

        //RightRotorModel
        Entity shipRightRotorModel = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipRightRotorModel);
        cContainer_.entityManager.SetEntityParent(shipRightRotorModel, shipModelEntity);
        cContainer_.transform3dManager.SetRelativePosition(shipRightRotorModel, Vec3f::zero);
        cContainer_.transform3dManager.SetRelativeScale(shipRightRotorModel, Vec3f::one);
        cContainer_.transform3dManager.SetRelativeRotation(shipModelEntity, EulerAngles(degree_t(0), degree_t(0), degree_t(0)));
        cContainer_.renderManager.AddComponent(shipRightRotorModel);
        cContainer_.renderManager.SetModel(shipRightRotorModel, config.dataRootPath + "models/ship/low_helice_g.obj");

        //LeftRotorAnchor
        Entity shipLeftRotorAnchor = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipLeftRotorAnchor);
        cContainer_.entityManager.SetEntityParent(shipLeftRotorAnchor, shipModelEntity);
        cContainer_.transform3dManager.SetRelativePosition(shipLeftRotorAnchor, pos + Vec3f::right * 7);
        cContainer_.transform3dManager.SetGlobalScale(shipLeftRotorAnchor, Vec3f(2, 1, 2));

        //LeftRotorModel
        Entity shipLeftRotorModel = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipLeftRotorModel);
        cContainer_.entityManager.SetEntityParent(shipLeftRotorModel, shipModelEntity);
        cContainer_.transform3dManager.SetRelativePosition(shipLeftRotorModel, Vec3f::zero);
        cContainer_.transform3dManager.SetRelativeScale(shipLeftRotorModel, Vec3f::one);
        cContainer_.transform3dManager.SetRelativeRotation(shipModelEntity, EulerAngles(degree_t(0), degree_t(0), degree_t(0)));
        cContainer_.renderManager.AddComponent(shipLeftRotorModel);
        cContainer_.renderManager.SetModel(shipLeftRotorModel, config.dataRootPath + "models/ship/low_helice_d.obj");

        PlayerComponent playerComponent;
        playerComponent.shipEntity = shipEntity;
        playerComponent.shipModelEntity = shipModelEntity;
        playerComponent.rightRotorAnchor = shipRightRotorAnchor;
        playerComponent.rightRotorModel = shipRightRotorModel;
        playerComponent.leftRotorAnchor = shipLeftRotorAnchor;
        playerComponent.leftRotorModel = shipLeftRotorModel;
        playerComponent.playerNumber = playerComponents_.size();
        playerComponent.playerSpawn = pos;

        std::vector<sdl::ControllerId> controllers = sdl::InputLocator::get().GetControllerIdVector();
        if (controllers.size() > playerCount_)
            playerComponent.linkedJoystick = controllers[playerCount_];
        playerComponents_[playerCount_] = playerComponent;

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

    void PlayerManager::Init()
    {

    }

    void PlayerManager::Update(seconds dt)
    {
        for (PlayerId playerId = 0; playerId < GetPlayerCount(); ++playerId) {
            std::vector<sdl::ControllerId> controllers = sdl::InputLocator::get().GetControllerIdVector();
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


    void PlayerManager::Destroy()
    {

    }
}
