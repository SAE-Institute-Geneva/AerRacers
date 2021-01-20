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
		EXPECT_TRUE(cContainer.transform3dManager.GetPosition(0) == Vec3f(1.0, 3.0, 5.0));
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
		EXPECT_TRUE(cContainer.transform3dManager.GetPosition(0) == Vec3f(960, 540, 0));
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
		const Configuration config = BasicEngine::GetInstance()->GetConfig();
		engine_.GetComponentManagerContainer().sceneManager.LoadScene(
			config.dataRootPath + testScene_.sceneName);
	}

	void Update(seconds) override
	{
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
	config.dataRootPath = "../../data/";
	config.windowSize   = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::TEST);

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
	config.dataRootPath = "../../data/";
	config.windowSize   = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::TEST);

	engine.SetWindowAndRenderer(&window, &renderer);
	TestUnityScene testUnity;
	SceneImporterTester testSceneImporteur(engine, testUnity);
	engine.RegisterSystem(testSceneImporteur);

	engine.Init();

	engine.EngineLoop();

	testSceneImporteur.HasSucceed();
}

class SceneExporterTester : public SystemInterface
{
public:
	explicit SceneExporterTester(AerEngine& engine) : engine_(engine) {}

	void Init() override
	{
		auto& cContainer = engine_.GetComponentManagerContainer();
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
		cContainer.transform3dManager.SetPosition(0, Vec3f(1.0, 3.0, 5.0));
	}

	void Update(seconds) override
	{
		updateCount_++;
		if (updateCount_ == kEngineDuration_)
		{
			auto& sceneManager = engine_.GetComponentManagerContainer().sceneManager;
			sceneManager.SaveCurrentScene();
			const Configuration config = BasicEngine::GetInstance()->GetConfig();
			sceneManager.LoadScene(
				config.dataRootPath + sceneManager.GetCurrentScene().sceneName + ".aerscene");
			engine_.Stop();
		}
	}

	void Destroy() override {}

	void HasSucceed()
	{
		auto& cContainer = engine_.GetComponentManagerContainer();
		EXPECT_TRUE(cContainer.sceneManager.GetCurrentScene().sceneName == "writed_scene");
		EXPECT_TRUE(cContainer.entityManager.GetEntitiesNmb() == 5);
		EXPECT_TRUE(cContainer.entityManager.GetEntityParent(1) == 0);
		EXPECT_TRUE(TagLocator::get().CompareEntitiesTag(0, 1));
		EXPECT_TRUE(TagLocator::get().IsEntityLayer(3, "TestLayer"));
		EXPECT_FALSE(TagLocator::get().IsEntityTag(1, 0));
		EXPECT_TRUE(
			cContainer.entityManager.HasComponent(0, EntityMask(ComponentType::TRANSFORM3D)));
		EXPECT_TRUE(cContainer.transform3dManager.GetPosition(0) == Vec3f(1.0, 3.0, 5.0));
	}

private:
	int updateCount_           = 0;
	const int kEngineDuration_ = 10;

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
	config.dataRootPath = "../../data/";
	config.windowSize   = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::TEST);

	engine.SetWindowAndRenderer(&window, &renderer);
	SceneExporterTester testSceneExporter(engine);
	engine.RegisterSystem(testSceneExporter);

	engine.Init();

	engine.EngineLoop();

	testSceneExporter.HasSucceed();
}
}    // namespace neko::aer
#endif
