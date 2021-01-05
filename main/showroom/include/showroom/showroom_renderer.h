#pragma once
/*
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
 */
#include <vector>
#include <memory>

#include <SDL_events.h>
#include "sdl_engine/sdl_engine.h"

#include "showroom/camera.h"
#include "showroom/model/model.h"

namespace neko
{
class ShowRoomRenderer
		: public SystemInterface,
		public DrawImGuiInterface,
		public sdl::SdlEventSystemInterface,
		public RenderCommandInterface
{
public:
    void Init() override;
    void Update(seconds dt) override;
    void Render() override;
    void DrawImGui() override;
    void Destroy() override;

    void OnEvent(const SDL_Event& event) override;

private:
	void CreateDockableWindow();
	void DrawMenuBar();
	void DrawSceneHierarchy();
	void DrawPropertiesWindow();
	void DrawToolWindow();

	void DrawMeshImGui(sr::Mesh& mesh);
	void DrawMeshImGuiObj(sr::Mesh& mesh);
	void DrawPopups();

	Job preRender_;
	sr::TextureManager textureManager_;
	sr::ShowRoomCamera3D camera_;

	sr::Model model_;
	gl::Shader shader_;

	//Dock Window
	static const ImGuiDockNodeFlags dockspaceFlags_ =
			ImGuiDockNodeFlags_NoDockingInCentralNode |
			ImGuiDockNodeFlags_AutoHideTabBar |
			ImGuiDockNodeFlags_PassthruCentralNode;
	std::vector<std::string> popupNamesStack_;

	//Scene Window
	char searchTerm_[64] = "";
	TextureName searchIcon_ = INVALID_TEXTURE_NAME;

	//Tools Window
	float focalLength_ = 50.0f;
	float sensorSize_ = 36.0f;

	//Meshes Window
	bool popupToggled_ = false;
	XXH64_hash_t popup_ = 0;
	std::string newNameBuffer_;
};
}

namespace ImGui
{
bool ButtonCentered(std::string_view label, const ImVec2& size = ImVec2(0, 0));
void TextCentered(std::string_view text);

static int InputTextCallback(ImGuiInputTextCallbackData* data);
bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags);
}