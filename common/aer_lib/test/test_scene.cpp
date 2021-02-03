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
 Date : 28.12.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>

#include "utils/file_utility.h"
#ifdef EASY_PROFILE_USE
    #include "easy/profiler.h"
#endif

#ifdef NEKO_GLES3
	#include <engine/system.h>
	#include <gl/gles3_window.h>
	#include <gl/graphics.h>
	#include "aer/aer_engine.h"
	#include "aer/scene.h"

namespace neko::aer
{
class TestSceneInterface
{
public:
	virtual void HasSucceed(ComponentManagerContainer& cContainer) = 0;
	std::string sceneName;
};

class TestUnityScene : public TestSceneInterface
{
public:
	explicit TestUnityScene() { sceneName = "scenes/WillToolScene2021-01-11-14-05-34.aerscene"; }

	virtual void HasSucceed(ComponentManagerContainer& cContainer) override
	{
		EXPECT_TRUE(cContainer.sceneManager.GetCurrentScene().sceneName == "WillToolScene");
		EXPECT_TRUE(cContainer.entityManager.GetEntitiesNmb() == 7);
		EXPECT_TRUE(cContainer.entityManager.GetEntityParent(1) == 0);
		EXPECT_TRUE(TagLocator::get().CompareEntitiesTag(0, 1));
		EXPECT_TRUE(TagLocator::get().IsEntityLayer(0, "Ground"));
		EXPECT_TRUE(TagLocator::get().IsEntityTag(0, 0));
		EXPECT_TRUE(
			cContainer.entityManager.HasComponent(0, EntityMask(ComponentType::TRANSFORM3D)));
		EXPECT_TRUE(cContainer.entityManager.GetEntityParent(1) == 0);
		EXPECT_NEAR(cContainer.transform3dManager.GetRelativePosition(0).x,Vec3f(1.0, 3.0, 5.0).x,0.1f);
		EXPECT_NEAR(cContainer.transform3dManager.GetRelativePosition(0).y,Vec3f(1.0, 3.0, 5.0).y,0.1f);
		EXPECT_NEAR(cContainer.transform3dManager.GetRelativePosition(0).z,Vec3f(1.0, 3.0, 5.0).z,0.1f);

	}
};

class TestExampleScene : public TestSceneInterface
{
public:
	explicit TestExampleScene() { sceneName = "scenes/scene_example.scene"; }

	void HasSucceed(ComponentManagerContainer& cContainer) override
	{
		EXPECT_TRUE(TagLocator::get().CompareEntitiesTag(1, 2));
		EXPECT_TRUE(TagLocator::get().IsEntityTag(0, "0"));
		EXPECT_TRUE(TagLocator::get().IsEntityLayer(1, "5"));
		EXPECT_TRUE(
			cContainer.entityManager.HasComponent(0, EntityMask(ComponentType::TRANSFORM3D)));
		EXPECT_TRUE(cContainer.entityManager.GetEntityParent(1) == 0);
		EXPECT_TRUE(cContainer.transform3dManager.GetRelativePosition(0) == Vec3f(960, 540, 0));
	}
};

class SceneImporterTester : public SystemInterface
{
public:
	explicit SceneImporterTester(AerEngine& engine, TestSceneInterface& testScene)
	   : engine_(engine), testScene_(testScene)
	{}

	void Init() override
    {
    #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
    #endif
		const Configuration config = BasicEngine::GetInstance()->GetConfig();
        engine_.GetPhysicsEngine().StopPhysic();
		engine_.GetComponentManagerContainer().sceneManager.LoadScene(
			config.dataRootPath + testScene_.sceneName); 
	}

	void Update(seconds) override
    {
    #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
    #endif
		updateCount_++;
		if (updateCount_ == kEngineDuration_) { engine_.Stop(); }
	}

	void Destroy() override {}

	void HasSucceed() { testScene_.HasSucceed(engine_.GetComponentManagerContainer()); }

private:
	int updateCount_           = 0;
	const int kEngineDuration_ = 10;

	AerEngine& engine_;

	TestSceneInterface& testScene_;
};

TEST(Scene, TestExampleSceneImporteur)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName   = "AerEditor";
	config.windowSize   = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);
	TestExampleScene testExample;
	SceneImporterTester testSceneImporteur(engine, testExample);
	engine.RegisterSystem(testSceneImporteur);

	engine.Init();

	engine.EngineLoop();

	testSceneImporteur.HasSucceed();
}

