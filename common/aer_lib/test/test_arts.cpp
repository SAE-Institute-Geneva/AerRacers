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
 Date : 22.01.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>
#ifdef EASY_PROFILE_USE
    #include "easy/profiler.h"
#endif

#include "aer/gizmos_renderer.h"
#include "aer/aer_engine.h"
#include "engine/engine.h"
#include "engine/system.h"
#include "engine/transform.h"
#ifdef NEKO_GLES3
    #include "gl/gles3_window.h"
    #include "gl/graphics.h"
    #include "gl/shader.h"
    #include "gl/shape.h"

namespace neko::aer
{
//#pragma region LevelDesign
//class TestLevelDesign : public SystemInterface,
//    public RenderCommandInterface,
//    public DrawImGuiInterface
//{
//public:
//    explicit TestLevelDesign(AerEngine& engine)
//        : engine_(engine),
//        rContainer_(engine.GetResourceManagerContainer()),
//        cContainer_(engine.GetComponentManagerContainer())
//    {
//    }
//
//    void Init() override
//    {
//        gizmosRenderer_ = &GizmosLocator::get();
//#ifdef EASY_PROFILE_USE
//        EASY_BLOCK("Test Init", profiler::colors::Green);
//#endif
//        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
//        testEntity_ = cContainer_.entityManager.CreateEntity();
//        cContainer_.transform3dManager.AddComponent(testEntity_);
//        cContainer_.renderManager.AddComponent(testEntity_);
//        cContainer_.renderManager.SetModel(
//            //testEntity_, config.dataRootPath + "models/leveldesign/big_terrain_01.obj");
//            testEntity_, config.dataRootPath + "models/leveldesign/aer_racer_circuit_v37.obj");
//        engine_.GetCameras().moveSpeed = 50.0f;
//    }
//
//    void Update(seconds dt) override
//    {
//#ifdef EASY_PROFILE_USE
//        EASY_BLOCK("Test Update", profiler::colors::Green);
//#endif
//        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
//        updateCount_ += dt.count();
//        if (updateCount_ > kEngineDuration_ || rContainer_.modelManager.IsLoaded(modelId))
//        {
//            loaded_ = rContainer_.modelManager.IsLoaded(modelId);
//            engine_.Stop();
//        }
//        if (!rContainer_.modelManager.IsLoaded(modelId)) return;
//
//        //const auto& model = rContainer_.modelManager.GetModel(modelId);
//        //for (size_t i = 0; i < model->GetMeshCount(); ++i)
//        //{
//        //    const auto& meshAabb = model->GetMesh(i).aabb;
//        //    gizmosRenderer_->DrawCube(meshAabb.CalculateCenter(), meshAabb.CalculateExtends());
//        //}
//    }
//
//    void Render() override {}
//
//    void Destroy() override
//    {
//        EXPECT_TRUE(loaded_);
//    }
//
//    void DrawImGui() override
//    {
//        ImGui::Begin("Test parameter");
//        {
//            float speed = engine_.GetCameras().moveSpeed;
//            if (ImGui::DragFloat("CameraSpeed", &speed)) {
//                engine_.GetCameras().moveSpeed = speed;
//            }
//        }
//        ImGui::End();
//    }
//
//private:
//    float updateCount_ = 0;
//    const float kEngineDuration_ = 30.0f;
//    bool loaded_ = false;
//    AerEngine& engine_;
//
//    ResourceManagerContainer& rContainer_;
//    ComponentManagerContainer& cContainer_;
//
//    IGizmoRenderer* gizmosRenderer_;
//
//    Entity testEntity_;
//};
//
//TEST(Arts, LevelDesign)
//{
//
//
//    //Travis Fix because Windows can't open a window
//    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
//    if (env != nullptr)
//    {
//        std::cout << "Test skip for travis windows" << std::endl;
//        return;
//    }
//
//    Configuration config;
//    config.windowName = "AerEditor";
//    config.windowSize = Vec2u(1400, 900);
//
//    sdl::Gles3Window window;
//    gl::Gles3Renderer renderer;
//    Filesystem filesystem;
//    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);
//
//    engine.SetWindowAndRenderer(&window, &renderer);
//
//    TestLevelDesign testRenderer(engine);
//
//    engine.RegisterSystem(testRenderer);
//    engine.RegisterOnDrawUi(testRenderer);
//    engine.Init();
//    engine.EngineLoop();
//    logDebug("Test without check");
//
//}
////#pragma endregion
#pragma region Ship
class TestShip : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface
{
public:
    explicit TestShip(AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer())
    {
    }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativeScale(testEntity_, Vec3f::one);
        cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f::forward * 10.0f);
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        ilRoso1 = SpawnIlRoso(Vec3f::right * 10.0f * -2);
        ilRoso2 = SpawnIlRoso(Vec3f::right * 10.0f * -1);
        ilRoso3 = SpawnIlRoso(Vec3f::right * 10.0f * 0);
        ilRoso4 = SpawnIlRoso(Vec3f::right * 10.0f * 1);
        cortese1 = SpawnCortese(Vec3f::right * 10.0f * 2);
        cortese2 = SpawnCortese(Vec3f::right * 10.0f * 3);
        engine_.GetCameras().moveSpeed = 1.0f;
        engine_.GetCameras().SetPosition(cameraPosition_, 0);
        testEntity_ = cortese2;
        ilRoso1Texture = rContainer_.textureManager.LoadTexture(config.dataRootPath + "models/ship/ilroso/textures/corps/blue/retopo_gros_objet_basecolor.png", Texture::DEFAULT);
        ilRoso2Texture = rContainer_.textureManager.LoadTexture(config.dataRootPath + "models/ship/ilroso/textures/corps/greyred/retopo_gros_objet_basecolor.png", Texture::DEFAULT);
        ilRoso3Texture = rContainer_.textureManager.LoadTexture(config.dataRootPath + "models/ship/ilroso/textures/corps/redblack/retopo_gros_objet_basecolor.png", Texture::DEFAULT);
        ilRoso4Texture = rContainer_.textureManager.LoadTexture(config.dataRootPath + "models/ship/ilroso/textures/corps/redwhite/retopo_gros_objet_basecolor.png", Texture::DEFAULT);
        cortese1Texture = rContainer_.textureManager.LoadTexture(config.dataRootPath + "models/ship/cortese/textures/corps_blue/low_cortese_complet_centre_basecolor.png", Texture::DEFAULT);
        cortese2Texture = rContainer_.textureManager.LoadTexture(config.dataRootPath + "models/ship/cortese/textures/corps_red/low_cortese_complet_centre_basecolor.png", Texture::DEFAULT);


    }

    Entity SpawnCortese(Vec3f pos)
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        Entity corps = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(corps);
        cContainer_.transform3dManager.SetRelativeScale(corps, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(corps, pos);
        cContainer_.renderManager.AddComponent(corps);
        cContainer_.renderManager.SetModel(
            corps, config.dataRootPath + "models/ship/cortese/corps/low_cortese_corps_resize.obj");
        Entity details = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(details);
        cContainer_.transform3dManager.SetRelativeScale(details, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(details, pos);
        cContainer_.renderManager.AddComponent(details);
        cContainer_.renderManager.SetModel(
            details, config.dataRootPath + "models/ship/cortese/details/low_cortese_elements_resize.obj");
        Entity helice_arriere = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(helice_arriere);
        cContainer_.transform3dManager.SetRelativeScale(helice_arriere, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativeRotation(helice_arriere, EulerAngles(degree_t(-90),degree_t(0),degree_t(0)));
        cContainer_.transform3dManager.SetRelativePosition(helice_arriere, pos + Vec3f(0, 1, -19) * 0.1f);
        cContainer_.renderManager.AddComponent(helice_arriere);
        cContainer_.renderManager.SetModel(
            helice_arriere, config.dataRootPath + "models/ship/cortese/helice_arriere/low_helice_arriere_origin_resize.obj");
        Entity helice_d = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(helice_d);
        cContainer_.transform3dManager.SetRelativeScale(helice_d, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(helice_d, pos + Vec3f(-16.75f, 0, 4) * 0.1f);
        cContainer_.renderManager.AddComponent(helice_d);
        cContainer_.renderManager.SetModel(
            helice_d, config.dataRootPath + "models/ship/cortese/helice_d/low_helice_d_origin_resize.obj");
        Entity helice_g = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(helice_g);
        cContainer_.transform3dManager.SetRelativeScale(helice_g, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(helice_g, pos + Vec3f(16.75f, 0, 4) * 0.1f);
        cContainer_.renderManager.AddComponent(helice_g);
        cContainer_.renderManager.SetModel(
            helice_g, config.dataRootPath + "models/ship/cortese/helice_g/low_helice_g_origin_resize.obj");
        return corps;
    }
    Entity SpawnIlRoso(Vec3f pos)
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        Entity corps = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(corps);
        cContainer_.transform3dManager.SetRelativeScale(corps, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(corps, pos);
        cContainer_.renderManager.AddComponent(corps);
        cContainer_.renderManager.SetModel(
            corps, config.dataRootPath + "models/ship/ilroso/corps/objet_central_low.obj");
        Entity details = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(details);
        cContainer_.transform3dManager.SetRelativeScale(details, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(details, pos);
        cContainer_.renderManager.AddComponent(details);
        cContainer_.renderManager.SetModel(
            details, config.dataRootPath + "models/ship/ilroso/details/details_low.obj");
        Entity helice_arriere = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(helice_arriere);
        cContainer_.transform3dManager.SetRelativeScale(helice_arriere, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativeRotation(helice_arriere, EulerAngles(degree_t(-90), degree_t(0), degree_t(0)));
        cContainer_.transform3dManager.SetRelativePosition(helice_arriere, pos + Vec3f(0, 3.75f, -20) * 0.1f);
        cContainer_.renderManager.AddComponent(helice_arriere);
        cContainer_.renderManager.SetModel(
            helice_arriere, config.dataRootPath + "models/ship/ilroso/helice_arriere/helice_arriere.obj");
        Entity helice_d = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(helice_d);
        cContainer_.transform3dManager.SetRelativeScale(helice_d, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(helice_d, pos + Vec3f(-12.5f, 3.5f, -1.5f) * 0.1f);
        cContainer_.renderManager.AddComponent(helice_d);
        cContainer_.renderManager.SetModel(
            helice_d, config.dataRootPath + "models/ship/ilroso/helice_d/helice_droit.obj");
        Entity helice_g = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(helice_g);
        cContainer_.transform3dManager.SetRelativeScale(helice_g, Vec3f::one * 0.1f);
        cContainer_.transform3dManager.SetRelativePosition(helice_g, pos + Vec3f(12.5f, 3.5f, -1.5f) * 0.1f);
        cContainer_.renderManager.AddComponent(helice_g);
        cContainer_.renderManager.SetModel(
            helice_g, config.dataRootPath + "models/ship/ilroso/helice_g/helice_gauche.obj");
        return corps;
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        if (rContainer_.textureManager.IsTextureLoaded(ilRoso1Texture)) {
            TextureName textureName = rContainer_.textureManager.GetTextureName(ilRoso1Texture);
            cContainer_.renderManager.SetDiffuseTexture(ilRoso1, textureName);
        }
        if (rContainer_.textureManager.IsTextureLoaded(ilRoso2Texture)) {
            TextureName textureName = rContainer_.textureManager.GetTextureName(ilRoso2Texture);
            cContainer_.renderManager.SetDiffuseTexture(ilRoso2, textureName);
        }
        if (rContainer_.textureManager.IsTextureLoaded(ilRoso3Texture)) {
            TextureName textureName = rContainer_.textureManager.GetTextureName(ilRoso3Texture);
            cContainer_.renderManager.SetDiffuseTexture(ilRoso3, textureName);
        }
        if (rContainer_.textureManager.IsTextureLoaded(ilRoso4Texture)) {
            TextureName textureName = rContainer_.textureManager.GetTextureName(ilRoso4Texture);
            cContainer_.renderManager.SetDiffuseTexture(ilRoso4, textureName);
        }
        if (rContainer_.textureManager.IsTextureLoaded(cortese1Texture)) {
            TextureName textureName = rContainer_.textureManager.GetTextureName(cortese1Texture);
            cContainer_.renderManager.SetDiffuseTexture(cortese1, textureName);
        }
        if (rContainer_.textureManager.IsTextureLoaded(cortese2Texture)) {
            TextureName textureName = rContainer_.textureManager.GetTextureName(cortese2Texture);
            cContainer_.renderManager.SetDiffuseTexture(cortese2, textureName);
        }

        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_ || rContainer_.modelManager.IsLoaded(modelId))
        {
            loaded_ = rContainer_.modelManager.IsLoaded(modelId);
            engine_.Stop();
        }
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;
    }

    void Render() override {}

    void Destroy() override
    {
        EXPECT_TRUE(loaded_);
    }

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 20.0f;
    bool loaded_ = false;
    AerEngine& engine_;
    Vec3f cameraPosition_ = Vec3f(0, 0, 10);

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    Entity testEntity_;

    Entity ilRoso1;
    Entity ilRoso2;
    Entity ilRoso3;
    Entity ilRoso4;
    Entity cortese1;
    Entity cortese2;
    TextureId ilRoso1Texture;
    TextureId ilRoso2Texture;
    TextureId ilRoso3Texture;
    TextureId ilRoso4Texture;
    TextureId cortese1Texture;
    TextureId cortese2Texture;
};

TEST(Arts, Ship)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr)
    {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = Vec2u(1400, 900);

    sdl::Gles3Window window;
    gl::Gles3Renderer renderer;
    Filesystem filesystem;
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestShip testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    logDebug("Test without check");

}
#pragma endregion
#pragma region Block
class TestBlock : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface
{
public:
    explicit TestBlock(AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer())
    {
    }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        for (std::size_t c = 0; c < blocksName_.size(); c++) {
            testEntity_ = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(testEntity_);
            cContainer_.transform3dManager.SetRelativeScale(testEntity_, Vec3f::one * 0.01f);
            cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f::right * 50.0f * (c - blocksName_.size()/2.0f));
            cContainer_.renderManager.AddComponent(testEntity_);
            cContainer_.renderManager.SetModel(
                testEntity_, config.dataRootPath + "models/blocks/" + blocksName_[c] + "/" + blocksName_[c] + ".obj");
        }
        engine_.GetCameras().moveSpeed = 100.0f;
        engine_.GetCameras().SetPosition(cameraPosition_, 0);
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_ || rContainer_.modelManager.IsLoaded(modelId))
        {
            loaded_ = rContainer_.modelManager.IsLoaded(modelId);
            engine_.Stop();
        }
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;
    }

    void Render() override {}

    void Destroy() override
    {
        EXPECT_TRUE(loaded_);
    }

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 20.0f;
    bool loaded_ = false;
    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    Entity testEntity_;
    Vec3f cameraPosition_ = Vec3f(0, 50, 300);

    const std::vector<std::string> blocksName_ =
    {
    "gros_block1",
    "gros_block2",
    "obstacle_grand1",
    "obstacle_grand2",
    "obstacle_moyen1",
    "obstacle_moyen2",
    "obstacle_moyen3",
    "obstacle_petit1",
    "obstacle_petit2",
    "obstacle_petit3"
    };


};

TEST(Arts, Block)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr)
    {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = Vec2u(1400, 900);

    sdl::Gles3Window window;
    gl::Gles3Renderer renderer;
    Filesystem filesystem;
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestBlock testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    logDebug("Test without check");

}
#pragma endregion 
#pragma region Totem
class TestTotem : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface
{
public:
    explicit TestTotem(AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer())
    {
    }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        for (std::size_t c = 0; c < totemNames_.size(); c++) {
            testEntity_ = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(testEntity_);
            cContainer_.transform3dManager.SetRelativeScale(testEntity_, Vec3f::one * 0.1f);
            cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f::right * 100.0f * (c - totemNames_.size() / 2.0f));
            cContainer_.renderManager.AddComponent(testEntity_);
            cContainer_.renderManager.SetModel(
                testEntity_, config.dataRootPath + "models/totems/" + totemNames_[c] + "/" + totemNames_[c] + ".obj");
        }
        engine_.GetCameras().moveSpeed = 100.0f;
        engine_.GetCameras().SetPosition(cameraPosition_, 0);
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_ || rContainer_.modelManager.IsLoaded(modelId))
        {
            loaded_ = rContainer_.modelManager.IsLoaded(modelId);
            engine_.Stop();
        }
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;
    }

    void Render() override {}

    void Destroy() override
    {
        EXPECT_TRUE(loaded_);
    }

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 20.0f;
    bool loaded_ = false;
    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    Entity testEntity_;
    Vec3f cameraPosition_ = Vec3f(0, 50, 300);

    const std::vector<std::string> totemNames_ =
    {
    "grand_totem_low",
    "totem_red",
    "totem_blue",
    "totem_green"
    };
};

