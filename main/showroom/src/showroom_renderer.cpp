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
#include <imgui.h>
#include <imgui_internal.h>
#include "showroom/showroom_renderer.h"

namespace neko
{
void ShowRoomRenderer::Init()
{
	TextureManagerLocator::provide(&textureManager_);
	BasicEngine::GetInstance()->RegisterSystem(textureManager_);

	ImGuiIO &io = ImGui::GetIO();
	float fontSizeInPixels = 16;

	const auto& config = BasicEngine::GetInstance()->config;
	io.Fonts->AddFontFromFileTTF((config.dataRootPath + "showroom/droid_sans.ttf").c_str(), fontSizeInPixels);

	preRender_ = Job([this, config]
	{
		searchIcon_ = gl::stbCreateTexture(config.dataRootPath + "showroom/icons/search.png");
	    shader_.LoadFromFile(config.dataRootPath + "showroom/shaders/diffuse.vert",
	                         config.dataRootPath + "showroom/shaders/diffuse.frag");

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	});

	camera_.Init();
	camera_.position = Vec3f(13.0f, 12.0f, 6.0f);
	camera_.fovY = 2 * Atan(0.5f * sensorSize_ / focalLength_);
	camera_.nearPlane = 0.001f;
	camera_.farPlane = 1'000.0f;
	camera_.WorldLookAt(Vec3f::zero);
	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void ShowRoomRenderer::Update(seconds dt)
{
	camera_.Update(dt);

	RendererLocator::get().Render(this);
}

void ShowRoomRenderer::Render()
{
	if (searchIcon_ == INVALID_TEXTURE_NAME) return;
	if (!model_.IsLoaded()) return;

	shader_.Bind();
	shader_.SetMat4("model", Mat4f::Identity);
	shader_.SetMat4("view", camera_.GenerateViewMatrix());
	shader_.SetMat4("projection", camera_.GenerateProjectionMatrix());
	model_.Draw(shader_);
}

void ShowRoomRenderer::Destroy()
{
	model_.Destroy();
	shader_.Destroy();
}

void ShowRoomRenderer::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}

void ShowRoomRenderer::DrawImGui()
{
	using namespace ImGui;
	CreateDockableWindow();
	DrawSceneHierarchy();
	DrawPropertiesWindow();
	DrawToolWindow();
	DrawPopups();
}

void ShowRoomRenderer::CreateDockableWindow()
{
	using namespace ImGui;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	SetNextWindowPos(viewport->GetWorkPos());
	SetNextWindowSize(viewport->GetWorkSize());
	SetNextWindowViewport(viewport->ID);
	PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	Begin("Showroom", reinterpret_cast<bool*>(true), windowFlags);
	{
		PopStyleVar();
		PopStyleVar(2);

		ImGuiIO& io = GetIO();
		ImGuiID dockspaceId = GetID("ShowroomDockSpace");
		DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags_);

		DrawMenuBar();
		End();
	}
}

void ShowRoomRenderer::DrawMenuBar()
{
	using namespace ImGui;
	BeginMenuBar();
	{
		//File Tab
		if (BeginMenu("File"))
		{
			//Open Model
			if(MenuItem("Open..."))
			{
#ifdef _WIN32
#elif linux
				char filename[1024];
				FILE *f = popen("zenity --file-selection --file-filter='Model files (fbx, obj) | *.fbx *.obj'", "r");
				fgets(filename, 1024, f);
				if (!std::string(filename).empty())
				{
					strtok(filename, "\n");

					if (model_.IsLoaded()) model_.Destroy();
					model_.LoadModel(filename);

					if (GetFilenameExtension(filename) == ".obj")
					{
						popupNamesStack_.emplace_back("ObjWarning");
					}
				}
#endif
			}

			Separator();

			//Save Model
			if(MenuItem("Save"))
			{
			}

			//Save Model As
			if(MenuItem("Save As..."))
			{
#ifdef _WIN32
#elif linux
				char filename[1024];
				FILE *f = popen("zenity --file-selection --save --confirm-overwrite "
					"--file-filter='Model files (fbx, obj) | *.fbx *.obj'", "r");
				fgets(filename, 1024, f);
#endif
			}

			Separator();

			if(MenuItem("Quit"))
			{
			}
			EndMenu();
		}

		//Edit Tab
		if (BeginMenu("Edit"))
		{
			EndMenu();
		}

		//View Tab
		if (BeginMenu("View"))
		{
			EndMenu();
		}

		//Help Tab
		if (BeginMenu("Help"))
		{
			if(MenuItem("About"))
			{
				popupNamesStack_.emplace_back("About");
			}
			EndMenu();
		}
		EndMenuBar();
	}

	BeginMenuBar();
	{
		EndMenuBar();
	}
}

