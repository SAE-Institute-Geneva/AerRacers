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
#include "engine/engine.h"
#include "engine/system.h"
#include "engine/transform.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"
#include "gl/shader.h"
#include "gl/shape.h"

class TestPhysX : public neko::SystemInterface, neko::RenderCommandInterface{
public :
    TestPhysX(neko::aer::AerEngine& engine, neko::EntityManager& entityManager,  neko::Transform3dManager& transform3dManager)
        : engine_(engine),
          entityManager_(entityManager),
        transform3dManager_(transform3dManager),
          physicsEngine_(entityManager, transform3dManager) { }

void InitActors()
{
    //Plane
    {
        planeEntity_ = entityManager_.CreateEntity();
        transform3dManager_.AddComponent(planeEntity_);
        transform3dManager_.SetPosition(planeEntity_, planePosition_);
        neko::physics::px::BoxPhysicsShape boxPhysicsShape;
        boxPhysicsShape.size = neko::Vec3f::one/2.0f;
        boxPhysicsShape.material = neko::physics::PhysicsMaterial{ 0.5f, 0.5f, 0.1f };
        boxPhysicsShape.isTrigger = true;
        physicsEngine_.AddBoxPhysicsShape(planeEntity_, boxPhysicsShape);
        neko::physics::px::RigidStatic rigidStatic;
        physicsEngine_.AddRigidStatic(planeEntity_, rigidStatic);
    }
    //Cube
    { 
        cubeEntity_ = entityManager_.CreateEntity();
        transform3dManager_.AddComponent(cubeEntity_);
        transform3dManager_.SetPosition(cubeEntity_, cubePosition_);
        neko::physics::px::BoxPhysicsShape boxPhysicsShape;
        boxPhysicsShape.size = neko::Vec3f::one/2.0f;
        boxPhysicsShape.material = neko::physics::PhysicsMaterial{ 0.5f, 0.5f, 0.1f };
        boxPhysicsShape.isTrigger = true;
        physicsEngine_.AddBoxPhysicsShape(cubeEntity_, boxPhysicsShape);
        neko::physics::px::RigidDynamic rigidDynamic;
        rigidDynamic.mass = 1.0f;
        rigidDynamic.useGravity = true;
        rigidDynamic.freezeRotation = neko::Vec3<bool>(true);
        physicsEngine_.AddRigidDynamic(cubeEntity_, rigidDynamic);
    }

}

void InitRenderer()
{
    textureManager_.Init();
    const auto& config = neko::BasicEngine::GetInstance()->config;
    shader_.LoadFromFile(
        config.dataRootPath + "shaders/04_hello_coords/coords.vert",
        config.dataRootPath + "shaders/04_hello_coords/coords.frag");
    textureWallId_ = textureManager_.LoadTexture(config.dataRootPath + "sprites/wall.jpg");

    cube_.Init();
    quad_.Init();

    // note that we're translating the scene in the reverse direction of where we want to move
    view_ = neko::Mat4f::Identity;
    view_ = neko::Transform3d::Translate(view_, neko::Vec3f(0.0f, 0.0f, -3.0f));


    glEnable(GL_DEPTH_TEST);
}

void Init() override
{
    physicsEngine_.Start();
    InitActors();

}

