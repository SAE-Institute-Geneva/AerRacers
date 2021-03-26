#include <gtest/gtest.h>

#include "engine/resource_locations.h"

#ifdef NEKO_GLES3
#include "gl/graphics.h"
#else
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"
#endif

#include "aer/aer_engine.h"

namespace neko::aer
{
class TestFmod : public SystemInterface, public DrawImGuiInterface
{
public:
	TestFmod(AerEngine& engine)
	   : engine_(engine), cContainer_(engine.GetComponentManagerContainer())
	{
		Init();

		engine.RegisterOnDrawUi(*this);
	}

	void Init() override
	{
		// Models
		cContainer_.entityManager.CreateEntity(0);
		cContainer_.entityManager.CreateEntity(1);
		cContainer_.entityManager.CreateEntity(2);
		cContainer_.entityManager.CreateEntity(3);

		cContainer_.transform3dManager.AddComponent(0);
		cContainer_.transform3dManager.SetGlobalPosition(0, Vec3f::zero * 3.0f);
		cContainer_.transform3dManager.AddComponent(1);
		cContainer_.transform3dManager.SetGlobalPosition(1, Vec3f::right * 3.0f);
		cContainer_.transform3dManager.AddComponent(2);
		cContainer_.transform3dManager.SetGlobalPosition(2, Vec3f::left * 3.0f);
		cContainer_.transform3dManager.AddComponent(3);
		cContainer_.transform3dManager.SetGlobalPosition(3, Vec3f::up * 3.0f);

		cContainer_.renderManager.AddComponent(0);
		cContainer_.renderManager.SetModel(0, GetModelsFolderPath() + "sphere/sphere.obj");
		cContainer_.renderManager.AddComponent(1);
		cContainer_.renderManager.SetModel(1, GetModelsFolderPath() + "cube/cube.obj");
		cContainer_.renderManager.AddComponent(2);
		cContainer_.renderManager.SetModel(2, GetModelsFolderPath() + "cube/cube.obj");
		cContainer_.renderManager.AddComponent(3);
		cContainer_.renderManager.SetModel(3, GetModelsFolderPath() + "cube/cube.obj");

		// Audio Sources
		cContainer_.entityManager.CreateEntity(4);

		cContainer_.transform3dManager.AddComponent(4);
		cContainer_.transform3dManager.SetGlobalPosition(4, Vec3f::up * 1.5f + Vec3f::right * 2.5f);
		cContainer_.transform3dManager.SetGlobalScale(4, Vec3f::one * 0.1f);

		cContainer_.renderManager.AddComponent(4);
		cContainer_.renderManager.SetModel(4, GetModelsFolderPath() + "sphere/sphere.obj");

		cContainer_.audioManager.AddComponent(4);
		cContainer_.audioManager.SetEventName(4, "sfx/ship_engine");
		cContainer_.audioManager.SetPlayOnWakeUp(4, true);
		cContainer_.audioManager.SetMaxDistance(4, 40.0f);
		cContainer_.audioManager.SetVolume(4, 50.0f);

		cContainer_.audioManager.Init();
	}

	void Update(seconds dt) override
	{
		updateCount_ += dt.count();

		// Audio Sources
		const float speed = InverseLerp(0.0, maxSpeed_, speed1_) * 100.0f;
		cContainer_.audioManager.SetParameter(4, "Speed", speed);

		Vec3f position = cContainer_.transform3dManager.GetGlobalPosition(4);
		position       = Quaternion::AngleAxis(degree_t(speed1_), Vec3f::up) * position;
		cContainer_.transform3dManager.SetGlobalPosition(4, position);

		if (updateCount_ >= engineDuration_)
		{
			HasSucceed();
			engine_.Stop();
		}
	}

	void Destroy() override {}

	void DrawImGui() override
	{
		using namespace ImGui;
		Begin("Audio Source Speeds");
		DragFloat("Engine Source Speed", &speed1_, LabelPos::RIGHT, 0.001f, 0.0f, MAXFLOAT);
		End();
	}

	void HasSucceed() {}

private:
	const float engineDuration_ = 10.0f;
	float updateCount_ = 0;

	float speed1_ = 0.05f;
	float maxSpeed_ = 0.1f;

	AerEngine& engine_;
	ComponentManagerContainer& cContainer_;
};

TEST(Fmod, TestFmod)
{
	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1280, 720);
	config.flags      = Configuration::NONE;

	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::EDITOR);
#ifdef NEKO_GLES3
	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
#elif NEKO_VULKAN
	sdl::VulkanWindow window;
	vk::VkRenderer renderer(&window);
	renderer.SetRenderer(std::make_unique<neko::vk::RendererEditor>());
#endif

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();
	TestFmod testFmod(engine);
	engine.RegisterSystem(testFmod);
	engine.EngineLoop();
}
}    // namespace neko::aer