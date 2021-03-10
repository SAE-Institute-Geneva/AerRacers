#include "aer/editor/tool/scene_loader.h"

#include "utils/file_utility.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
SceneLoader::SceneLoader(AerEngine& engine, ToolType type, int id, std::string_view name)
   : EditorToolInterface(engine, type, id, name),
	 sceneManager_(engine.GetComponentManagerContainer().sceneManager)
{}

void SceneLoader::Init()
{
	Configuration config = engine_.GetConfig();
	filepath_            = config.dataRootPath + "scenes/";
	engine_.GetPhysicsEngine().RegisterFixedUpdateListener(*this);
}

void SceneLoader::Update(seconds) {}

void SceneLoader::FixedUpdate(seconds)
{
	if (toSave_)
	{
		sceneManager_.SaveCurrentScene();
		toSave_ = false;
	}
}

void SceneLoader::Destroy() {}

void SceneLoader::DrawImGui()
{
	//if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(SDL_SCANCODE_S)) {    }
    //If is True Display Window
    if (isVisible)
    {
	    const std::string name = std::string(GetName()) + "##" + std::to_string(GetId());
		if (ImGui::Begin(name.c_str(), &isVisible))
		{
			LoadSceneFiles();
			if (!scenesPaths_.empty())
			{
				if (ImGui::BeginCombo("Scene", scenesPaths_[selectedSceneIndex_].c_str()))
				{
					for (std::size_t i = 0; i < scenesPaths_.size(); i++)
					{
						const std::string_view scenePath = scenesPaths_[i];
						bool isSelected = scenePath == scenesPaths_[selectedSceneIndex_];

						if (ImGui::Selectable(scenePath.data(), &isSelected))
							selectedSceneIndex_ = i;
					}

					ImGui::EndCombo();
				}

				if (ImGui::Button("LoadScene"))
					sceneManager_.LoadScene(scenesPaths_[selectedSceneIndex_]);

				if (ImGui::Button("Save Current Scene"))
				{
					if (!engine_.GetPhysicsEngine().IsPhysicRunning())
						sceneManager_.SaveCurrentScene();
					else
						toSave_ = true;
				}
			}

			ImGui::End();
		}
		else
		{
			ImGui::End();
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

void SceneLoader::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
