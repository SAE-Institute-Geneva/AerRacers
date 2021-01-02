/* ----------------------------------------------------
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

 Author : Floreau Luca
 Co-Author :
 Date : 22.11.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>
#include "PxPhysicsAPI.h"
#include "PxConfig.h"
#include <log.h>


#include "aer_engine.h"
#include "physics_engine.h"
#include "physics_callbacks.h"
#include "engine/engine.h"
#include "engine/system.h"
#include "engine/transform.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"
#include "gl/shader.h"
#include "gl/shape.h"

class SceneInterface :
    public neko::DrawImGuiInterface,
    public neko::physics::OnCollisionInterface,
    public neko::physics::OnTriggerInterface,
    public neko::physics::FixedUpdateInterface {
protected:
    ~SceneInterface() = default;
public:
    virtual void InitActors(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        neko::physics::PhysicsEngine& physicsEngine) = 0;
    virtual void HasSucceed() = 0;
    virtual void Update() {}
    virtual void DrawImGui() override {}
    virtual void FixedUpdate(neko::seconds dt) override {}
    neko::Entity viewedEntity;
    int engineDuration = 1000;
protected:

    neko::EntityManager* entityManager_;
    neko::Transform3dManager* transform3dManager_;
    neko::physics::PhysicsEngine* physicsEngine_;
    neko::IGizmosRenderer* gizmosLocator_;
};

class SceneCubeFall final : public SceneInterface {
public :
    void InitActors(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration = 300;
        entityManager_ = &entityManager;
        transform3dManager_ = &transform3dManager;
        physicsEngine_ = &physicsEngine;
        gizmosLocator_ = &neko::GizmosLocator::get();
        //Plane
        {
            planeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(planeEntity_);
            transform3dManager_->SetPosition(planeEntity_, planePosition_);
            transform3dManager_->SetScale(planeEntity_, neko::Vec3f(5, 1, 5));
            neko::physics::RigidStaticData rigidStatic;
            rigidStatic.colliderType = neko::physics::ColliderType::BOX;
            rigidStatic.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            physicsEngine_->AddRigidStatic(
                planeEntity_,
                rigidStatic);
        }
        //Cube
        for (int i = 0; i < kCubeNumbers; ++i) {
            cubeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeEntity_);
            transform3dManager_->SetPosition(
                cubeEntity_,
                cubePosition_ + neko::Vec3f(0, i, 0));
            transform3dManager_->SetScale(cubeEntity_, neko::Vec3f::one / 4.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.mass = 1.0f;
            rigidDynamic.useGravity = true;
            rigidDynamic.freezeRotation = neko::Vec3<bool>(false);
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            rigidDynamic.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            physicsEngine_->AddRigidDynamic(
                cubeEntity_,
                rigidDynamic);
        }
        viewedEntity = cubeEntity_;
    }

    void HasSucceed() override
    {
        EXPECT_TRUE(true);
        EXPECT_TRUE(true);
    }

    void Update() override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::SPACE) ==
            neko::sdl::ButtonState::DOWN) {
            physicsEngine_->AddForce(
                cubeEntity_,
                neko::Vec3f::up * 200.0f);
        }
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::KEY_LEFT_CTRL) ==
            neko::sdl::ButtonState::DOWN) {
            physicsEngine_->AddForceAtPosition(
                cubeEntity_,
                neko::Vec3f::up * 200.0f,
                neko::Vec3f::one);
        }
        //logDebug(
        //    neko::Vec3f(
        //        transform3dManager_->GetAngles(cubeEntity_).x.value(),
        //        transform3dManager_->GetAngles(cubeEntity_).y.value(),
        //        transform3dManager_->GetAngles(cubeEntity_).z.value()).
        //    ToString());


        //Raycast
        neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
            transform3dManager_->GetPosition(cubeEntity_),
            neko::Vec3f::down,
            50.0f);
        //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
        //    " Distance : " << raycastInfo.GetDistance() <<
        //    " Position : " << raycastInfo.GetPoint() <<
        //    " Normal : " << raycastInfo.GetNormal() << std::endl;

        //Display Gizmo
        for (neko::Entity entity = 0.0f;
             entity < entityManager_->GetEntitiesSize(); entity++) {
            if (!entityManager_->HasComponent(
                    entity,
                    neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)) &&
                !entityManager_->HasComponent(
                    entity,
                    neko::EntityMask(neko::ComponentType::RIGID_STATIC))) {
                continue;
            }

            neko::physics::ColliderType colliderType = physicsEngine_->
                GetColliderType(entity);
            switch (colliderType) {
            case neko::physics::ColliderType::INVALID:
                break;
            case neko::physics::ColliderType::BOX: {
                neko::physics::BoxColliderData boxColliderData = physicsEngine_
                    ->
                    GetBoxColliderData(entity);
                gizmosLocator_->DrawCube(
                    transform3dManager_->GetPosition(entity) + boxColliderData.
                    offset,
                    boxColliderData.size,
                    transform3dManager_->GetAngles(entity),
                    boxColliderData.isTrigger ?
                        neko::Color4(neko::Color::yellow, 1.0f) :
                    neko::Color4(neko::Color::green, 1.0f),
                    2.0f);
            }
            break;
            case neko::physics::ColliderType::SPHERE: {
                neko::physics::SphereColliderData sphereColliderData =
                    physicsEngine_->GetSphereColliderData(entity);
                //gizmosRenderer_.DrawSphere(
                //    transform3dManager_.GetPosition(entity) + sphereColliderData.offset,
                //    sphereColliderData.radius,
                //    transform3dManager_.GetAngles(entity),
                //    sphereColliderData.isTrigger ? neko::Color::yellow : neko::Color::green,
                //    2.0f);
            }
            break;
            default: ;
            }
        }
    }


    void OnCollisionEnter(const physx::PxContactPair& pairHeader) override
    {
        //logDebug(std::to_string(pairHeader.actors[0]->getNbShapes()));
    }

    void OnTriggerEnter(physx::PxTriggerPair* pairs) override
    {
        //logDebug(std::to_string(pairs->otherActor->getNbShapes()));
    }

    void FixedUpdate(neko::seconds dt) override { }

    void DrawImGui() override { }
private:
    const static size_t kCubeNumbers = 25;
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 5.0f, -5.0f);
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, -3.0f, -5.0f);
    neko::Vec3f cubePositions_[kCubeNumbers] =
    {
        neko::Vec3f(0.0f, 0.0f, 0.0f),
        neko::Vec3f(2.0f, 5.0f, -15.0f),
        neko::Vec3f(-1.5f, -2.2f, -2.5f),
        neko::Vec3f(-3.8f, -2.0f, -12.3f),
        neko::Vec3f(2.4f, -0.4f, -3.5f),
        neko::Vec3f(-1.7f, 3.0f, -7.5f),
        neko::Vec3f(1.3f, -2.0f, -2.5f),
        neko::Vec3f(1.5f, 2.0f, -2.5f),
        neko::Vec3f(1.5f, 0.2f, -1.5f),
        neko::Vec3f(-1.3f, 1.0f, -1.5f)
    };

    neko::Entity cubeEntity_;
    neko::Entity planeEntity_;

};

class SceneRotation final : public SceneInterface {
public:
    void InitActors(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration = 400;
        entityManager_ = &entityManager;
        transform3dManager_ = &transform3dManager;
        physicsEngine_ = &physicsEngine;
        gizmosLocator_ = &neko::GizmosLocator::get();
        
        //Cube
        cubeEntity_ = entityManager_->CreateEntity();
        transform3dManager_->AddComponent(cubeEntity_);
        transform3dManager_->SetPosition(
            cubeEntity_,
            cubePosition_);
        transform3dManager_->SetScale(cubeEntity_, cubeSize_);
        neko::physics::RigidDynamicData rigidDynamic;
        rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
        rigidDynamic.material = neko::physics::PhysicsMaterial{
            0.5f,
            0.5f,
            0.1f
        };
        rigidDynamic.mass = 1.0f;
        rigidDynamic.useGravity = false;
        rigidDynamic.freezeRotation = neko::Vec3<bool>(false);
        rigidDynamic.freezePosition = neko::Vec3<bool>(true);
        physicsEngine_->AddRigidDynamic(
            cubeEntity_,
            rigidDynamic);
        physicsEngine_->AddForceAtPosition(
            cubeEntity_,
            neko::Vec3f::up * 200.0f,
            neko::Vec3f::one);
        viewedEntity = cubeEntity_;
    }

    void HasSucceed() override
    {
        EXPECT_TRUE(true);
        EXPECT_TRUE(true);
    }

    void Update() override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::SPACE) ==
            neko::sdl::ButtonState::DOWN) {
        }
        //logDebug(
        //    neko::Vec3f(
        //        transform3dManager_->GetAngles(cubeEntity_).x.value(),
        //        transform3dManager_->GetAngles(cubeEntity_).y.value(),
        //        transform3dManager_->GetAngles(cubeEntity_).z.value()).
        //    ToString());

        //Display Gizmo
        for (neko::Entity entity = 0.0f;
            entity < entityManager_->GetEntitiesSize(); entity++) {
            if (!entityManager_->HasComponent(
                entity,
                neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)) &&
                !entityManager_->HasComponent(
                    entity,
                    neko::EntityMask(neko::ComponentType::RIGID_STATIC))) {
                continue;
            }

            neko::physics::ColliderType colliderType = physicsEngine_->
                GetColliderType(entity);
            switch (colliderType) {
            case neko::physics::ColliderType::INVALID:
                break;
            case neko::physics::ColliderType::BOX: {
                neko::physics::BoxColliderData boxColliderData = physicsEngine_
                    ->GetBoxColliderData(entity);
                gizmosLocator_->DrawCube(
                    transform3dManager_->GetPosition(entity) + boxColliderData.
                    offset,
                    boxColliderData.size,
                    transform3dManager_->GetAngles(entity),
                    boxColliderData.isTrigger ?
                    neko::Color4(neko::Color::yellow, 1.0f) :
                    neko::Color4(neko::Color::green, 1.0f),
                    2.0f);
            }
            }
        }
    }

    void DrawImGui() override { }
private:
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 0.0f, 0.0f);
    neko::Vec3f cubeSize_ = neko::Vec3f(1.0f, 1.0f, 1.0f);

    neko::Entity cubeEntity_;
    neko::Entity planeEntity_;

};

class SceneRaycast final : public SceneInterface {
public:
    void InitActors(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration = 50;
        entityManager_ = &entityManager;
        transform3dManager_ = &transform3dManager;
        physicsEngine_ = &physicsEngine;
        gizmosLocator_ = &neko::GizmosLocator::get();
        //Plane
        {
            planeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(planeEntity_);
            transform3dManager_->SetPosition(planeEntity_, planePosition_);
            transform3dManager_->SetScale(planeEntity_, neko::Vec3f(4, 1, 4));
            neko::physics::RigidStaticData rigidStatic;
            rigidStatic.colliderType = neko::physics::ColliderType::BOX;
            rigidStatic.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            rigidStatic.boxColliderData.isTrigger = false;
            physicsEngine_->AddRigidStatic(
                planeEntity_,
                rigidStatic);
        }
        viewedEntity = planeEntity_;
    }

    void HasSucceed() override
    {
        EXPECT_TRUE(rayOneTouch_);
        EXPECT_FALSE(rayTwoTouch_);
        EXPECT_FALSE(rayThreeTouch_);
        EXPECT_FALSE(rayFourTouch_);
    }

    void Update() override
    {
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_,
                rayDirection_,
                50.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayOneTouch_ = raycastInfo.touch;
            //Display Gizmo
            gizmosLocator_->DrawLine(
                rayOrigin_,
                rayOrigin_ + rayDirection_* 50.0f,
                raycastInfo.touch ?
                neko::Color4(neko::Color::green, 1.0f) :
                neko::Color4(neko::Color::red, 1.0f),
                2.0f);
        }
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_ + neko::Vec3f::right * 2,
                rayDirection_,
                2.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayTwoTouch_ = raycastInfo.touch;

            //Display Gizmo
            gizmosLocator_->DrawLine(
                rayOrigin_ + neko::Vec3f::right * 2,
                rayOrigin_ + neko::Vec3f::right * 2 + rayDirection_ * 2.0f,
                raycastInfo.touch ?
                neko::Color4(neko::Color::green, 1.0f) :
                neko::Color4(neko::Color::red, 1.0f),
                2.0f);
        }
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_ + neko::Vec3f::left * 4,
                rayDirection_,
                50.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayThreeTouch_ = raycastInfo.touch;

            //Display Gizmo
            gizmosLocator_->DrawLine(
                rayOrigin_ + neko::Vec3f::left * 4,
                rayOrigin_ + neko::Vec3f::left * 4 + rayDirection_ * 50.0f,
                raycastInfo.touch ?
                neko::Color4(neko::Color::green, 1.0f) :
                neko::Color4(neko::Color::red, 1.0f),
                2.0f);
        }
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_ + neko::Vec3f::left * 2,
                rayOrigin_ + neko::Vec3f::left * 2 + neko::Vec3f::up,
                50.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayFourTouch_ = raycastInfo.touch;

            //Display Gizmo
            gizmosLocator_->DrawLine(
                rayOrigin_ + neko::Vec3f::left * 2,
                rayOrigin_ + neko::Vec3f::left * 2 + neko::Vec3f::up * 50.0f,
                raycastInfo.touch ?
                neko::Color4(neko::Color::green, 1.0f) :
                neko::Color4(neko::Color::red, 1.0f),
                2.0f);
        }
        //std::cout << std::endl;
    }

    void DrawImGui() override { }
private:
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, -3.0f, -5.0f);
    neko::Entity planeEntity_;

    neko::Vec3f rayOrigin_ = neko::Vec3f(0.0f, 0.0f, -5.0f);
    neko::Vec3f rayDirection_ = neko::Vec3f::down;

    bool rayOneTouch_ = false;
    bool rayTwoTouch_ = false;
    bool rayThreeTouch_ = false;
    bool rayFourTouch_ = false;
};

class SceneCollision final : public SceneInterface {
public:
    void InitActors(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration = 100;
        entityManager_ = &entityManager;
        transform3dManager_ = &transform3dManager;
        physicsEngine_ = &physicsEngine;
        gizmosLocator_ = &neko::GizmosLocator::get();
        //Plane
        {
            planeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(planeEntity_);
            transform3dManager_->SetPosition(planeEntity_, planePosition_);
            transform3dManager_->SetScale(planeEntity_, neko::Vec3f(5, 0.5f, 5));
            neko::physics::RigidStaticData rigidStatic;
            rigidStatic.colliderType = neko::physics::ColliderType::BOX;
            physicsEngine_->AddRigidStatic(
                planeEntity_,
                rigidStatic);
        }
        //Cube Touch
        {
            cubeTouchEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeTouchEntity_);
            transform3dManager_->SetPosition(
                cubeTouchEntity_,
                cubePosition_ + neko::Vec3f::left * 1.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            physicsEngine_->AddRigidDynamic(
                cubeTouchEntity_,
                rigidDynamic);
        }
        //Cube Trigger
        {
            cubeTriggerEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeTriggerEntity_);
            transform3dManager_->SetPosition(
                cubeTriggerEntity_,
                cubePosition_ + neko::Vec3f::right * 1.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            rigidDynamic.boxColliderData.isTrigger = true;
            physicsEngine_->AddRigidDynamic(
                cubeTriggerEntity_,
                rigidDynamic);
        }
        //Cube Not Touch
        {
            cubeNotTouchEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeNotTouchEntity_);
            transform3dManager_->SetPosition(
                cubeNotTouchEntity_,
                cubePosition_ + neko::Vec3f::right * 5.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            physicsEngine_->AddRigidDynamic(
                cubeNotTouchEntity_,
                rigidDynamic);
        }
        //Cube Not Trigger
        {
            cubeNotTriggerEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeNotTriggerEntity_);
            transform3dManager_->SetPosition(
                cubeNotTriggerEntity_,
                cubePosition_ + neko::Vec3f::left * 5.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            rigidDynamic.boxColliderData.isTrigger = true;
            physicsEngine_->AddRigidDynamic(
                cubeNotTriggerEntity_,
                rigidDynamic);
        }
        viewedEntity = cubeTouchEntity_;
    }

    void HasSucceed() override
    {
        EXPECT_TRUE(cubeTouch_);
        EXPECT_FALSE(cubeNotTouch_);
        EXPECT_TRUE(cubeTrigger_);
        EXPECT_TRUE(cubeTriggerExit_);
        EXPECT_FALSE(cubeNotTrigger_);
    }

    void Update() override
    {
        //Display Gizmo
        for (neko::Entity entity = 0.0f;
            entity < entityManager_->GetEntitiesSize(); entity++) {
            if (!entityManager_->HasComponent(
                entity,
                neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC))) {
                continue;
            }

            neko::physics::ColliderType colliderType = physicsEngine_->
                GetColliderType(entity);
            switch (colliderType) {
            case neko::physics::ColliderType::INVALID:
                break;
            case neko::physics::ColliderType::BOX: {
                neko::physics::BoxColliderData boxColliderData = physicsEngine_
                    ->
                    GetBoxColliderData(entity);
                gizmosLocator_->DrawCube(
                    transform3dManager_->GetPosition(entity) + boxColliderData.
                    offset,
                    boxColliderData.size,
                    transform3dManager_->GetAngles(entity),
                    boxColliderData.isTrigger ?
                    neko::Color4(neko::Color::yellow, 1.0f) :
                    neko::Color4(neko::Color::green, 1.0f),
                    2.0f);
            }
            break;
            }
        }
    }


    void OnCollisionEnter(const physx::PxContactPair& pairHeader) override
    {
        if (pairHeader.shapes[0]->getActor() == physicsEngine_->GetRigidDynamic(cubeTouchEntity_).GetPxRigidDynamic() ||
            pairHeader.shapes[1]->getActor() == physicsEngine_->GetRigidDynamic(cubeTouchEntity_).GetPxRigidDynamic()) {
            cubeTouch_ = true;
        }
        if (pairHeader.shapes[0]->getActor() == physicsEngine_->GetRigidDynamic(cubeNotTouchEntity_).GetPxRigidDynamic() ||
            pairHeader.shapes[1]->getActor() == physicsEngine_->GetRigidDynamic(cubeNotTouchEntity_).GetPxRigidDynamic()) {
            cubeNotTouchEntity_ = true;
        }
    }

    void OnTriggerEnter(physx::PxTriggerPair* pairs) override
    {
        if (pairs->triggerActor == physicsEngine_->GetRigidDynamic(cubeTriggerEntity_).GetPxRigidDynamic()) {
            cubeTrigger_ = true;
        }
        if (pairs->triggerActor == physicsEngine_->GetRigidDynamic(cubeNotTriggerEntity_).GetPxRigidDynamic()) {
            cubeNotTrigger_ = true;
        }
    }

    void OnTriggerExit(physx::PxTriggerPair* pairs) override
    {
        if (pairs->triggerActor == physicsEngine_->GetRigidDynamic(cubeTriggerEntity_).GetPxRigidDynamic()) {
            cubeTriggerExit_ = true;
        }
    }

    void FixedUpdate(neko::seconds dt) override { }

    void DrawImGui() override { }
private:
    const static size_t kCubeNumbers = 25;
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 5.0f, -5.0f);
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, 0.0f, -5.0f);

    neko::Entity cubeTouchEntity_ = neko::INVALID_ENTITY;
    neko::Entity cubeNotTouchEntity_ = neko::INVALID_ENTITY;
    neko::Entity cubeTriggerEntity_ = neko::INVALID_ENTITY;
    neko::Entity cubeNotTriggerEntity_ = neko::INVALID_ENTITY;

    neko::Entity planeEntity_ = neko::INVALID_ENTITY;

    bool cubeTouch_ = false;
    bool cubeNotTouch_ = false;
    bool cubeTrigger_ = false;
    bool cubeNotTrigger_ = false;
    bool cubeTriggerExit_ = false;

};

class TestPhysX
    : public neko::SystemInterface,
      public neko::RenderCommandInterface,
      public neko::DrawImGuiInterface {
public :
    TestPhysX(
        neko::aer::AerEngine& engine,
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        SceneInterface& sceneInterface)
        : engine_(engine),
          entityManager_(entityManager),
          transform3dManager_(transform3dManager),
          physicsEngine_(entityManager, transform3dManager),
          gizmosRenderer_(nullptr),
          rigidDynamicViewer_(entityManager, physicsEngine_),
          transform3dViewer_(entityManager, transform3dManager),
          sceneInterface_(sceneInterface)
    {
        physicsEngine_.RegisterTriggerListener(sceneInterface_);
        physicsEngine_.RegisterCollisionListener(sceneInterface_);
        physicsEngine_.RegisterFixedUpdateListener(sceneInterface_);
        engine_.RegisterOnDrawUi(sceneInterface_);
        physicsEngine_.RegisterFixedUpdateListener(rigidDynamicViewer_);
    }

    void InitRenderer()
    {
        textureManager_.Init();
        gizmosRenderer_.Init();
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        shader_.LoadFromFile(
            config.dataRootPath + "shaders/opengl/coords.vert",
            config.dataRootPath + "shaders/opengl/coords.frag");
        textureWallId_ = textureManager_.LoadTexture(
            config.dataRootPath + "sprites/wall.jpg", neko::Texture::DEFAULT);

        cube_.Init();
        quad_.Init();


        glEnable(GL_DEPTH_TEST);
    }

    void Init() override
    {
        physicsEngine_.Start();

        sceneInterface_.InitActors(
            entityManager_,
            transform3dManager_,
            physicsEngine_);
        rigidDynamicViewer_.SetSelectedEntity(sceneInterface_.viewedEntity);
        transform3dViewer_.SetSelectedEntity(sceneInterface_.viewedEntity);

        camera_.position = kCameraOriginPos_;
        camera_.reverseDirection = neko::Vec3f::forward;
        camera_.fovY = neko::degree_t(45.0f);
        camera_.nearPlane = 0.1f;
        camera_.farPlane = 100.0f;
        gizmosRenderer_.SetCamera(&camera_);

        physicsEngine_.StartPhysic();
    }

    void Update(neko::seconds dt) override
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        camera_.SetAspect(
            static_cast<float>(config.windowSize.x) / config.windowSize.y);

        sceneInterface_.Update();

        physicsEngine_.Update(dt.count());
        textureManager_.Update(dt);
        neko::RendererLocator::get().Render(this);
        neko::RendererLocator::get().Render(&gizmosRenderer_);
        gizmosRenderer_.Update(dt);
        updateCount_++;
        if (updateCount_ == sceneInterface_.engineDuration) {
            engine_.Stop();
        }
    }

    void Render() override
    {
        if (shader_.GetProgram() == 0)
            return;
        if (textureWall_ == neko::INVALID_TEXTURE_NAME) {
            textureWall_ = textureManager_.GetTextureName(textureWallId_);
            return;
        }
        shader_.Bind();
        glBindTexture(GL_TEXTURE_2D, textureWall_);
        shader_.SetMat4("view", camera_.GenerateViewMatrix());
        shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
        for (neko::Entity entity = 0.0f;
             entity < entityManager_.GetEntitiesSize(); entity++) {
            neko::Mat4f model = neko::Mat4f::Identity; //model transform matrix
            if (entityManager_.HasComponent(
                    entity,
                    neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)) ||
                entityManager_.HasComponent(
                    entity,
                    neko::EntityMask(neko::ComponentType::RIGID_STATIC))) {
                model = neko::Transform3d::Scale(
                    model,
                    transform3dManager_.GetScale(entity));
                model = neko::Transform3d::Rotate(
                    model,
                    transform3dManager_.GetAngles(entity));
                model = neko::Transform3d::Translate(
                    model,
                    transform3dManager_.GetPosition(entity));
                shader_.SetMat4("model", model);
                cube_.Draw();
            }
        }
    }

    void Destroy() override
    {
        physicsEngine_.Destroy();
        shader_.Destroy();
        cube_.Destroy();
        quad_.Destroy();
        textureManager_.Destroy();
        gizmosRenderer_.Destroy();
    }

    void DrawImGui()
    {

        for (neko::Entity entity = 0.0f;
             entity < entityManager_.GetEntitiesSize(); entity++) {
            if (!entityManager_.HasComponent(
                entity,
                neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)))
                continue;
            std::string title = "Entity ";
            title += std::to_string(entity);
            neko::physics::RigidDynamicData rigidDynamicData;
            // = physicsEngine_.GetRigidDynamicData(entity);
            ImGui::Begin(title.c_str());
            ImGui::Text("RigidBody");
            if (physicsEngine_.IsPhysicRunning()) {
                ImGui::TextColored(
                    ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                    "Physics is active");
                if (ImGui::Button("Stop")) { physicsEngine_.StopPhysic(); }
            } else {
                ImGui::TextColored(
                    ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                    "Physics is not active");
                if (ImGui::Button("Play")) { physicsEngine_.StartPhysic(); }
            }

            rigidDynamicViewer_.DrawImGui();
            ImGui::Text("Transform");
            transform3dViewer_.DrawImGui();
            ImGui::End();

        }
    }

    void HasSucceed() { sceneInterface_.HasSucceed(); }
private :
    int updateCount_ = 0;

    neko::aer::AerEngine& engine_;
    neko::EntityManager& entityManager_;
    neko::Transform3dManager& transform3dManager_;
    neko::physics::PhysicsEngine physicsEngine_;
    neko::physics::RigidDynamicViewer rigidDynamicViewer_;
    neko::Transform3dViewer transform3dViewer_;
    neko::GizmosRenderer gizmosRenderer_;

    neko::Camera3D camera_;
    neko::EulerAngles cameraAngles_{
        neko::degree_t(0.0f),
        neko::degree_t(0.0f),
        neko::degree_t(0.0f)
    };
    const neko::Vec3f kCameraOriginPos_ = neko::Vec3f(0.0f, 0.0f, -5.0f);
    const neko::EulerAngles kCameraOriginAngles_ = neko::EulerAngles(
        neko::degree_t(0.0f),
        neko::degree_t(0.0f),
        neko::degree_t(0.0f));

    neko::gl::RenderCuboid cube_{neko::Vec3f::zero, neko::Vec3f::one};
    neko::gl::RenderQuad quad_{neko::Vec3f::zero, neko::Vec2f::one};

    neko::gl::Shader shader_;
    neko::gl::TextureManager textureManager_;
    neko::TextureName textureWall_ = neko::INVALID_TEXTURE_NAME;
    neko::TextureId textureWallId_ = neko::INVALID_TEXTURE_ID;
    neko::seconds timeSinceInit_{0};

    SceneInterface& sceneInterface_;

};

TEST(PhysX, TestCubeFall)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(
        entityManager);
    SceneCubeFall sceneCubeFall;
    TestPhysX testPhysX(engine, entityManager, transform3dManager, sceneCubeFall);

    engine.RegisterSystem(testPhysX);
    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    neko::Job initJob{[&testPhysX]() { testPhysX.InitRenderer(); }};
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}

TEST(PhysX, TestRotation)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(
        entityManager);
    SceneRotation sceneRotation;
    TestPhysX testPhysX(engine, entityManager, transform3dManager, sceneRotation);

    engine.RegisterSystem(testPhysX);
    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    neko::Job initJob{ [&testPhysX]() { testPhysX.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}

TEST(PhysX, TestRaycast)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(
        entityManager);
    SceneRaycast sceneRaycast;
    TestPhysX testPhysX(engine, entityManager, transform3dManager, sceneRaycast);

    engine.RegisterSystem(testPhysX);
    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    neko::Job initJob{ [&testPhysX]() { testPhysX.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}

TEST(PhysX, TestTriggerCollision)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(
        entityManager);
    SceneCollision sceneCollision;
    TestPhysX testPhysX(engine, entityManager, transform3dManager, sceneCollision);

    engine.RegisterSystem(testPhysX);
    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    neko::Job initJob{ [&testPhysX]() { testPhysX.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}