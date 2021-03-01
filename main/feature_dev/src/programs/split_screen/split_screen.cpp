#include "programs/split_screen/split_screen.h"

#include "engine/engine.h"
#include "engine/log.h"

namespace neko::dev
{
void SplitScreen::Init()
{
	const auto& config = BasicEngine::GetInstance()->GetConfig();
	shader_.LoadFromFile(config.dataRootPath + "shaders/opengl/base.vert",
		config.dataRootPath + "shaders/opengl/base.frag");
	shader_.BindUbo(2 * sizeof(Mat4f));
	cube_.Init();

	camera_.Init();
	camera_.SetPosition(Vec3f::one * 3.0f, 0);
	camera_.SetPosition(Vec3f::right * 3.0f, 1);
	camera_.SetPosition(Vec3f::one * 3.0f + Vec3f::left, 2);
	camera_.SetPosition(Vec3f::down * 3.0f + Vec3f::back, 3);
	camera_.WorldAllLookAt(Vec3f::zero);

	glEnable(GL_DEPTH_TEST);
	glCheckError();
}

void SplitScreen::Update(seconds dt)
{
	std::lock_guard<std::mutex> lock(updateMutex_);
	timeSinceInit_ += dt;

	camera_.Update(dt);
}

void SplitScreen::Render()
{
	std::lock_guard<std::mutex> lock(updateMutex_);

	const auto& size = BasicEngine::GetInstance()->GetConfig().windowSize;
	switch (playerNum_)
	{
		case 1:
		{
			camera_.SetAspects(size.x, size.y);

			const Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			const Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x, size.y);
			RenderScene();
			break;
		}
		case 2:
		{
			camera_.SetAspects(size.x / 2.0f, size.y);

			// Left
			Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x / 2.0f, size.y);
			RenderScene();

			// Right
			camProj = camera_.GenerateProjectionMatrix(1);
			camView = camera_.GenerateViewMatrix(1);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, 0, size.x / 2.0f, size.y);
			RenderScene();
			break;
		}
		case 3:
		{
			camera_.SetAspects(size.x / 2.0f, size.y / 2.0f);

			// Top Left
			Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene();

			// Top Right
			camProj = camera_.GenerateProjectionMatrix(1);
			camView = camera_.GenerateViewMatrix(1);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene();

			// Bottom Left
			camProj = camera_.GenerateProjectionMatrix(2);
			camView = camera_.GenerateViewMatrix(2);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x / 2.0f, size.y / 2.0f);
			RenderScene();
			break;
		}
		case 4:
		{
			camera_.SetAspects(size.x / 2.0f, size.y / 2.0f);

			// Top Left
			Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene();

			// Top Right
			camProj = camera_.GenerateProjectionMatrix(1);
			camView = camera_.GenerateViewMatrix(1);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene();

			// Bottom Left
			camProj = camera_.GenerateProjectionMatrix(2);
			camView = camera_.GenerateViewMatrix(2);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x / 2.0f, size.y / 2.0f);
			RenderScene();

			// Bottom Right
			camProj = camera_.GenerateProjectionMatrix(3);
			camView = camera_.GenerateViewMatrix(3);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, 0, size.x / 2.0f, size.y / 2.0f);
			RenderScene();
			break;
		}
		case 0:
		default: LogError("Invalid Player number!!"); break;
	}
}

void SplitScreen::Destroy() {}

void SplitScreen::DrawImGui()
{
	using namespace ImGui;
	Begin("Parameters");
	{
		PushItemWidth(-1);
		Text("Player Number");
		SameLine();
		DragInt("##playerNum_", reinterpret_cast<int*>(&playerNum_), 0.05f, 1, 4);
		PopItemWidth();
	}
	End();
}

void SplitScreen::OnEvent(const SDL_Event& event) { camera_.OnEvent(event); }

void SplitScreen::RenderScene() const
{
	shader_.Bind();
	shader_.SetMat4("model", Mat4f::Identity);
	cube_.Draw();
}
}    // namespace neko::dev
