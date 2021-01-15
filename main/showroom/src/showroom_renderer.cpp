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
#include "imgui_internal.h"

#include "showroom/showroom_renderer.h"

#ifdef _WIN32
    #include <commdlg.h>
#endif

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
		folderIcon_ = gl::stbCreateTexture(config.dataRootPath + "showroom/icons/folder.png");
		deleteIcon_ = gl::stbCreateTexture(config.dataRootPath + "showroom/icons/delete.png");
	    shader_.LoadFromFile(
			config.dataRootPath + "showroom/shaders/light.vert",
			config.dataRootPath + "showroom/shaders/light.frag");

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

	if (model_.IsLoaded())
	{
		auto& meshes = model_.GetMeshes();
		for (auto& mesh : meshes) mesh.UpdateTextures();
	}

	ImGui::GetIO().WantCaptureKeyboard = true;
	RendererLocator::get().Render(this);
}

void ShowRoomRenderer::Render()
{
	if (searchIcon_ == INVALID_TEXTURE_NAME) return;
	if (!model_.IsLoaded()) return;

	auto& meshes = model_.GetMeshes();
	UpdateShader(shader_);
	for (auto& mesh : meshes)
	{
		mesh.Draw(shader_);
	}
}

void ShowRoomRenderer::UpdateShader(const gl::Shader& shader)
{
	shader.Bind();

	const Mat4f camView = camera_.GenerateViewMatrix();
	const Mat4f camProj = camera_.GenerateProjectionMatrix();

	shader.SetMat4("view", camView);
	shader.SetMat4("proj", camProj);
	shader.SetMat4("model", modelMat_);
	shader.SetMat3("normalMatrix", Mat3f(modelMat_).Inverse().Transpose());

	shader.SetVec3("viewPos", camera_.position);
	shader.SetUInt("lightType", lightType_);

	shader.SetVec3("light.position", pointLight_.position);
	shader.SetVec3("light.direction", directionalLight_.direction.Normalized());
	shader.SetVec3("light.ambient", pointLight_.ambient);
	shader.SetVec3("light.diffuse", pointLight_.diffuse);
	shader.SetFloat("light.specular", pointLight_.specular);
	shader.SetFloat("light.intensity", pointLight_.intensity);

	shader.SetFloat("sLight.blend", Cos(spotLight_.angle * spotLight_.blend));
	shader.SetFloat("sLight.angle", Cos(spotLight_.angle));
	shader.SetFloat("sLight.radius", spotLight_.radius);
}

void ShowRoomRenderer::Destroy()
{
	model_.Destroy();
	textureManager_.Destroy();
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
	DrawImGuizmo();

	//ShowDemoWindow();
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

		ImGuiID dockspaceId = GetID("ShowroomDockSpace");
		DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags_);

		DrawMenuBar();
		ImGuizmo::SetDrawlist();

		End();
	}
}

void ShowRoomRenderer::DrawImGuizmo()
{
	const Mat4f camView = camera_.GenerateViewMatrix();
	const Mat4f camProj = camera_.GenerateProjectionMatrix();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	switch (selectedNode_)
	{
		case NONE: break;
		case LIGHT:
		{
			Mat4f lightModel = Transform3d::RotationMatrixFrom(lightOrientation_);
			lightModel = Transform3d::Translate(lightModel, pointLight_.position);
			if (ImGuizmo::Manipulate(
				&camView[0][0],
				&camProj[0][0],
				manipulateOperation_,
				ImGuizmo::MODE::LOCAL,
				&lightModel[0][0]))
			{
				Vec3f pos, scale;
				EulerAngles rot;
				lightModel.Decompose(pos, rot, scale);

				pointLight_.position = pos;
				spotLight_.position = pointLight_.position;

				lightOrientation_ = rot;
				directionalLight_.direction = Quaternion::FromEuler(-rot) * Vec3f::up;
				spotLight_.direction = directionalLight_.direction;
			}
			break;
		}
		case MODEL:
		{
			if (!model_.IsLoaded()) break;
			ImGuizmo::Manipulate(
				&camView[0][0],
				&camProj[0][0],
				manipulateOperation_,
				ImGuizmo::MODE::LOCAL,
				&modelMat_[0][0]);
			break;
		}
		default:
		{
			if (!model_.IsLoaded()) break;
			auto& mesh = model_.GetMesh(selectedNode_ - selectionOffset_);
			Mat4f modelMat = mesh.GetModelMatrix();
			if (ImGuizmo::Manipulate(
				&camView[0][0],
				&camProj[0][0],
				manipulateOperation_,
				ImGuizmo::MODE::LOCAL,
				&modelMat[0][0]))
				mesh.SetModelMatrix(modelMat);
			break;
		}
	}
}