void ShowRoomRenderer::DrawSceneHierarchy()
{
	using namespace ImGui;
	if (Begin("Scene"))
	{
		PushItemWidth(-1);
		const float height = GetTextLineHeightWithSpacing();
		ImGui::Image(reinterpret_cast<ImTextureID>(searchIcon_), ImVec2(height, height)); SameLine();
		InputText("##Search", searchTerm_, IM_ARRAYSIZE(searchTerm_));

		PopItemWidth(); Separator();

		SetCursorPosX(ImGui::GetCursorPosX() - 8.0f);
		SetCursorPosY(ImGui::GetCursorPosY());
		BeginChild("Meshes");
		{
			SetNextItemOpen(true);
			if (ImGui::TreeNodeEx("Root Node", ImGuiTreeNodeFlags_SpanAvailWidth))
			{
				auto& meshes = model_.GetMeshes();
				for (auto& mesh : meshes)
				{
					if (mesh.GetParentName() == 0)
					{
						if (GetFilenameExtension(model_.GetPath()) == ".obj")
							DrawMeshImGuiObj(mesh);
						else
							DrawMeshImGui(mesh);
					}
				}
				TreePop();
			}
			EndChild();
		}
		End();
	}
}

void ShowRoomRenderer::DrawPropertiesWindow()
{
	using namespace ImGui;
	if (Begin("Properties"))
	{
		const auto& meshes = model_.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& textures = mesh.GetTextures();
			for (const auto& texture : textures)
			{
				Texture tex = textureManager_.GetTexture(texture.textureId);
				ImGui::Image(reinterpret_cast<ImTextureID>(tex.name), ImVec2(20, 20));
			}
		}
		End();
	}
}

