#include "aer_engine.h"
#include "editor/tool_manager.h"
#include "editor/tool/logger.h"

namespace neko::aer {
EditorToolManager::EditorToolManager(AerEngine &engine)
  : engine_(engine) {
}

void EditorToolManager::Init() {
    ModeEnum mode = engine_.GetMode();
    if (mode == ModeEnum::EDITOR) {
        AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
    }
}


void EditorToolManager::Update(seconds dt) {
  for (auto &tool : tools_) {
    tool->Update(dt);
  }
}

void EditorToolManager::Destroy() {
  for (auto &tool : tools_) {
    tool->Destroy();
  }

  tools_.clear();
}

void EditorToolManager::DrawImGui() {
  ImGuiIO io = ImGui::GetIO();

  //Editor Menu
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Settings")) {
      DrawList();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools")) {
      DrawList();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About Neko")) {
        aboutVisible_ = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (aboutVisible_) {
    //Todo Change
    ImGui::Begin("About", &aboutVisible_);
    ImGui::Text(("Welcome to " + (engine_.config.windowName)).c_str());
    ImGui::Text("Best Editor Ever");
    ImGui::Separator();
    ImGui::Text(
        "Neko is a 3D game engine based on SDL2 and OpenGL ES 3.0 that works on Desktop,");
    ImGui::Text(
        "WebGL2 and Nintendo Switch (port to Android and iOS possible) used at SAE Institute Geneva.");
    ImGui::Text("");
    ImGui::Text("Copyright(c) 2020 SAE Institute Switzerland AG");
    ImGui::End();
  }

  for (auto &tool : tools_) {
    tool->DrawImGui();
  }
}

void EditorToolManager::DrawList() {
  //std::find_if
  for (auto &tool : tools_) {
    if (ImGui::MenuItem(tool->name_.c_str())) {
      tool->isVisible = true;
    }
  }
}

void EditorToolManager::OnEvent(const SDL_Event &event) {
  for (auto &tool : tools_) {
    tool->OnEvent(event);
  }
}

}