void ShowRoomRenderer::DrawMenuBar()
{
	using namespace ImGui;
	BeginMainMenuBar();
	{
		//File Tab
		if (BeginMenu("File"))
		{
			//Open Model
			if (MenuItem("Open..."))
			{
				char filename[1024];
#ifdef _WIN32
				OPENFILENAME ofn;
				ZeroMemory(&filename, sizeof(filename));
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner   = nullptr;
				ofn.lpstrFilter = "Model files (*.fbx;*.obj;*.blend)\0*.fbx;*.obj;*.blend\0";
				ofn.lpstrFile   = filename;
				ofn.nMaxFile    = IM_ARRAYSIZE(filename);
				ofn.Flags       = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

				if (GetOpenFileNameA(&ofn))
				{
                    const std::string path = filename;
#elif linux
				FILE* f = popen("zenity --file-selection --file-filter='Model files "
								"(fbx, obj, blend) | *.fbx *.obj *.blend' --title='Open...'", "r");
				fgets(filename, 1024, f);

				std::string path = filename;
				if (!path.empty())
				{
					path = path.substr(0, path.size() - 1);
#endif
					if (model_.IsLoaded())
					{
						model_.Destroy();
						textureManager_.Destroy();
					}
					model_.LoadModel(path);

					if (GetFilenameExtension(path) == ".obj")
					{
						popupNamesStack_.emplace_back("ObjWarning");
					}
				}
			}

			Separator();

			//Save Model
			if(MenuItem("Save"))
			{
				popupNamesStack_.emplace_back("NoFeature");
			}

			//Save Model As
            if (MenuItem("Save As..."))
            {
				popupNamesStack_.emplace_back("NoFeature");
	            /*char filename[1024];
#ifdef _WIN32
#elif linux
                FILE* f = popen("zenity --file-selection --save --confirm-overwrite "
                                "--file-filter='Model files (fbx, obj) | *.fbx *.obj'", "r");
                fgets(filename, 1024, f);
#endif*/
            }

            Separator();

			if(MenuItem("Quit"))
			{
				engine_.Stop();
			}
			ImGui::EndMenu();
		}

		//Edit Tab
		if (BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}

		//View Tab
		if (BeginMenu("View"))
		{
			ImGui::EndMenu();
		}

		//Help Tab
		if (BeginMenu("Help"))
		{
			if(MenuItem("About"))
			{
				popupNamesStack_.emplace_back("About");
			}
			ImGui::EndMenu();
		}
		EndMainMenuBar();
	}

	BeginMenuBar();
	{
		if (RadioButton("Translate", manipulateOperation_ == ImGuizmo::TRANSLATE))
			manipulateOperation_ = ImGuizmo::TRANSLATE;
		if (RadioButton("Rotate", manipulateOperation_ == ImGuizmo::ROTATE))
			manipulateOperation_ = ImGuizmo::ROTATE;
		if (RadioButton("Scale", manipulateOperation_ == ImGuizmo::SCALE))
			manipulateOperation_ = ImGuizmo::SCALE;
		EndMenuBar();
	}
}

void ShowRoomRenderer::DrawSceneHierarchy()
{
	using namespace ImGui;

	auto& meshes= model_.GetMeshes();
	if (IsMouseClicked(0))
	{
		const auto& winSize = BasicEngine::GetInstance()->config.windowSize;
		const Vec3f dir     = -camera_.reverseDirection.Normalized();

		degree_t fovX        = camera_.GetFovX();
		degree_t fovY        = camera_.fovY;
		float halfWidthNear  = Sin(fovX / 2.0f) * camera_.nearPlane;
		float halfHeightNear = Sin(fovY / 2.0f) * camera_.nearPlane;
		float halfWidthFar   = Sin(fovX / 2.0f) * camera_.farPlane;
		float halfHeightFar  = Sin(fovY / 2.0f) * camera_.farPlane;
		Plane nearPlane      = Plane(camera_.position + dir * camera_.nearPlane, dir);
		Plane farPlane       = Plane(camera_.position + dir * camera_.farPlane, -dir);

		const Vec2f mousePos = GetMousePos();
		const Vec2f relativeMousePos =
			Vec2f((mousePos.x - winSize.x * 0.5f) / (winSize.x * 0.5f),
			      -(mousePos.y - winSize.y * 0.5f) / (winSize.y * 0.5f));
		const Vec3f up    = camera_.GetUp();
		const Vec3f right = camera_.GetRight();

		Vec3f farPos      = farPlane.point;
		farPos += right * relativeMousePos.x * halfWidthFar;
		farPos += up * relativeMousePos.y * halfHeightFar;

		Vec3f nearPos = nearPlane.point;
		nearPos += right * relativeMousePos.x * halfWidthNear;
		nearPos += up * relativeMousePos.y * halfHeightNear;

		Vec3f ray = farPos - camera_.position;

		std::vector<size_t> candidates;
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			if (meshes[i].GetAabb().DoIntersectRay(ray, camera_.position))
				candidates.push_back(i);
		}

		if (!candidates.empty())
		{
			float smallestDist      = maxFloat;
			size_t currentCandidate = INVALID_INDEX;
			for (const auto& candidate : candidates)
			{
				const Vec3f center = model_.GetMesh(candidate).GetAabb().CalculateCenter();
				const float dist = (center - nearPos).SquareMagnitude();
				if (dist < smallestDist)
				{
					smallestDist = dist;
					currentCandidate = candidate;
				}
			}

			selectedNode_ = currentCandidate + selectionOffset_;
		}
	}

	if (Begin("Scene"))
	{
		PushItemWidth(-1);
		const float height = GetTextLineHeightWithSpacing();
		ImGui::Image(reinterpret_cast<ImTextureID>(searchIcon_), ImVec2(height, height)); SameLine();
		searchFilter_.Draw();

		PopItemWidth(); Separator();

		SetCursorPosX(ImGui::GetCursorPosX() - 8.0f);
		SetCursorPosY(ImGui::GetCursorPosY());
		BeginChild("Scene##Child");
		{
			//Light Node
			{
				std::string lightName;
				switch (lightType_)
				{
					case LightType::NO_LIGHT:
						lightName = "No Light";
						break;
					case LightType::POINT:
						lightName = "Point Light";
						break;
					case LightType::SUN:
						lightName = "Sun";
						break;
					case LightType::SPOT:
						lightName = "Spot Light";
						break;
				}

				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
				                               ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

				if (selectedNode_ == LIGHT)
					nodeFlags |= ImGuiTreeNodeFlags_Selected;

				TreeNodeEx(lightName.c_str(), nodeFlags);
				if (ImGui::IsItemClicked())
					selectedNode_ = LIGHT;
			}

			ImGuiTreeNodeFlags rootNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			std::string rootName;
			if (!model_.IsLoaded())
			{
				rootNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				rootName = "No Model Loaded";
			}
			else
			{
				rootName = model_.GetName();
			}

			if (selectedNode_ == MODEL)
				rootNodeFlags |= ImGuiTreeNodeFlags_Selected;

			SetNextItemOpen(true);
			const bool isOpen = TreeNodeEx(rootName.c_str(), rootNodeFlags);
			if (ImGui::IsItemClicked())
				selectedNode_ = MODEL;

			if (isOpen)
			{
				for (auto& mesh : meshes)
				{
					if (mesh.GetParentName() == 0 && !searchFilter_.IsActive())
					{
						if (GetFilenameExtension(model_.GetPath()) == ".obj")
							DrawMeshImGuiObj(mesh);
						else
							DrawMeshImGui(mesh);
					}
					else if (searchFilter_.IsActive())
					{
						DrawMeshImGuiObj(mesh);
					}
				}

				if (model_.IsLoaded()) TreePop();
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
		switch (selectedNode_)
		{
			case NONE:
				Text("No item selected");
				Separator();
				break;
			case LIGHT:
				DrawLightProperties();
				break;
			case MODEL:
				DrawModelProperties();
				break;
			default:
				DrawMeshProperties();
				break;
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

	const std::size_t nodeId = model_.GetMeshId(mesh) + selectionOffset_;
	const std::string popupName = std::string("RenameNode##").append(std::to_string(mesh.GetVAO()));
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (children.empty())
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (selectedNode_ == nodeId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	const bool nodeOpen = TreeNodeEx(mesh.GetName().data(), nodeFlags);
	if (ImGui::IsItemClicked())
		selectedNode_ = nodeId;

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
	if (!searchFilter_.PassFilter(mesh.GetName().data())) return;

	using namespace ImGui;
	const std::size_t nodeId = model_.GetMeshId(mesh) + selectionOffset_;
	const std::string popupName = std::string("RenameNode##").append(std::to_string(mesh.GetVAO()));
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (selectedNode_ == nodeId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	TreeNodeEx(mesh.GetName().data(), nodeFlags);
	if (ImGui::IsItemClicked())
		selectedNode_ = nodeId;

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

	if (BeginPopupModal("NoFeature", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		TextCentered("This feature is not yet implemented");

		Separator();
		if (ButtonCentered("OK", ImVec2(100, 0))) CloseCurrentPopup();

		EndPopup();
	}
}

void ShowRoomRenderer::DrawLightProperties()
{
	using namespace ImGui;
	Text("Light");
	Separator();

	Combo("Type", (int*) &lightType_, lightLabels_);

	if (lightType_ == NO_LIGHT) return;
	PushItemWidth(-1);
	switch (lightType_)
	{
		case NO_LIGHT:
			break;
		case POINT:
			DrawPointLight();
			break;
		case SUN:
			DrawDirectionalLight();
			break;
		case SPOT:
			DrawSpotLight();
			break;
	}

	Text("Intensity"); SameLine();
	if (DragFloat("##pointLight_.intensity", &pointLight_.intensity, 1.0f, 0.0f, 1'000.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		directionalLight_.intensity = pointLight_.intensity;
		spotLight_.intensity = pointLight_.intensity;
	}

	Text("Ambient Color"); SameLine();
	if (ColorEdit4("##pointLight_.ambient", &pointLight_.ambient[0], ImGuiColorEditFlags_NoInputs))
	{
		directionalLight_.ambient = pointLight_.ambient;
		spotLight_.ambient = pointLight_.ambient;
	}

	Text("Diffuse Color"); SameLine();
	if (ColorEdit4("##pointLight_.diffuse", &pointLight_.diffuse[0], ImGuiColorEditFlags_NoInputs))
	{
		directionalLight_.diffuse = pointLight_.diffuse;
		spotLight_.diffuse = pointLight_.diffuse;
	}

	Text("Specular"); SameLine();
	if (DragFloat("##spotLight_.specular", &pointLight_.specular, 0.01f, 0.0f, 1.0f))
	{
		directionalLight_.specular = pointLight_.specular;
		spotLight_.specular = pointLight_.specular;
	}
	PopItemWidth();
}

void ShowRoomRenderer::DrawModelProperties()
{
	using namespace ImGui;
	if (model_.IsLoaded()) Text("%s", model_.GetName().data());
	else Text("%s", "No model loaded");
	Separator();
	if (!model_.IsLoaded()) return;

	Vec3f pos, scale;
	EulerAngles angles;
	modelMat_.Decompose(pos, angles, scale);
	auto anglesF = Vec3f(angles.x.value(), angles.y.value(), angles.z.value());

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Position X "); SameLine();
	float textPos1 = GetCursorPosX();
	DragFloat("##pos.x", &pos.x, 0.1f, minFloat, maxFloat);

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Y "); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##pos.y", &pos.y, 0.1f, minFloat, maxFloat);
	PopStyleVar();

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Z"); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##pos.z", &pos.z, 0.1f, minFloat, maxFloat);

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	SetCursorPosX(textPos1 - CalcTextSize("Rotation X ").x); Text("Rotation X "); SameLine();
	SetCursorPosX(textPos1);
	if (DragFloat("##anglesF.x", &anglesF.x, 0.1f, -360.0f, 360.0f))
		angles.x = degree_t(anglesF.x);

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Y "); SameLine();
	SetCursorPosX(textPos1);
	if (DragFloat("##anglesF.y", &anglesF.y, 0.1f, -360.0f, 360.0f))
		angles.y = degree_t(anglesF.y);
	PopStyleVar();

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Z"); SameLine();
	SetCursorPosX(textPos1);
	if (DragFloat("##anglesF.z", &anglesF.z, 0.1f, -360.0f, 360.0f))
		angles.z = degree_t(anglesF.z);

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	SetCursorPosX(textPos1 - CalcTextSize("Scale X ").x); Text("Scale X "); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##scale.x", &scale.x, 0.1f, smallFloat, maxFloat);

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Y "); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##scale.y", &scale.y, 0.1f, smallFloat, maxFloat);
	PopStyleVar();

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Z"); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##scale.z", &scale.z, 0.1f, smallFloat, maxFloat);

	modelMat_ = Transform3d::Transform(pos, angles, scale);
}

void ShowRoomRenderer::DrawMeshProperties()
{
	using namespace ImGui;
	auto& mesh = model_.GetMesh(selectedNode_ - selectionOffset_);
	Text("%s", mesh.GetName().data());
	Separator();

	auto& textures        = mesh.GetTextures();
	const auto diffuseId  = mesh.GetTexture(sr::Texture::TextureType::DIFFUSE);
	const auto specularId = mesh.GetTexture(sr::Texture::TextureType::SPECULAR);
	const auto normalId   = mesh.GetTexture(sr::Texture::TextureType::NORMAL);
	const auto emissiveId = mesh.GetTexture(sr::Texture::TextureType::EMISSIVE);

	Text("Diffuse");
	DrawTextureInput(diffuseId, sr::Texture::TextureType::DIFFUSE, textures);

	Text("Specular");
	DrawTextureInput(specularId, sr::Texture::TextureType::SPECULAR, textures);

	Text("Normal");
	DrawTextureInput(normalId, sr::Texture::TextureType::NORMAL, textures);

	Text("Emission");
	DrawTextureInput(emissiveId, sr::Texture::TextureType::EMISSIVE, textures);

	if (TreeNodeEx("Material Options", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		float shininess = mesh.GetShininess();
		Text("Shininess "); SameLine();
		if (DragFloat("##shininess", &shininess, 0.01f, 0.0f))
			mesh.SetShininess(shininess);

		TreePop();
	}
}

void ShowRoomRenderer::DrawTextureInput(
	size_t index,
	sr::Texture::TextureType type,
	std::vector<sr::Texture>& textures)
{
	using namespace ImGui;
	const auto windowWidth     = GetWindowWidth();
	const Vec2f spacing        = GetStyle().ItemSpacing;
	const float itemHeight     = GetFrameHeight();
	const float textLineHeight = GetTextLineHeight();
	const float buttonOffset   = itemHeight * 2 + textLineHeight * 2 + itemHeight / 2.0f;
	if (index != INVALID_INDEX)
	{
		PushStyleVar(ImGuiStyleVar_ItemSpacing, Vec2f::zero);
		ImGui::Image(reinterpret_cast<ImTextureID>(textures[index].textureName),
		             Vec2f(itemHeight)); SameLine();
		PushItemWidth(GetWindowSize().x - buttonOffset);

		const std::string nameId = "##texName" + std::to_string(static_cast<int>(type));
		InputText(nameId.c_str(), &textures[index].name, ImGuiTextFlags_None); SameLine();

		const std::string folderId = "##folderId" + std::to_string(static_cast<int>(type));
		if (ImageButton(folderId, folderIcon_, Vec2f(textLineHeight)))
			OpenTexture(index, textures);
		SameLine();
		PopStyleVar();
		PopItemWidth();

		PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing * 4.0f);
		const std::string deleteId = "##deleteId" + std::to_string(static_cast<int>(type));
		if (ImageButton(deleteId, deleteIcon_, Vec2f(textLineHeight)))
			textures[index] = {};
		PopStyleVar();
	}
	else
	{
		PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, Vec2f(0, spacing.y));
		PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 255));

		const std::string nameId = "New##texNew" + std::to_string(static_cast<int>(type));
		if (Button(nameId.c_str(), Vec2f(windowWidth / 2.0f - spacing.x, 0)))
			popupNamesStack_.emplace_back("NoFeature");
		SameLine();
		PopStyleVar(1);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing * 4.0f);

		const std::string openId = "Open##texNew" + std::to_string(static_cast<int>(type));
		if (Button(openId.c_str(), Vec2f(windowWidth / 2.0f - spacing.x, 0)))
			OpenTexture(type, textures);
		PopStyleVar(2);
		PopStyleColor();
	}
}