void ShowRoomRenderer::DrawToolWindow()
{
	using namespace ImGui;
	if (Begin("Tools"))
	{
		if (BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (BeginTabItem("View"))
			{
				if (DragFloat("Focal Length", &focalLength_, 1.0f, 1.0f, 5000.0f, "%.4f", ImGuiSliderFlags_Logarithmic))
					camera_.fovY = 2 * Atan(0.5f * sensorSize_ / focalLength_);

				PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
				DragFloat("Clip Start", &camera_.nearPlane, 1.0f, 0.001f, 5000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
				PopStyleVar();
				DragFloat("Clip End", &camera_.farPlane, 1.0f, 0.001f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_Logarithmic);

				EndTabItem();
			}
			EndTabBar();
		}
		End();
	}
}

void ShowRoomRenderer::DrawMeshImGui(sr::Mesh& mesh)
{
	using namespace ImGui;
	std::vector<sr::Mesh*> children;
	for (auto& modelMesh : model_.GetMeshes())
		if (modelMesh.GetParentName() == HashString(mesh.GetName()))
			children.push_back(&modelMesh);

	const std::string popupName = std::string("RenameNode##").append(std::to_string(mesh.GetVAO()));
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (children.empty())
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	const bool nodeOpen = TreeNodeEx(mesh.GetName().data(), nodeFlags);
	if (ImGuiLastItemDataBackup().LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect && IsMouseDoubleClicked(0))
	{
		OpenPopup(popupName.c_str());
		popupToggled_ = true;
		popup_ = HashString(popupName);
	}

	const bool lastState = popupToggled_;
	if (BeginPopup(popupName.c_str()))
	{
		newNameBuffer_ = std::string(mesh.GetName());
		if (InputText("##RenameNode", &newNameBuffer_, ImGuiInputTextFlags_EnterReturnsTrue))
			CloseCurrentPopup();

		EndPopup();
	}
	else if (popup_ == HashString(popupName)) popupToggled_ = false;

	if (!popupToggled_ && lastState)
	{
		for (auto& aMesh : model_.GetMeshes())
			if (aMesh.GetParentName() == HashString(mesh.GetName()))
				aMesh.SetParentName(newNameBuffer_);

		mesh.SetName(newNameBuffer_);
	}

	if (nodeOpen && !children.empty())
	{
		for (auto& child : children)
			DrawMeshImGui(*child);

		TreePop();
	}
}

void ShowRoomRenderer::DrawMeshImGuiObj(sr::Mesh& mesh)
{
	using namespace ImGui;
	const std::string popupName = std::string("RenameNode##").append(std::to_string(mesh.GetVAO()));
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	TreeNodeEx(mesh.GetName().data(), nodeFlags);
	if (ImGuiLastItemDataBackup().LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect && IsMouseDoubleClicked(0))
	{
		OpenPopup(popupName.c_str());
		popupToggled_ = true;
		popup_ = HashString(popupName);
	}

	const bool lastState = popupToggled_;
	if (BeginPopup(popupName.c_str()))
	{
		newNameBuffer_ = std::string(mesh.GetName());
		if (InputText("##RenameNode", &newNameBuffer_, ImGuiInputTextFlags_EnterReturnsTrue))
			CloseCurrentPopup();

		EndPopup();
	}
	else if (popup_ == HashString(popupName)) popupToggled_ = false;

	if (!popupToggled_ && lastState)
		mesh.SetName(newNameBuffer_);
}

void ShowRoomRenderer::DrawPopups()
{
	using namespace ImGui;
	for (auto it = popupNamesStack_.begin(); it != popupNamesStack_.end();)
	{
		OpenPopup(popupNamesStack_[it - popupNamesStack_.begin()].c_str());
		it = popupNamesStack_.erase(it);
	}

	if (BeginPopupModal("ObjWarning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		TextCentered("Warning: Obj format does not supported mesh hierarchy");
		Separator();
		if (ButtonCentered("OK", ImVec2(100, 0))) CloseCurrentPopup();

		EndPopup();
	}

	if (BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		TextCentered("NekoShowroom");
		TextCentered("Version 0.1");
		NewLine();
		TextCentered("Copyright © 2020-2020");
		TextCentered("SAE Institute Geneva");

		Separator();
		if (ButtonCentered("OK", ImVec2(100, 0))) CloseCurrentPopup();

		EndPopup();
	}
}
}

bool ImGui::ButtonCentered(std::string_view label, const ImVec2& size)
{
	float framePaddingX = GetStyle().FramePadding.x;
	float textSize = CalcTextSize(label.data()).x;

	NewLine();
	if (size.x != 0) SameLine(GetWindowSize().x / 2 - size.x + (size.x / 2));
	else SameLine(GetWindowSize().x / 2 - textSize + (textSize / 2) - framePaddingX);
	return Button(label.data(), size);
}

void ImGui::TextCentered(std::string_view text)
{
	float textSize = CalcTextSize(text.data()).x;

	NewLine();
	SameLine(GetWindowSize().x / 2 - textSize + (textSize / 2));
	Text("%s", text.data());
}

int ImGui::InputTextCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		auto* str = (std::string*)data->UserData;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}

	return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
{
	flags |= ImGuiInputTextFlags_CallbackResize;
	return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, (void*)str);
}