TEST(Arts, Totem)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr)
    {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = Vec2u(1400, 900);

    sdl::Gles3Window window;
    gl::Gles3Renderer renderer;
    Filesystem filesystem;
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestTotem testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    logDebug("Test without check");

}
#pragma endregion
#pragma region LevelDesign
class LevelDesignViewer : public SystemInterface,
    public DrawImGuiInterface
{
public:
    explicit LevelDesignViewer(AerEngine& engine)
        : engine_(engine)
    {}

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const Configuration config = BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath + "scenes/LevelDesign05-04.aerscene");
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->position = Vec3f(10.0f, 5.0f, 0.0f);
        camera->Rotate(EulerAngles(degree_t(0.0f), degree_t(-90.0f), degree_t(0.0f)));
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        updateCount_ += dt.count();
        //if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Destroy() override {}

    void HasSucceed() {}

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 2.0f;

    AerEngine& engine_;
};
TEST(Arts, TestLevelDesignSceneViewer)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr)
    {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = Vec2u(1400, 900);

    sdl::Gles3Window window;
    gl::Gles3Renderer renderer;
    Filesystem filesystem;
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);
    LevelDesignViewer testSceneImporteur(engine);
    engine.RegisterSystem(testSceneImporteur);
    engine.RegisterOnDrawUi(testSceneImporteur);

    engine.Init();

    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Scene_Neko_Profile.prof");