TEST(Scene, TestUnitySceneImporteur)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName   = "AerEditor";
	config.windowSize   = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);
	TestUnityScene testUnity;
	SceneImporterTester testSceneImporteur(engine, testUnity);
	engine.RegisterSystem(testSceneImporteur);

	engine.Init();

	engine.EngineLoop();

	testSceneImporteur.HasSucceed();
}

class TestSceneExporter : public SystemInterface
{
public:
    explicit TestSceneExporter(AerEngine& engine) : engine_(engine) {}

    void Init() override
    {
        #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
        #endif
        auto& cContainer           = engine_.GetComponentManagerContainer();
        const Configuration config = BasicEngine::GetInstance()->GetConfig();
        WriteStringToFile(config.dataRootPath + "scenes/writed_scene.aerscene", "");
        cContainer.entityManager.CreateEntity();
        cContainer.entityManager.CreateEntity();
        cContainer.entityManager.CreateEntity();
        cContainer.entityManager.CreateEntity();
        cContainer.entityManager.CreateEntity();
		cContainer.sceneManager.SetSceneName("writed_scene");
		cContainer.entityManager.SetEntityParent(1, 0);
		cContainer.sceneManager.AddTag("TestTag");
		cContainer.sceneManager.AddLayer("TestLayer");
		TagLocator::get().SetEntityTag(0, "TestTag");
		TagLocator::get().SetEntityTag(1, "TestTag");
		TagLocator::get().SetEntityLayer(2, "TestLayer");
        TagLocator::get().SetEntityLayer(3, "TestLayer");
        cContainer.transform3dManager.AddComponent(0);
        cContainer.transform3dManager.AddComponent(3);
        cContainer.transform3dManager.AddComponent(2);
        cContainer.transform3dManager.SetRelativePosition(0, Vec3f(1.0, 3.0, 5.0));
        cContainer.renderManager.AddComponent(3);
        cContainer.renderManager.SetModel(3, config.dataRootPath + "models/cube/cube.obj");
        cContainer.renderManager.AddComponent(2);
        cContainer.renderManager.SetModel(2, config.dataRootPath + "models/sphere/sphere.obj");
        neko::physics::RigidStaticData rigidStatic;
        rigidStatic.colliderType = neko::physics::ColliderType::BOX;
        rigidStatic.material     = neko::physics::PhysicsMaterial {0.5f, 0.5f, 0.5f};
        cContainer.rigidStaticManager.AddRigidStatic(3, rigidStatic);
        neko::physics::RigidDynamicData rigidDynamic;
        rigidDynamic.colliderType = neko::physics::ColliderType::SPHERE;
        rigidDynamic.useGravity   = false;
        rigidDynamic.freezeRotation = Vec3<bool>(true, false, true);
        rigidDynamic.mass           = 50.0f;
        rigidDynamic.linearDamping  = 10.0f;
        cContainer.rigidDynamicManager.AddRigidDynamic(2, rigidDynamic);
	}

	void Update(seconds dt) override
    {
    #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
    #endif
		updateCount_+= dt.count();
        if (updateCount_ >= kFirstSceneDuration_ && !loaded_)
		{
			auto& sceneManager = engine_.GetComponentManagerContainer().sceneManager;
			sceneManager.SaveCurrentScene();
            engine_.GetComponentManagerContainer().entityManager.CleanEntity();
			const Configuration config = BasicEngine::GetInstance()->GetConfig();
			sceneManager.LoadScene(
				config.dataRootPath + "scenes/" + sceneManager.GetCurrentScene().sceneName + ".aerscene");
            loaded_ = true;
		}

        if (updateCount_ >= kEngineDuration_) {
            HasSucceed();
            engine_.Stop();
        }
	}

	void Destroy() override {}

