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

class TestPhysX
    : public neko::SystemInterface,
      public neko::RenderCommandInterface,
      public neko::DrawImGuiInterface,
      neko::physics::OnCollisionInterface,
      neko::physics::OnTriggerInterface,
      neko::physics::FixedUpdateInterface{
public :
    TestPhysX(
        neko::aer::AerEngine& engine,
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager)
        : engine_(engine),
          entityManager_(entityManager),
          transform3dManager_(transform3dManager),
          physicsEngine_(entityManager, transform3dManager),
          gizmosRenderer_(nullptr),
        rigidDynamicViewer_(entityManager, physicsEngine_)
    {
        physicsEngine_.RegisterTriggerListener(*this);
        physicsEngine_.RegisterCollisionListener(*this);
        physicsEngine_.RegisterFixedUpdateListener(*this);
        physicsEngine_.RegisterFixedUpdateListener(rigidDynamicViewer_);
    }

    void InitActors()
    {
        //Plane
        {
            planeEntity_ = entityManager_.CreateEntity();
            transform3dManager_.AddComponent(planeEntity_);
            transform3dManager_.SetPosition(planeEntity_, planePosition_);
            transform3dManager_.SetScale(planeEntity_, neko::Vec3f(5, 1, 5));
            neko::physics::BoxColliderData boxCollider;
            boxCollider.size = neko::Vec3f::one / 2.0f;
            boxCollider.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            boxCollider.isTrigger = true;
            neko::physics::RigidStaticData rigidStatic;
            physicsEngine_.AddRigidStatic(
                planeEntity_,
                rigidStatic,
                boxCollider);
        }
        //Cube
        {
            cubeEntity_ = entityManager_.CreateEntity();
            transform3dManager_.AddComponent(cubeEntity_);
            transform3dManager_.SetPosition(cubeEntity_, cubePosition_);
            neko::physics::BoxColliderData boxCollider;
            boxCollider.size = neko::Vec3f::one;
            boxCollider.offset = neko::Vec3f::one;
            boxCollider.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            boxCollider.isTrigger = true;
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.mass = 1.0f;
            rigidDynamic.useGravity = true;
            rigidDynamic.freezeRotation = neko::Vec3<bool>(false);
            rigidDynamic.boxColliderData = boxCollider;
            physicsEngine_.AddRigidDynamic(
                cubeEntity_,
                rigidDynamic,
                boxCollider);
            rigidDynamicViewer_.SetSelectedEntity(cubeEntity_);
        }
    }

    void InitRenderer()
    {
        textureManager_.Init();
        gizmosRenderer_.Init();
        const auto& config = neko::BasicEngine::GetInstance()->config;
        shader_.LoadFromFile(
            config.dataRootPath + "shaders/aer_racer/coords.vert",
            config.dataRootPath + "shaders/aer_racer/coords.frag");
        textureWallId_ = textureManager_.LoadTexture(
            config.dataRootPath + "sprites/wall.jpg");

        cube_.Init();
        quad_.Init();


        glEnable(GL_DEPTH_TEST);
    }

    void Init() override
    {
        physicsEngine_.Start();
        InitActors();
        camera_.position = kCameraOriginPos_;
        camera_.reverseDirection = neko::Vec3f::forward;
        camera_.fovY = neko::degree_t(45.0f);
        camera_.nearPlane = 0.1f;
        camera_.farPlane = 100.0f;
        gizmosRenderer_.SetCamera(&camera_);
    }

    void Update(neko::seconds dt) override
    {
        const auto& config = neko::BasicEngine::GetInstance()->config;
        camera_.SetAspect(static_cast<float>(config.windowSize.x) / config.windowSize.y);
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::SPACE) ==
            neko::sdl::ButtonState::DOWN) {
            physicsEngine_.AddForce(cubeEntity_, neko::Vec3f::up * 200.0f);
        }
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::KEY_LEFT_CTRL) ==
            neko::sdl::ButtonState::DOWN) {
            physicsEngine_.AddForceAtPosition(cubeEntity_,
                neko::Vec3f::up * 200.0f,
                neko::Vec3f::right);
        }
        neko::physics::PxRaycastInfo raycastInfo = physicsEngine_.Raycast(
            transform3dManager_.GetPosition(cubeEntity_),
            neko::Vec3f::down,
            50.0f);
        //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
        //    " Distance : " << raycastInfo.GetDistance() <<
        //    " Position : " << raycastInfo.GetPoint() <<
        //    " Normal : " << raycastInfo.GetNormal() << std::endl;
        for (neko::Entity entity = 0.0f;
            entity < entityManager_.GetEntitiesSize(); entity++) {
            if (!entityManager_.HasComponent(
                entity,
                neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)) &&
                !entityManager_.HasComponent(
                    entity,
                    neko::EntityMask(neko::ComponentType::RIGID_STATIC))) {
                continue;
            }

            neko::physics::ColliderType colliderType = physicsEngine_.GetColliderType(entity);
            switch (colliderType) {
            case neko::physics::ColliderType::INVALID:
                break;
            case neko::physics::ColliderType::BOX:
            {
                neko::physics::BoxColliderData boxColliderData = physicsEngine_.GetBoxColliderData(entity);
                gizmosRenderer_.DrawCube(
                    transform3dManager_.GetPosition(entity) + boxColliderData.offset,
                    transform3dManager_.GetScale(entity) * boxColliderData.size,
                    transform3dManager_.GetAngles(entity),
                    boxColliderData.isTrigger ? neko::Color::yellow : neko::Color::green,
                    2.0f);
            }
            break;
            case neko::physics::ColliderType::SPHERE:
            {
                neko::physics::SphereColliderData sphereColliderData = physicsEngine_.GetSphereColliderData(entity);
                //gizmosRenderer_.DrawSphere(
                //    transform3dManager_.GetPosition(entity) + sphereColliderData.offset,
                //    sphereColliderData.radius,
                //    transform3dManager_.GetAngles(entity),
                //    sphereColliderData.isTrigger ? neko::Color::yellow : neko::Color::green,
                //    2.0f);
            }
            break;
            default:;
            }
        }

        physicsEngine_.Update(dt.count());
        textureManager_.Update(dt);
        neko::RendererLocator::get().Render(this);
        neko::RendererLocator::get().Render(&gizmosRenderer_);
        gizmosRenderer_.Update(dt);
        //updateCount_++;
        if (updateCount_ == kEngineDuration_) { engine_.Stop(); }
    }

    void Render() override
    {
        if (shader_.GetProgram() == 0)
            return;
        if (textureWall_ == neko::INVALID_TEXTURE_NAME) {
            textureWall_ = textureManager_.GetTexture(textureWallId_).name;
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


    void OnCollisionEnter(const physx::PxContactPairHeader& pairHeader) override
    {
        //logDebug(std::to_string(pairHeader.actors[0]->getNbShapes()));
    }

    void OnTriggerEnter(physx::PxTriggerPair* pairs) override
    {
        //logDebug(std::to_string(pairs->otherActor->getNbShapes()));
    }
    
    void DrawImGui()
    {
        std::string title = "RigidBody ";

        for (neko::Entity entity = 0.0f;
             entity < entityManager_.GetEntitiesSize(); entity++) {
            if (!entityManager_.HasComponent(
                entity,
                neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)))
                continue;
            title += std::to_string(entity);
            neko::physics::RigidDynamicData rigidDynamicData;// = physicsEngine_.GetRigidDynamicData(entity);
            ImGui::Begin(title.c_str());

            if (physicsEngine_.IsPhysicRunning())
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Physics is active");
                if (ImGui::Button("Stop")) {
                    physicsEngine_.StopPhysic();
                }
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Physics is not active");
                if (ImGui::Button("Play")) {
                    physicsEngine_.StartPhysic();
                }
            }

            
            rigidDynamicViewer_.DrawImGui();

            ImGui::End();
        }
    }


    void FixedUpdate(neko::seconds dt) override
{
       
}
private :
    int updateCount_ = 0;
    const int kEngineDuration_ = 200;

    neko::aer::AerEngine& engine_;
    neko::EntityManager& entityManager_;
    neko::Transform3dManager& transform3dManager_;
    neko::physics::PhysicsEngine physicsEngine_;
    neko::physics::RigidDynamicViewer rigidDynamicViewer_;
    neko::GizmosRenderer gizmosRenderer_;

    neko::Camera3D camera_;
    neko::EulerAngles cameraAngles_{ neko::degree_t(0.0f),  neko::degree_t(0.0f),  neko::degree_t(0.0f) };
    const neko::Vec3f kCameraOriginPos_ = neko::Vec3f(0.0f, 0.0f, -5.0f);
    const neko::EulerAngles kCameraOriginAngles_ = neko::EulerAngles(
        neko::degree_t(0.0f), neko::degree_t(0.0f), neko::degree_t(0.0f));

    neko::gl::RenderCuboid cube_{neko::Vec3f::zero, neko::Vec3f::one};
    neko::gl::RenderQuad quad_{neko::Vec3f::zero, neko::Vec2f::one};
    const static size_t kCubeNumbers = 10;
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
    neko::gl::Shader shader_;
    neko::gl::TextureManager textureManager_;
    neko::TextureName textureWall_ = neko::INVALID_TEXTURE_NAME;
    neko::TextureId textureWallId_;
    neko::seconds timeSinceInit_{0};

    neko::Entity cubeEntity_;
    neko::Entity planeEntity_;

};

TEST(PhysX, TestPhysX)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::aer::AerEngine engine(&config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(
        entityManager);
    TestPhysX testPhysX(engine, entityManager, transform3dManager);

    engine.RegisterSystem(testPhysX);
    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    neko::Job initJob{[&testPhysX]() { testPhysX.InitRenderer(); }};
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}
