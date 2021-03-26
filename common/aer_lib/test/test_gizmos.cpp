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

#ifdef NEKO_GLES3
#include "engine/resource_locations.h"

#include "aer/aer_engine.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"

namespace neko::aer
{
class TestGizmos : public SystemInterface,
				   public RenderCommandInterface,
				   public DrawImGuiInterface
{
public:
	TestGizmos(AerEngine& engine,
		EntityManager& entityManager,
		Transform3dManager& transform3dManager)
	   : engine_(engine),
		 entityManager_(entityManager),
		 transform3dManager_(transform3dManager),
		 gizmosRenderer_(nullptr)
	{}

	void InitActors()
	{
		//Plane
		{
			planeEntity_ = entityManager_.CreateEntity();
			transform3dManager_.AddComponent(planeEntity_);
			transform3dManager_.SetRelativePosition(planeEntity_, planePosition_);
			transform3dManager_.SetRelativeScale(planeEntity_, Vec3f(5, 1, 5));
		}
		//Sphere
		{
			sphereEntity_ = entityManager_.CreateEntity();
			transform3dManager_.AddComponent(sphereEntity_);
			transform3dManager_.SetRelativePosition(sphereEntity_, cubePosition_);
		}
	}

	void InitRenderer()
	{
		textureManager_.Init();
		const auto& config = BasicEngine::GetInstance()->GetConfig();
		shader_.LoadFromFile(
			GetGlShadersFolderPath() + "coords.vert", GetGlShadersFolderPath() + "coords.frag");
		textureWallId_ =
			textureManager_.LoadTexture(config.dataRootPath + "sprites/wall.jpg", Texture::DEFAULT);

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
		camera_.position         = kCameraOriginPos_;
		camera_.reverseDirection = Vec3f::forward;
		camera_.fovY             = degree_t(45.0f);
		camera_.nearPlane        = 0.1f;
		camera_.farPlane         = 100.0f;
		gizmosRenderer_.SetCamera(&camera_);
	}

	void Update(seconds dt) override
	{
		auto& inputLocator = sdl::InputLocator::get();
		if (inputLocator.GetKeyState(sdl::KeyCodeType::SPACE) == sdl::ButtonState::DOWN)
		{}
		if (inputLocator.GetKeyState(sdl::KeyCodeType::KEY_LEFT_CTRL) ==
			sdl::ButtonState::DOWN)
		{}
		const auto& config = BasicEngine::GetInstance()->GetConfig();
		camera_.SetAspect(static_cast<float>(config.windowSize.x) / config.windowSize.y);
		textureManager_.Update(dt);
		RendererLocator::get().Render(this);
		RendererLocator::get().Render(&gizmosRenderer_);

		updateCount_ += dt.count();
		if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
		gizmosRenderer_.Update(dt);
	}

	void Render() override
	{
		if (shader_.GetProgram() == 0) return;
		if (textureWall_ == INVALID_TEXTURE_NAME)
		{
			textureWall_ = textureManager_.GetTextureName(textureWallId_);
			return;
		}
		shader_.Bind();
		glBindTexture(GL_TEXTURE_2D, textureWall_);
		shader_.SetMat4("view", camera_.GenerateViewMatrix());
		shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
		for (Entity entity = 0.0f; entity < entityManager_.GetEntitiesSize(); entity++)
		{
			Mat4f model = Mat4f::Identity;    //model transform matrix
			if (entityManager_.HasComponent(
					entity, EntityMask(ComponentType::TRANSFORM3D)))
			{
				model = Transform3d::Scale(model, transform3dManager_.GetRelativeScale(entity));
				model = Transform3d::Rotate(model, transform3dManager_.GetRelativeRotation(entity));
				model =
					Transform3d::Translate(model, transform3dManager_.GetRelativePosition(entity));
				shader_.SetMat4("model", model);
				if (entity == planeEntity_)
				{
					cube_.Draw();
					gizmosRenderer_.DrawCube(transform3dManager_.GetRelativePosition(entity),
                        transform3dManager_.GetRelativeScale(entity),
                        EulerAngles(degree_t(0)),
						Color::blue,
						10.0f);
				}

				if (entity == sphereEntity_)
				{
					sphere_.Draw();
					gizmosRenderer_.DrawSphere(transform3dManager_.GetRelativePosition(entity),
                        1.0f,
                        EulerAngles(degree_t(0)),
						Color::green,
						3.0f);
				}
			}
		}
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

	void DrawImGui() override {}

private:
    float updateCount_           = 0;
	const float kEngineDuration_ = 0.5f;

	AerEngine& engine_;
	EntityManager& entityManager_;
	Transform3dManager& transform3dManager_;
	GizmoRenderer gizmosRenderer_;

	Camera3D camera_;
	//Mat4f view_{ Mat4f::Identity };
	//Mat4f projection_{ Mat4f::Identity };
	EulerAngles cameraAngles_ {
		degree_t(0.0f), degree_t(0.0f), degree_t(0.0f)};
	const Vec3f kCameraOriginPos_ = Vec3f(0.0f, 0.0f, 3.0f);
	const EulerAngles kCameraOriginAngles_ =
		EulerAngles(degree_t(0.0f), degree_t(0.0f), degree_t(0.0f));

	gl::RenderCuboid cube_ {Vec3f::zero, Vec3f::one};
	gl::RenderQuad quad_ {Vec3f::zero, Vec2f::one};

	gl::RenderCircle circle_ {Vec3f::zero, 1.0f};
	gl::RenderSphere sphere_ {Vec3f::zero, 1.0f};
	gl::RenderWireFrameSphere wireFrameSphere_ {Vec3f::zero, 1.0f};

	const static size_t kCubeNumbers        = 10;
	Vec3f cubePosition_               = Vec3f(0.0f, 2.0f, -5.0f);
	Vec3f planePosition_              = Vec3f(0.0f, -3.0f, -5.0f);
	Vec3f cubePositions[kCubeNumbers]       = {
        Vec3f(0.0f, 0.0f, 0.0f),
        Vec3f(2.0f, 5.0f, -15.0f),
        Vec3f(-1.5f, -2.2f, -2.5f),
        Vec3f(-3.8f, -2.0f, -12.3f),
        Vec3f(2.4f, -0.4f, -3.5f),
        Vec3f(-1.7f, 3.0f, -7.5f),
        Vec3f(1.3f, -2.0f, -2.5f),
        Vec3f(1.5f, 2.0f, -2.5f),
        Vec3f(1.5f, 0.2f, -1.5f),
        Vec3f(-1.3f, 1.0f, -1.5f),
    };
	gl::Shader shader_;
	gl::TextureManager textureManager_;
	TextureName textureWall_ = INVALID_TEXTURE_NAME;
	TextureId textureWallId_;
	seconds timeSinceInit_ {0};

	Entity sphereEntity_;
	Entity planeEntity_;
};

TEST(Engine, Gizmos)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	// config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::TEST);

	engine.SetWindowAndRenderer(&window, &renderer);

	EntityManager entityManager;
	Transform3dManager transform3dManager = Transform3dManager(entityManager);
	TestGizmos textGizmos(engine, entityManager, transform3dManager);

	engine.RegisterSystem(textGizmos);
	engine.RegisterOnDrawUi(textGizmos);
	engine.Init();
	Job initJob {[&textGizmos]() { textGizmos.InitRenderer(); }};
	BasicEngine::GetInstance()->ScheduleJob(&initJob, JobThreadType::RENDER_THREAD);
	engine.EngineLoop();
    logDebug("Test without check");
}
}    // namespace neko
#endif