	void HasSucceed()
	{
		auto& cContainer = engine_.GetComponentManagerContainer();
		//Test Scene
		EXPECT_TRUE(cContainer.sceneManager.GetCurrentScene().sceneName == "writed_scene");
		EXPECT_TRUE(cContainer.entityManager.GetEntitiesNmb() == 5);
		EXPECT_TRUE(cContainer.entityManager.GetEntityParent(1) == 0);
		EXPECT_TRUE(TagLocator::get().CompareEntitiesTag(0, 1));
		EXPECT_TRUE(TagLocator::get().IsEntityLayer(3, "TestLayer"));
		EXPECT_FALSE(TagLocator::get().IsEntityTag(1, 0));
		//Test Transform
		EXPECT_TRUE(
			cContainer.entityManager.HasComponent(0, EntityMask(ComponentType::TRANSFORM3D)));
        EXPECT_TRUE(cContainer.transform3dManager.GetRelativePosition(0) == Vec3f(1.0, 3.0, 5.0));
        EXPECT_FALSE(
            cContainer.entityManager.HasComponent(1, EntityMask(ComponentType::TRANSFORM3D)));
		//Test Renderer
        EXPECT_TRUE(
            cContainer.entityManager.HasComponent(2, EntityMask(ComponentType::MODEL)));
        EXPECT_TRUE(cContainer.rendererViewer.GetMeshName(2) == "sphere");
        EXPECT_TRUE(cContainer.entityManager.HasComponent(3, EntityMask(ComponentType::MODEL)));
        EXPECT_TRUE(cContainer.rendererViewer.GetMeshName(3) == "cube");
        EXPECT_FALSE(cContainer.entityManager.HasComponent(0, EntityMask(ComponentType::MODEL)));
		//Test Rigidbody
        EXPECT_TRUE(
            cContainer.entityManager.HasComponent(2, EntityMask(ComponentType::RIGID_DYNAMIC)));
        EXPECT_TRUE(cContainer.rigidDynamicManager.GetRigidDynamicData(2).colliderType ==
                    physics::ColliderType::SPHERE);
        EXPECT_TRUE(
            cContainer.rigidDynamicManager.GetRigidDynamicData(2).useGravity == false);
        EXPECT_FALSE(cContainer.rigidDynamicManager.GetRigidDynamicData(2).freezeRotation ==
                    Vec3<bool>(true, false, true));
        EXPECT_TRUE(cContainer.rigidDynamicManager.GetRigidDynamicData(2).mass == 50.0f);
        EXPECT_TRUE(cContainer.rigidDynamicManager.GetRigidDynamicData(2).linearDamping == 10.0f);
        EXPECT_TRUE(cContainer.entityManager.HasComponent(3, EntityMask(ComponentType::RIGID_STATIC)));
        EXPECT_TRUE(cContainer.rigidStaticManager.GetComponent(3).GetRigidStaticData().colliderType ==
                    physics::ColliderType::BOX);
        EXPECT_TRUE(
            cContainer.rigidStaticManager.GetComponent(3).GetRigidStaticData().material.bouciness == 0.5f);
        EXPECT_FALSE(cContainer.entityManager.HasComponent(0, EntityMask(ComponentType::RIGID_DYNAMIC)));
        EXPECT_FALSE(cContainer.entityManager.HasComponent(0, EntityMask(ComponentType::RIGID_STATIC)));
	}

private:
	float updateCount_           = 0;
    const float kFirstSceneDuration_ = 2.0f;
    const float kEngineDuration_ = 5.0f;

	bool loaded_ = false;

	AerEngine& engine_;
};

TEST(Scene, TestSceneExporteur)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName   = "AerEditor";
	config.windowSize   = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);
    TestSceneExporter testSceneExporter(engine);
	engine.RegisterSystem(testSceneExporter);

	engine.Init();

	engine.EngineLoop();
}

class TestUnitySceneViewer : public TestSceneInterface
{
public:
    explicit TestUnitySceneViewer()
    {
        sceneName = "scenes/PlaygroundTest2021-02-03-11-20-04.aerscene";
    }

    virtual void HasSucceed(ComponentManagerContainer& cContainer) override
    {
    }
};

class SceneViewerTester : public SystemInterface
{
public:
    explicit SceneViewerTester(AerEngine& engine, TestSceneInterface& testScene)
       : engine_(engine), testScene_(testScene)
    {}

    void Init() override
    {
    #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
    #endif
        const Configuration config = BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath + testScene_.sceneName);
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->position = Vec3f(10.0f, 5.0f, 0.0f);
        camera->Rotate(EulerAngles(degree_t(0.0f), degree_t(-90.0f), degree_t(0.0f)));
    }

    void Update(seconds dt) override
    {
    #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
    #endif
        updateCount_+=dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Destroy() override {}

    void HasSucceed() { testScene_.HasSucceed(engine_.GetComponentManagerContainer()); }

private:
    float updateCount_           = 0;
    const float kEngineDuration_ = 10.0f;

    AerEngine& engine_;

    TestSceneInterface& testScene_;
};

TEST(Scene, TestUnitySceneView)
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
    TestUnitySceneViewer testExample;
    SceneViewerTester testSceneImporteur(engine, testExample);
    engine.RegisterSystem(testSceneImporteur);

    engine.Init();

    engine.EngineLoop();
    #ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Scene_Neko_Profile.prof");
    #endif

    //testSceneImporteur.HasSucceed();
    logDebug("Test without check");
}
}    // namespace neko::aer
#endif