    void Update(neko::seconds dt) override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::SPACE) == neko::sdl::ButtonState::DOWN) {
            neko::physics::px::RigidDynamic cubeRigid = physicsEngine_.GetRigidDynamic(cubeEntity_);
            cubeRigid.AddForce(neko::Vec3f::up * 200.0f);
        }
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::KEY_LEFT_CTRL) == neko::sdl::ButtonState::DOWN) {
            neko::physics::px::RigidDynamic cubeRigid = physicsEngine_.GetRigidDynamic(cubeEntity_);
            cubeRigid.AddForceAtPosition(neko::Vec3f::up * 200.0f, neko::Vec3f::right);
        }


        physicsEngine_.Update(dt.count());
        for (neko::Entity entity = 0.0f; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)) &&
                !entityManager_.HasComponent(entity, neko::EntityMask(neko::ComponentType::RIGID_STATIC)))
            {
                continue;
            }
            //neko::Vec3f pos = transform3dManager_.GetPosition(entity);
            //std::cout << pos << std::endl;
        }
        const auto& config = neko::BasicEngine::GetInstance()->config;
        projection_ = neko::Transform3d::Perspective(
            neko::degree_t(45.0f),
            static_cast<float>(config.windowSize.x) / config.windowSize.y,
            0.1f,
            100.0f);
        textureManager_.Update(dt);
        neko::RendererLocator::get().Render(this);
        //updateCount_++;
        if (updateCount_ == kEngineDuration_) {
            engine_.Stop();
        }
    }

    void Render() override
    {
        if (shader_.GetProgram() == 0)
            return;
        if (textureWall_ == neko::INVALID_TEXTURE_NAME)
        {
            textureWall_ = textureManager_.GetTexture(textureWallId_).name;
            return;
        }
        shader_.Bind();
        glBindTexture(GL_TEXTURE_2D, textureWall_);
        shader_.SetMat4("view", view_);
        shader_.SetMat4("projection", projection_);
        for (neko::Entity entity = 0.0f; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            neko::Mat4f model = neko::Mat4f::Identity; //model transform matrix
            if (entityManager_.HasComponent(entity, neko::EntityMask(neko::ComponentType::BOX_COLLIDER)))
            {
                model = neko::Transform3d::Rotate(model, transform3dManager_.GetAngles(entity));
                model = neko::Transform3d::Translate(model, transform3dManager_.GetPosition(entity));
                shader_.SetMat4("model", model);
                cube_.Draw();
            }
            // else if (entityManager_.HasComponent(entity, neko::EntityMask(neko::ComponentType::QUAD_COLLIDER) {
            //    model = neko::Mat4f::Identity; //model transform matrix
            //    model = neko::Transform3d::Rotate(model, neko::degree_t(90.0f), neko::Vec3f::right);
            //    model = neko::Transform3d::Translate(model, planePosition_);
            //    shader_.SetMat4("model", model);
            //    quad_.Draw();
            //    
            //}
        }
    }

    void Destroy() override
{
    physicsEngine_.Destroy();
    shader_.Destroy();
    cube_.Destroy();
    quad_.Destroy();
    textureManager_.Destroy();

}

private :
    int updateCount_ = 0;
    const int kEngineDuration_ = 200;

    neko::aer::AerEngine& engine_;
    neko::EntityManager& entityManager_;
    neko::Transform3dManager& transform3dManager_;
    neko::physics::px::PhysicsEngine physicsEngine_;
    //physx::PxShape* shape;
    //physx::PxRigidActor* plane;
    //physx::PxRigidDynamic* actor;


    neko::gl::RenderCuboid cube_{neko::Vec3f::zero, neko::Vec3f::one};
    neko::gl::RenderQuad quad_{neko::Vec3f::zero, neko::Vec2f::one};
    const static size_t cubeNumbers_ = 10;
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 5.0f, -5.0f);
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, -3.0f, -5.0f);
    neko::Vec3f cubePositions[cubeNumbers_] =
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
     neko::TextureName  textureWall_ = neko::INVALID_TEXTURE_NAME;
     neko::TextureId  textureWallId_;
     neko::Mat4f view_{ neko::Mat4f::Identity };
     neko::Mat4f projection_{ neko::Mat4f::Identity };
     neko::seconds timeSinceInit_{ 0 };

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
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::aer::AerEngine engine(&config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(entityManager);
    TestPhysX testPhysX(engine, entityManager, transform3dManager);

    engine.RegisterSystem(testPhysX);
    engine.Init();
    neko::Job initJob{ [&testPhysX]() { testPhysX.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(&initJob, neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}
