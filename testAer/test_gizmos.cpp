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

#include "gizmos_renderer.h"
#ifdef NEKO_GLES3
#include <log.h>


#include "aer_engine.h"
#include "engine/engine.h"
#include "engine/system.h"
#include "engine/transform.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"
#include "gl/shader.h"
#include "gl/shape.h"

class TestGizmos
    : public neko::SystemInterface,
    public neko::RenderCommandInterface,
    public neko::DrawImGuiInterface {
public:
    TestGizmos(
        neko::aer::AerEngine& engine,
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager)
        : engine_(engine),
        entityManager_(entityManager),
        transform3dManager_(transform3dManager),
        gizmosRenderer_(nullptr)
    {
    }

    void InitActors()
    {
        //Plane
        {
            planeEntity_ = entityManager_.CreateEntity();
            transform3dManager_.AddComponent(planeEntity_);
            transform3dManager_.SetPosition(planeEntity_, planePosition_);
            transform3dManager_.SetScale(planeEntity_, neko::Vec3f(5, 1, 5));
        }
        //Cube
        {
            cubeEntity_ = entityManager_.CreateEntity();
            transform3dManager_.AddComponent(cubeEntity_);
            transform3dManager_.SetPosition(cubeEntity_, cubePosition_);
        }
    }

    void InitRenderer()
    {
        textureManager_.Init();
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        shader_.LoadFromFile(
            config.dataRootPath + "shaders/opengl/coords.vert",
            config.dataRootPath + "shaders/opengl/coords.frag");
        textureWallId_ = textureManager_.LoadTexture(
            config.dataRootPath + "sprites/wall.jpg", neko::Texture::DEFAULT);

        cube_.Init();
        quad_.Init();
        sphere_.Init();
        circle_.Init();
        wireFrameSphere_.Init();
        wireFrameSphere_.SetLineWidth(1.0f);
        gizmosRenderer_.Init();
        glEnable(GL_DEPTH_TEST);
    }

    void Init() override
    {
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
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::SPACE) ==
            neko::sdl::ButtonState::DOWN) {
        }
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::KEY_LEFT_CTRL) ==
            neko::sdl::ButtonState::DOWN) {
        }
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        camera_.SetAspect(static_cast<float>(config.windowSize.x) / config.windowSize.y);
        textureManager_.Update(dt);
        neko::RendererLocator::get().Render(this);
        neko::RendererLocator::get().Render(&gizmosRenderer_);
        updateCount_++;
        //if (updateCount_ == kEngineDuration_) { engine_.Stop(); }
        gizmosRenderer_.Update(dt);
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
        //shader_.SetMat4("view", view_);
        shader_.SetMat4("view", camera_.GenerateViewMatrix());
        shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
        for (neko::Entity entity = 0.0f;
            entity < entityManager_.GetEntitiesSize(); entity++) {
            neko::Mat4f model = neko::Mat4f::Identity; //model transform matrix
            if (entityManager_.HasComponent(
                entity,
                neko::EntityMask(neko::ComponentType::TRANSFORM3D))) {
                model = neko::Transform3d::Scale(
                    model,
                    transform3dManager_.GetScale(entity)/2.0f);
                model = neko::Transform3d::Rotate(
                    model,
                    transform3dManager_.GetAngles(entity));
                model = neko::Transform3d::Translate(
                    model,
                    transform3dManager_.GetPosition(entity));
                shader_.SetMat4("model", model);
                sphere_.Draw();
                gizmosRenderer_.DrawCube(transform3dManager_.GetPosition(entity), transform3dManager_.GetScale(entity), neko::Color4(neko::Color::blue, 1.0f),10.0f);
                gizmosRenderer_.DrawSphere(transform3dManager_.GetPosition(entity), 1.0f, neko::Color4(neko::Color::green, 1.0f), 10.0f);
            }
        }
        circle_.Draw();
    }

    void Destroy() override
    {
        shader_.Destroy();
        cube_.Destroy();
        quad_.Destroy();
        sphere_.Destroy();
        wireFrameSphere_.Destroy();
        textureManager_.Destroy();
        gizmosRenderer_.Destroy();
    }

    void DrawImGui() override
    {
    }
private:
    int updateCount_ = 0;
    const int kEngineDuration_ = 50;

    neko::aer::AerEngine& engine_;
    neko::EntityManager& entityManager_;
    neko::Transform3dManager& transform3dManager_;
    neko::Gles3GizmosRenderer gizmosRenderer_;

    neko::Camera3D camera_;
    //neko::Mat4f view_{ neko::Mat4f::Identity };
    //neko::Mat4f projection_{ neko::Mat4f::Identity };
    neko::EulerAngles cameraAngles_{ neko::degree_t(0.0f),  neko::degree_t(0.0f),  neko::degree_t(0.0f) };
    const neko::Vec3f kCameraOriginPos_ = neko::Vec3f(0.0f, 0.0f, 3.0f);
    const neko::EulerAngles kCameraOriginAngles_ = neko::EulerAngles(
        neko::degree_t(0.0f), neko::degree_t(0.0f), neko::degree_t(0.0f));

    neko::gl::RenderCuboid cube_{ neko::Vec3f::zero, neko::Vec3f::one };
    neko::gl::RenderQuad quad_{ neko::Vec3f::zero, neko::Vec2f::one };

    neko::gl::RenderCircle circle_{ neko::Vec3f::zero, 1.0f };
    neko::gl::RenderSphere sphere_{ neko::Vec3f::zero, 1.0f };
    neko::gl::RenderWireFrameSphere wireFrameSphere_{ neko::Vec3f::zero, 1.0f };

    const static size_t kCubeNumbers = 10;
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 2.0f, -5.0f);
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, -3.0f, -5.0f);
    neko::Vec3f cubePositions[kCubeNumbers] =
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
    neko::seconds timeSinceInit_{ 0 };

    neko::Entity cubeEntity_;
    neko::Entity planeEntity_;

};

TEST(Engine, Gizmos)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    // config.dataRootPath = "../data/";
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
    TestGizmos textGizmos(engine, entityManager, transform3dManager);

    engine.RegisterSystem(textGizmos);
    engine.RegisterOnDrawUi(textGizmos);
    engine.Init();
    neko::Job initJob{ [&textGizmos]() { textGizmos.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}
#endif