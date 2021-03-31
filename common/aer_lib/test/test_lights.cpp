#include <gtest/gtest.h>

#include "engine/resource_locations.h"

#ifdef NEKO_GLES3
#include "gl/graphics.h"
#else
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"
#endif

#include "aer/aer_engine.h"
#include "gl/gles3_window.h"

namespace neko::aer
{
class TestLights : public SystemInterface, public DrawImGuiInterface
{
public:
	TestLights(AerEngine& engine)
	   : engine_(engine), cContainer_(engine.GetComponentManagerContainer())
	{
		Init();
		DirectionalLight* dirLight = DirectionalLight::Instance;
		dirLight->diffuse = Color3(Color::green);

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

		// Lights
		cContainer_.entityManager.CreateEntity(4);
		cContainer_.entityManager.CreateEntity(5);

		cContainer_.transform3dManager.AddComponent(4);
		cContainer_.transform3dManager.SetGlobalPosition(4, Vec3f::up * 1.5f + Vec3f::right * 2.5f);
		cContainer_.transform3dManager.SetGlobalScale(4, Vec3f::one * 0.1f);
		cContainer_.transform3dManager.AddComponent(5);
		cContainer_.transform3dManager.SetGlobalPosition(5, Vec3f::up * 1.5f - Vec3f::one * 2.5f);
		cContainer_.transform3dManager.SetGlobalScale(5, Vec3f::one * 0.1f);

		cContainer_.renderManager.AddComponent(4);
		cContainer_.renderManager.SetModel(4, GetModelsFolderPath() + "sphere/sphere.obj");
		cContainer_.renderManager.AddComponent(5);
		cContainer_.renderManager.SetModel(5, GetModelsFolderPath() + "sphere/sphere.obj");

		cContainer_.lightManager.AddComponent(4);
		cContainer_.lightManager.SetDiffuse(4, Vec3f(Color::blue));
		cContainer_.lightManager.AddComponent(5);
		cContainer_.lightManager.SetDiffuse(5, Vec3f(Color::red));
	}

	void Update(seconds dt) override
	{
		updateCount_ += dt.count();

		// Directional Light
		DirectionalLight* dirLight = DirectionalLight::Instance;
		dirLight->direction =
			Quaternion::AngleAxis(degree_t(speed1_), Vec3f::up) * dirLight->direction;

		// Point Light
		Vec3f position = cContainer_.transform3dManager.GetGlobalPosition(4);
		position       = Quaternion::AngleAxis(degree_t(speed2_), Vec3f::one) * position;
		cContainer_.transform3dManager.SetGlobalPosition(4, position);

		position = cContainer_.transform3dManager.GetGlobalPosition(5);
		position = Quaternion::AngleAxis(degree_t(speed3_), -Vec3f::one) * position;
		cContainer_.transform3dManager.SetGlobalPosition(5, position);

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
		Begin("Light Speeds");
		DragFloat("Dir Light Speed", &speed1_, LabelPos::RIGHT, 0.001f, 0.0f, MAXFLOAT);
		DragFloat("Blue Light Speed", &speed2_, LabelPos::RIGHT, 0.001f, 0.0f, MAXFLOAT);
		DragFloat("Red Light Speed", &speed3_, LabelPos::RIGHT, 0.001f, 0.0f, MAXFLOAT);
		End();
	}

	void HasSucceed() {}

private:
	const float engineDuration_ = 10.0f;
	float updateCount_ = 0;

	float speed1_ = 0.025f;
	float speed2_ = 0.01f;
	float speed3_ = 0.1f;

	AerEngine& engine_;
	ComponentManagerContainer& cContainer_;
};

TEST(Lights, TestDirLight)
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
	TestLights testLights(engine);
	engine.RegisterSystem(testLights);
	engine.EngineLoop();
}
}    // namespace neko::aer