void ShowRoomRenderer::OpenTexture(size_t index, std::vector<sr::Texture>& textures)
{
	char filename[1024];

#ifdef _WIN32
	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = nullptr;
	ofn.lpstrFilter = "Image (*.png;*.jpg)\0*.png;*.jpg\0";
	ofn.lpstrFile   = filename;
	ofn.nMaxFile    = IM_ARRAYSIZE(filename);
	ofn.Flags       = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
        const std::string path = filename;
#elif linux
	FILE* f = popen("zenity --file-selection --file-filter='Image "
	                "(png, jpg) | *.png *.jpg' --title='Open...'", "r");
	fgets(filename, 1024, f);
	std::string path = filename;
	if (!path.empty())
	{
		path = path.substr(0, path.size() - 1);
#endif
		textures[index].textureId = textureManager_.LoadTexture(path, Texture::TextureFlags::DEFAULT);
		textures[index].name = path.substr(path.find_last_of('/') + 1, path.size());
	}
}

void ShowRoomRenderer::OpenTexture(
	sr::Texture::TextureType type,
	std::vector<sr::Texture>& textures)
{
	char filename[1024];

#ifdef _WIN32
	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = nullptr;
	ofn.lpstrFilter = "Image (*.png;*.jpg)\0*.png;*.jpg\0";
	ofn.lpstrFile   = filename;
	ofn.nMaxFile    = IM_ARRAYSIZE(filename);
	ofn.Flags       = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
        const std::string path = filename;
#elif linux
	FILE* f = popen("zenity --file-selection --file-filter='Image "
	                "(png, jpg) | *.png *.jpg' --title='Open...'", "r");
	fgets(filename, 1024, f);
	std::string path = filename;
	if (!path.empty())
	{
		path = path.substr(0, path.size() - 1);
#endif
		textures.emplace_back();
		textures.back().type = type;
		textures.back().textureId =
			textureManager_.LoadTexture(path, Texture::TextureFlags::DEFAULT);
#ifdef _WIN32
		textures.back().name = path.substr(path.find_last_of("\\") + 1, path.size());
#elif linux
		textures.back().name = path.substr(path.find_last_of('/') + 1, path.size());
#endif
	}
}

