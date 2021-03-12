#include "aer/editor/tool/scene_loader.h"

#include "utils/file_utility.h"
#include "utils/imgui_utility.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
SceneLoader::SceneLoader(AerEngine& engine)
   : EditorToolInterface(engine), sceneManager_(engine.GetComponentManagerContainer().sceneManager)
{}

void SceneLoader::Init()
{
	Configuration config = engine_.GetConfig();
	filepath_            = config.dataRootPath + "scenes/";
	engine_.GetPhysicsEngine().RegisterFixedUpdateListener(*this);
}

void SceneLoader::FixedUpdate(seconds)
{
	if (toSave_)
	{
		sceneManager_.SaveCurrentScene();
		toSave_ = false;
	}
}

void SceneLoader::DrawImGui()
{
	LoadSceneFiles();
	if (!scenesPaths_.empty())
	{
		ImGui::Combo("Scene", &selectedSceneIndex_, &scenesPaths_);
		if (ImGui::Button("LoadScene")) sceneManager_.LoadScene(scenesPaths_[selectedSceneIndex_]);
		if (ImGui::Button("Save Current Scene"))
		{
			if (!engine_.GetPhysicsEngine().IsPhysicRunning()) sceneManager_.SaveCurrentScene();
			else toSave_ = true;
		}
	}
}

void SceneLoader::LoadSceneFiles()
{
	scenesPaths_.clear();
	IterateDirectory(
		filepath_,
		[this](const std::string_view path) { scenesPaths_.emplace_back(path.data()); },
		true);
}
}    // namespace neko::aer