#endif

    //testSceneImporteur.HasSucceed();
    logDebug("Test without check");
}
#pragma endregion
#pragma region Stb
class TestStb : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface
{
public:
    explicit TestStb(AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer())
    {
    }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        if (engine_.GetMode() != ModeEnum::TEST)
        {
            const Configuration config = BasicEngine::GetInstance()->GetConfig();
            testEntity_ = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(testEntity_);
            cContainer_.transform3dManager.SetRelativeScale(testEntity_, Vec3f::one * 100.0f);
            cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f::right * 300.0f);
            cContainer_.renderManager.AddComponent(testEntity_);
            cContainer_.renderManager.SetModel(
                testEntity_, config.dataRootPath + "models/cube/cube.obj");
            testEntity_ = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(testEntity_);
            cContainer_.transform3dManager.SetRelativeScale(testEntity_, Vec3f::one * 0.1f);
            cContainer_.renderManager.AddComponent(testEntity_);
            cContainer_.renderManager.SetModel(
                testEntity_, config.dataRootPath + "models/blocks/gros_block1/gros_block1.obj");
            engine_.GetCameras().moveSpeed = 50.0f;
            engine_.GetCameras().SetPosition(cameraPosition_, 0);
        }
        
        if (engine_.GetMode() != ModeEnum::TEST)
        {
            const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
            textureWallId_ = engine_.GetResourceManagerContainer().textureManager.LoadTexture(config.dataRootPath + "sprites/wall.jpg", Texture::DEFAULT);
            textureBlockId_ = engine_.GetResourceManagerContainer().textureManager.LoadTexture(config.dataRootPath + "models/blocks/gros_block1/pierre1_basecolor.png", Texture::DEFAULT);
            textureCubeId_ = engine_.GetResourceManagerContainer().textureManager.LoadTexture(config.dataRootPath + "models/cube/BaseColor.png", Texture::DEFAULT);

        }

    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_ || (rContainer_.modelManager.IsLoaded(modelId) && rContainer_.textureManager.IsTextureLoaded(textureWallId_)))
        {
            loadedModel_ = rContainer_.modelManager.IsLoaded(modelId);
            loadedTexture_ = rContainer_.textureManager.IsTextureLoaded(textureWallId_);
            engine_.Stop();
        }
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;
        RendererLocator::get().Render(this);
    }

    void Render() override
    {
        if (shader_.GetProgram() == 0)
        {
            return;
        }
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        shader_.Bind();
        shader_.SetInt("ourTexture", 0);//set the texture slot
        glActiveTexture(GL_TEXTURE0);//activate the texture slot
        const Texture* texture = engine_.GetResourceManagerContainer().textureManager.GetTexture(textureWallId_);
        if (texture)
        {
            glBindTexture(GL_TEXTURE_2D, texture->name);//bind texture id to texture slot
            glCheckError();
            quad_.SetOffset(Vec3f::right);
            quad_.Draw();
            glCheckError();
        }
        glBindTexture(GL_TEXTURE_2D, texture_);//bind texture id to texture slot
        glCheckError();
        quad_.SetOffset(Vec3f::down);
        quad_.Draw();
        glCheckError();
        texture = engine_.GetResourceManagerContainer().textureManager.GetTexture(textureBlockId_);
        if (texture)
        {
            glBindTexture(GL_TEXTURE_2D, texture->name);//bind texture id to texture slot
            glCheckError();
            quad_.SetOffset(Vec3f::left);
            quad_.Draw();
            glCheckError();
        }
        texture = engine_.GetResourceManagerContainer().textureManager.GetTexture(textureCubeId_);
        if (texture)
        {
            glBindTexture(GL_TEXTURE_2D, texture->name);//bind texture id to texture slot
            glCheckError();
            quad_.SetOffset(Vec3f::up);
            quad_.Draw();
            glCheckError();
        }
    }

    void Destroy() override
    {
        EXPECT_TRUE(loadedModel_);
        EXPECT_TRUE(loadedTexture_);
    }

    void DrawImGui() override
    {
    }

    void InitRenderer()
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();

        const auto& filesystem = BasicEngine::GetInstance()->GetFilesystem();
        shader_.LoadFromFile(
            config.dataRootPath + "shaders/02_hello_texture/texture.vert",
            config.dataRootPath + "shaders/02_hello_texture/texture.frag");
        rendershader_.LoadFromFile(
            config.dataRootPath + "shaders/opengl/light.vert",
            config.dataRootPath + "shaders/opengl/light.frag");
        quad_.Init();

        const auto texturePath = config.dataRootPath + "sprites/wall.jpg";
        texture_ = gl::StbCreateTexture(texturePath, filesystem);

        glCheckError();
    }

private:
    TextureName texture_ = INVALID_TEXTURE_NAME;
    TextureId textureWallId_ = INVALID_TEXTURE_ID;
    TextureId textureBlockId_ = INVALID_TEXTURE_ID;
    TextureId textureCubeId_ = INVALID_TEXTURE_ID;
    TextureName textureWall_ = INVALID_TEXTURE_NAME;
    gl::RenderQuad quad_{ Vec3f::up, Vec2f::one };
    gl::Shader shader_;
    gl::Shader rendershader_;

    float updateCount_ = 0;
    const float kEngineDuration_ = 20.0f;
    bool loadedModel_ = false;
    bool loadedTexture_ = false;
    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    Entity testEntity_;
    Vec3f cameraPosition_ = Vec3f(0, 300, 753);
};

TEST(Arts, Stb)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr)
    {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = Vec2u(1400, 900);

    sdl::Gles3Window window;
    gl::Gles3Renderer renderer;
    Filesystem filesystem;
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestStb testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.Init();
    Job initJob{ [&testRenderer]() { testRenderer.InitRenderer(); } };
    BasicEngine::GetInstance()->ScheduleJob(&initJob, JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
    logDebug("Test without check");

}
#pragma endregion 
}    // namespace neko::aer
#endif