void ShowRoomRenderer::DrawPointLight()
{
	using namespace ImGui;
	if (TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		Text("Position X "); SameLine();
		float textPos = GetCursorPosX();
		if (DragFloat("##pointLight_.position.x", &pointLight_.position.x))
			spotLight_.position.x = pointLight_.position.x;

		SetCursorPosX(textPos - CalcTextSize("X ").x); Text("Y "); SameLine();
		SetCursorPosX(textPos);
		if (DragFloat("##pointLight_.position.y", &pointLight_.position.y))
			spotLight_.position.y = pointLight_.position.y;
		PopStyleVar();

		SetCursorPosX(textPos - CalcTextSize("X ").x); Text("Z"); SameLine();
		SetCursorPosX(textPos);
		if (DragFloat("##pointLight_.position.z", &pointLight_.position.z))
			spotLight_.position.z = pointLight_.position.z;

		TreePop();
	}

	Text("Radius "); SameLine();
	if (DragFloat("##spotLight_.radius", &spotLight_.radius, 0.01f))
		pointLight_.radius = spotLight_.radius;
}

void ShowRoomRenderer::DrawDirectionalLight()
{
	using namespace ImGui;
	if (TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		float angleX = lightOrientation_.x.value();
		float angleY = lightOrientation_.y.value();
		float angleZ = lightOrientation_.z.value();

		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		Text("Rotation X "); SameLine();
		float textPos = GetCursorPosX();
		if (DragFloat("##angleXf", &angleX))
		{
			lightOrientation_.x = degree_t(angleX);
			directionalLight_.direction = Quaternion::FromEuler(-lightOrientation_) * Vec3f::up;
			spotLight_.direction = directionalLight_.direction;
		}

		SetCursorPosX(textPos - CalcTextSize("X ").x); Text("Y "); SameLine();
		SetCursorPosX(textPos);
		if (DragFloat("##angleYf", &angleY))
		{
			lightOrientation_.y = degree_t(angleY);
			directionalLight_.direction = Quaternion::FromEuler(-lightOrientation_) * Vec3f::up;
			spotLight_.direction = directionalLight_.direction;
		}
		PopStyleVar();

		SetCursorPosX(textPos - CalcTextSize("X ").x); Text("Z"); SameLine();
		SetCursorPosX(textPos);
		if (DragFloat("##angleZf", &angleZ))
		{
			lightOrientation_.z = degree_t(angleZ);
			directionalLight_.direction = Quaternion::FromEuler(-lightOrientation_) * Vec3f::up;
			spotLight_.direction = directionalLight_.direction;
		}

		TreePop();
	}
}

