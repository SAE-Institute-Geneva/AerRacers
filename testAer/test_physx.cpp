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
#include "gl/gles3_window.h"
#include "gl/graphics.h"
#include "gl/shader.h"
#include "gl/shape.h"

class TestPhysX : public neko::SystemInterface, neko::RenderCommandInterface{
public :
    TestPhysX(neko::aer::AerEngine& engine) : engine_(engine) { }

void InitActors()
{
    //Plane
    physx::PxMaterial* material = physicsEngine_.mPhysics_->createMaterial(0.5f, 0.5f, 0.1f);
    if (!material)
        std::cerr << "createMaterial failed!";
    physx::PxReal d = -1.0f;
    physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.0f, d, -5.0f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
    plane = physicsEngine_.mPhysics_->createRigidStatic(pose);
    if (!plane)
        std::cerr << "create plane failed!";
    shape = physicsEngine_.mPhysics_->createShape(physx::PxPlaneGeometry(), *material);
    shape->setMaterials(&material, 1);
    shape->setGeometry(physx::PxPlaneGeometry());
    plane->attachShape(*shape);
    if (!shape)
        std::cerr << "create shape failed!";
    physicsEngine_.scene_->addActor(*plane);
    //Cube
    physx::PxReal density = 1.0f;
    physx::PxTransform transform = physx::PxTransform(physx::PxVec3(0.0f, 5.0f, -5.0f));
    physx::PxVec3 dimensions(1.0f, 1.0f, 1.0f);
    physx::PxBoxGeometry geometry(dimensions);

   actor = PxCreateDynamic(
       *physicsEngine_.mPhysics_,
       transform,
       geometry,
       *material,
       density);
    if (!actor)
        std::cerr << "create actor failed!";
    physicsEngine_.scene_->addActor(*actor);

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
        float mStepSize = dt.count();
        physicsEngine_.scene_->simulate(mStepSize);
        physicsEngine_.scene_->fetchResults(true);
        {
            std::cout << "Result fetch" << std::endl;
            std::cout << "\tplane" << std::endl;
            physx::PxTransform np = physx::PxShapeExt::getGlobalPose(*shape, *plane);
            std::cout << "\t( " << np.p.x << " ; " << np.p.y << " ; " << np.p.z << " )" << std::endl;
            planePosition_ = neko::Vec3f(np.p);
            std::cout << "\tcube" << std::endl;
            physx::PxU32 nShapes = actor->getNbShapes();
            physx::PxShape** shapes = new physx::PxShape * [nShapes];
            actor->getShapes(shapes, nShapes);

            while (nShapes--) {
                physx::PxTransform np = physx::PxShapeExt::getGlobalPose(*shape, *actor);
                std::cout << "\t( " << np.p.x << " ; " << np.p.y << " ; " << np.p.z << " )" << std::endl;
                cubePosition_ = neko::Vec3f(np.p);
            }

            delete[] shapes;
        }
        neko::RendererLocator::get().Render(this);
        const auto& config = neko::BasicEngine::GetInstance()->config;
        projection_ = neko::Transform3d::Perspective(
            neko::degree_t(45.0f),
            static_cast<float>(config.windowSize.x) / config.windowSize.y,
            0.1f,
            100.0f);
        textureManager_.Update(dt);
        updateCount_++;
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

        //for (const auto cubePosition : cubePositions)
        //{
        //    neko::Mat4f model = neko::Mat4f::Identity; //model transform matrix
        //    model = neko::Transform3d::Translate(model, cubePosition);
        //    shader_.SetMat4("model", model);
        //    cube_.Draw();
        //}
        neko::Mat4f model = neko::Mat4f::Identity; //model transform matrix
        model = neko::Transform3d::Translate(model, cubePosition_);
        shader_.SetMat4("model", model);
        cube_.Draw();
        model = neko::Transform3d::Translate(model, planePosition_);
        model = neko::Transform3d::Rotate(model, neko::degree_t(90.0f), neko::Vec3f::up);
        shader_.SetMat4("model", model);
        quad_.Draw();
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
    const int kEngineDuration_ = 500;

    neko::aer::AerEngine& engine_;
    neko::physics::PhysicsEngine physicsEngine_;
    physx::PxShape* shape;
    physx::PxRigidActor* plane;
    physx::PxRigidDynamic* actor;


    neko::gl::RenderCuboid cube_{neko::Vec3f::zero, neko::Vec3f::one };
    neko::gl::RenderQuad quad_{neko::Vec3f::zero, neko::Vec2f::one };
    const static size_t cubeNumbers_ = 10;
    neko::Vec3f cubePosition_ = neko::Vec3f::one;
    neko::Vec3f planePosition_ = neko::Vec3f::one;
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

    TestPhysX testPhysX(engine);

    engine.RegisterSystem(testPhysX);
    engine.Init();
    neko::Job initJob{ [&testPhysX]() { testPhysX.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(&initJob, neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}