void ShowRoomRenderer::DrawSpotLight()
{
	using namespace ImGui;
	if (TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		float angleX = lightOrientation_.x.value();
		float angleY = lightOrientation_.y.value();
		float angleZ = lightOrientation_.z.value();

		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		Text("Position X "); SameLine();
		float textPos1 = GetCursorPosX();
		if (DragFloat("##pointLight_.position.x", &pointLight_.position.x))
			spotLight_.position.x = pointLight_.position.x;

		SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Y "); SameLine();
		SetCursorPosX(textPos1);
		if (DragFloat("##pointLight_.position.y", &pointLight_.position.y))
			spotLight_.position.y = pointLight_.position.y;
		PopStyleVar();

		SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Z"); SameLine();
		SetCursorPosX(textPos1);
		if (DragFloat("##pointLight_.position.z", &pointLight_.position.z))
			spotLight_.position.z = pointLight_.position.z;

		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		Text("Rotation X "); SameLine();
		float textPos2 = GetCursorPosX();
		if (DragFloat("##angleXf", &angleX))
		{
			lightOrientation_.x = degree_t(angleX);
			directionalLight_.direction = Quaternion::FromEuler(-lightOrientation_) * Vec3f::up;
			spotLight_.direction = directionalLight_.direction;
		}

		SetCursorPosX(textPos2 - CalcTextSize("X ").x); Text("Y "); SameLine();
		SetCursorPosX(textPos2);
		if (DragFloat("##angleYf", &angleY))
		{
			lightOrientation_.y = degree_t(angleY);
			directionalLight_.direction = Quaternion::FromEuler(-lightOrientation_) * Vec3f::up;
			spotLight_.direction = directionalLight_.direction;
		}
		PopStyleVar();

		SetCursorPosX(textPos2 - CalcTextSize("X ").x); Text("Z"); SameLine();
		SetCursorPosX(textPos2);
		if (DragFloat("##angleZf", &angleZ))
		{
			lightOrientation_.z = degree_t(angleZ);
			directionalLight_.direction = Quaternion::FromEuler(-lightOrientation_) * Vec3f::up;
			spotLight_.direction = directionalLight_.direction;
		}

		TreePop();
	}

	float angle = spotLight_.angle.value();
	Text("Angle "); SameLine();
	if (DragFloat("##spotLight_.angle", &angle, 0.1f, 0.0f, 180.0f, "%.0f°"))
		spotLight_.angle = degree_t(angle);

	Text("Blend "); SameLine();
	DragFloat("##spotLight_.blend", &spotLight_.blend, 0.01f, 0.0f, 1.0f);

	Text("Radius "); SameLine();
	if (DragFloat("##spotLight_.radius", &spotLight_.radius, 0.01f))
		pointLight_.radius = spotLight_.radius;
}
}    // namespace neko

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

bool ImGui::Combo(const char* label, int* currentItem, const std::vector<std::string>& items, int heightInItems)
{
	const auto lambda = [](void* data, int idx, const char** outText)
	{
		*outText = static_cast<const std::vector<std::string>*>(data)->at(idx).c_str();
		return true;
	};

	return Combo(label, currentItem, lambda, (void*)&items, items.size(), heightInItems);
}

bool ImGui::ImageButton(
	std::string_view itemId,
	neko::TextureName userTextureId,
	const ImVec2& size,
	const ImVec2& uv0,
	const ImVec2& uv1,
	int framePadding,
	const ImVec4& bgCol,
	const ImVec4& tintCol)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if (window->SkipItems)
		return false;

	auto texId = reinterpret_cast<ImTextureID>(userTextureId);

	const ImGuiID id = window->GetID(itemId.data());
	const ImVec2 padding = (framePadding >= 0) ?
                               ImVec2((float)framePadding, (float)framePadding) :
                               g.Style.FramePadding;
	return ImageButtonEx(id, texId, size, uv0, uv1, padding, bgCol, tintCol);
}
