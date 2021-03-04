#ifdef _WIN32
#include <commdlg.h>
#include <minwinbase.h>
#endif

#include "mathematics/plane.h"
#include "utils/file_utility.h"

#include "showroom/showroom_renderer.h"

namespace neko
{
ShowRoomRenderer::ShowRoomRenderer(BasicEngine& engine)
   : engine_(engine),
	 gizmoRenderer_(&camera_),
	 bloomFbo_(BasicEngine::GetInstance()->GetConfig().windowSize),
	 blurFbo_(BasicEngine::GetInstance()->GetConfig().windowSize)
{
	engine.RegisterSystem(textureManager_);
	engine.RegisterSystem(gizmoRenderer_);
}

void ShowRoomRenderer::Init()
{
	ImGuiIO& io                  = ImGui::GetIO();
	const float fontSizeInPixels = 16.0f;
	io.Fonts->AddFontFromFileTTF("data/droid_sans.ttf", fontSizeInPixels);

	camera_.Init();
	camera_.position  = Vec3f(13.0f, 12.0f, 6.0f);
	camera_.fovY      = 2 * Atan(0.5f * sensorSize_ / focalLength_);
	camera_.nearPlane = 0.001f;
	camera_.farPlane  = 1'000.0f;
	camera_.WorldLookAt(Vec3f::zero);

	pointLight_.position = Vec3f(5.0f, 3.0f, -3.0f);
	spotLight_.position  = pointLight_.position;
	dirLight_.direction  = Quaternion::FromEuler(lightAngles_) * Vec3f::down;
	spotLight_.direction = dirLight_.direction;

	preRender_ = Job(
		[this]
		{
			screenQuad_.Init();

			const auto& filesystem = BasicEngine::GetInstance()->GetFilesystem();
			searchIcon_            = sr::StbCreateTexture("data/icons/search.png", filesystem);
			folderIcon_            = sr::StbCreateTexture("data/icons/folder.png", filesystem);
			deleteIcon_            = sr::StbCreateTexture("data/icons/delete.png", filesystem);

			shader_.LoadFromFile("data/shaders/light.vert", "data/shaders/light.frag");
			blurShader_.LoadFromFile("data/shaders/blur.vert", "data/shaders/blur.frag");
			screenShader_.LoadFromFile("data/shaders/screen.vert", "data/shaders/screen.frag");

			CreateFramebuffers();

			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		});

	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void ShowRoomRenderer::CreateFramebuffers()
{
	const Vec2u size = BasicEngine::GetInstance()->GetConfig().windowSize;
    bloomFbo_.size = size;
    blurFbo_.size = size;

	//Generate Bloom Framebuffer
	glGenFramebuffers(1, &bloomFbo_.fbo);
	glCheckError();
	bloomFbo_.Bind();

	glGenTextures(IM_ARRAYSIZE(bloomFbo_.colorBuffers), &bloomFbo_.colorBuffers[0]);
	for (int i = 0; i < IM_ARRAYSIZE(bloomFbo_.colorBuffers); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, bloomFbo_.colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, bloomFbo_.colorBuffers[i], 0);
	}

    unsigned attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    glCheckFramebuffer();
    glCheckError();

    glGenRenderbuffers(1, &bloomFbo_.depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, bloomFbo_.depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.x, size.y);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bloomFbo_.depthRbo);

    //Generate Blur Framebuffer
	glGenFramebuffers(1, &blurFbo_.fbo);
	blurFbo_.Bind();

	glGenTextures(1, &blurFbo_.colorBuffer);
	glBindTexture(GL_TEXTURE_2D, blurFbo_.colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurFbo_.colorBuffer, 0);
	glCheckError();

	Framebuffer::Unbind();
}

void ShowRoomRenderer::Update(const seconds dt)
{
	camera_.Update(dt);
	gizmoRenderer_.Clear();

	if (modelObj_.IsLoaded())
	{
		auto& meshes = modelObj_.GetMeshes();
		for (auto& mesh : meshes) { mesh.UpdateTextures(); }
    }

	DrawGrid();

	const Color4 color = selectedNode_ == LIGHT ? Color::orange : Color::grey;
	gizmoRenderer_.DrawSphere(pointLight_.position, 0.25f, color);
	switch (lightType_)
	{
		case NO_LIGHT: break;
		case POINT:
			gizmoRenderer_.DrawSphere(pointLight_.position, pointLight_.radius, color);
			break;
		case SPOT: gizmoRenderer_.DrawSphere(pointLight_.position, pointLight_.radius, color);
		case SUN:
			gizmoRenderer_.DrawLine(
				pointLight_.position, pointLight_.position + dirLight_.direction * 5.0f, color);
			break;
	}

	RendererLocator::get().Render(this);
}

void ShowRoomRenderer::Render()
{
    if (isResized_)
    {
        bloomFbo_.Destroy();
        blurFbo_.Destroy();
        CreateFramebuffers();

        isResized_ = false;
    }

	if (searchIcon_ == INVALID_TEXTURE_NAME) return;
	if (modelObj_.IsLoaded())
	{
		if (isModelLoading_)
		{
			const auto now = std::chrono::high_resolution_clock::now();
			const auto duration = std::chrono::duration_cast<seconds>(now - modelLoadingStart_);
			modelLoadingTime_ = static_cast<double>(duration.count());
			isModelLoading_ = false;
		}

		bloomFbo_.Bind();
		{
			BloomFbo::Clear(Color::clear);
			UpdateShader(shader_);

            /*const auto& meshes = modelObj_.GetMeshes();
            for (size_t i = 0; i < meshes.size(); ++i)
            {
                shader_.SetBool("isSelected", selectedNode_ - selectionOffset_ == i);
                meshes[i].Draw(shader_, modelMat_);
            }*/

            const auto& meshesObj = modelObj_.GetMeshes();
            for (std::size_t i = 0; i < meshesObj.size(); ++i)
            {
                shader_.SetBool("isSelected", selectedNode_ - selectionOffset_ == i);
	            meshesObj[i].Draw(shader_, modelMat_);
            }
		}

		blurFbo_.Bind();
		{
			BlurFbo::Clear(Color::clear);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			blurShader_.Bind();
			blurShader_.SetTexture("image", bloomFbo_.colorBuffers[1], 0);
			blurShader_.SetTexture("image", bloomFbo_.colorBuffers[1], 0);
			blurShader_.SetVec2("screenSize", Vec2f(BasicEngine::GetInstance()->GetConfig().windowSize));
			screenQuad_.Draw();
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}

		Framebuffer::Unbind();
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (modelObj_.IsLoaded())
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		screenShader_.Bind();
		screenShader_.SetTexture("screenTexture", bloomFbo_.colorBuffers[0], 0);
		screenShader_.SetTexture("bloomBlur", blurFbo_.colorBuffer, 1);
		screenShader_.SetTexture("outline", bloomFbo_.colorBuffers[2], 2);
		screenQuad_.Draw();
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		bloomFbo_.RetrieveDepth();
	}
	gizmoRenderer_.Render();
}

void ShowRoomRenderer::UpdateShader(const gl::Shader& shader) const
{
	shader.Bind();

	const Mat4f camView = camera_.GenerateViewMatrix();
	const Mat4f camProj = camera_.GenerateProjectionMatrix();

	shader.SetMat4("view", camView);
	shader.SetMat4("proj", camProj);
	shader.SetMat4("model", modelMat_);
	shader.SetMat3("normalMatrix", Mat3f(modelMat_).Inverse().Transpose());

	shader.SetVec3("outlineCol", Color3(Color::orange) * 0.5f);
	shader.SetVec3("viewPos", camera_.position);
	shader.SetUInt("lightType", lightType_);

	shader.SetVec3("light.position", pointLight_.position);
	shader.SetVec3("light.direction", dirLight_.direction.Normalized());
	shader.SetVec3("light.ambient", pointLight_.ambient);
	shader.SetVec3("light.diffuse", pointLight_.diffuse);
	shader.SetFloat("light.specular", pointLight_.specular);
	shader.SetFloat("light.intensity", pointLight_.intensity);

	shader.SetFloat("sLight.blend", Cos(spotLight_.angle * spotLight_.blend));
	shader.SetFloat("sLight.angle", Cos(spotLight_.angle));
	shader.SetFloat("sLight.radius", spotLight_.radius);
}

void ShowRoomRenderer::DrawGrid()
{
	//Draw the coordinate axes
	const Vec3i posI      = Vec3i(camera_.position);
	const Vec3f startPosX = Vec3f::right * (posI.x - 100.0f);
	const Vec3f endPosX   = Vec3f::right * (posI.x + 100.0f);
	const Vec3f startPosZ = Vec3f::forward * (posI.z - 100.0f);
	const Vec3f endPosZ   = Vec3f::forward * (posI.z + 100.0f);

	if (startPosZ.z <= 0.0f)
		gizmoRenderer_.DrawLine(startPosX, endPosX, Color4(1.0f, 0.12f, 0.12f, 1.0f), 2.0f);
	if (startPosX.x <= 0.0f)
		gizmoRenderer_.DrawLine(startPosZ, endPosZ, Color4(0.08f, 0.21f, 0.76f, 1.0f), 2.0f);

	Color4 color = Color4(Vec3f(0.2f), 1.0f);

	//Draw lines along the X axis
	for (int x = -100; x < 100; ++x)
	{
		const Vec3f startPos = Vec3f(posI.x, 0.0f, posI.z) + Vec3f::back * 100.0f +
		                       Vec3f::right * static_cast<float>(x);
		if (startPos.x == 0.0f) continue;

		const Vec3f endPos = Vec3f(posI.x, 0.0f, posI.z) + Vec3f::forward * 100.0f +
		                     Vec3f::right * static_cast<float>(x);
		gizmoRenderer_.DrawLine(startPos, endPos, int(endPos.x) % 10 ? color : color * 2.0f);
	}

	//Draw lines along the Z axis
	for (int z = -100; z < 100; ++z)
	{
		const Vec3f startPos = Vec3f(posI.x, 0.0f, posI.z) + Vec3f::left * 100.0f +
		                       Vec3f::forward * static_cast<float>(z);
		if (startPos.z == 0.0f) continue;

		const Vec3f endPos = Vec3f(posI.x, 0.0f, posI.z) + Vec3f::right * 100.0f +
		                     Vec3f::forward * static_cast<float>(z);
		gizmoRenderer_.DrawLine(startPos, endPos, int(endPos.z) % 10 ? color : color * 2.0f);
	}
}

void ShowRoomRenderer::Destroy()
{
	modelObj_.Destroy();
	shader_.Destroy();

	textureManager_.Destroy();
	gizmoRenderer_.Destroy();

	screenShader_.Destroy();
	blurShader_.Destroy();
	bloomFbo_.Destroy();
	blurFbo_.Destroy();
	screenQuad_.Destroy();

	sr::DestroyTexture(searchIcon_);
	sr::DestroyTexture(folderIcon_);
	sr::DestroyTexture(deleteIcon_);
}

void ShowRoomRenderer::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);

    if (event.type == SDL_WINDOWEVENT)
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            isResized_ = true;
}

void ShowRoomRenderer::DrawImGui()
{
	using namespace ImGui;
    mouseEvents_.MouseFrame();

	CreateDockableWindow();
	DrawInfoBar();
	DrawSceneHierarchy();
	DrawPropertiesWindow();
	DrawToolWindow();
	DrawPopups();
	DrawImGuizmo();

    auto& meshes= modelObj_.GetMeshes();
    if (IsMouseDown(0) && !IsAnyWindowHovered() && !IsAnyItemHovered() && !IsAnyItemActive() && !ImGuizmo::IsUsing())
    {
        const Vec2f winSize = Vec2f(BasicEngine::GetInstance()->GetConfig().windowSize);
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
			const Mat4f& model = meshes[i].GetModelMatrix();
			Aabb3d aabb = meshes[i].GetAabb();
			aabb.lowerLeftBound += Transform3d::GetPosition(model);
			aabb.upperRightBound += Transform3d::GetPosition(model);
            if (aabb.DoIntersectRay(ray, camera_.position))
                candidates.push_back(i);
        }

        if (!candidates.empty())
        {
            float smallestDist      = kMaxFloat;
            size_t currentCandidate = INVALID_INDEX;
            for (const auto& candidate : candidates)
            {
			    const Mat4f& model = modelObj_.GetMesh(candidate).GetModelMatrix();
			    Aabb3d aabb = modelObj_.GetMesh(candidate).GetAabb();
				aabb.lowerLeftBound += Transform3d::GetPosition(model);
				aabb.upperRightBound += Transform3d::GetPosition(model);

                const Vec3f center = aabb.CalculateCenter();
                const float dist = (center - nearPos).SquareMagnitude();
                if (dist < smallestDist)
                {
                    smallestDist = dist;
                    currentCandidate = candidate;
                }
            }

            selectedNode_ = currentCandidate + selectionOffset_;
        }
        else
        {
            selectedNode_ = NONE;
        }
    }

	//ShowDemoWindow();
}

void ShowRoomRenderer::CreateDockableWindow()
{
	using namespace ImGui;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = GetMainViewport();
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

		ImGuizmo::SetDrawlist();

        dockspaceId_ = GetID("ShowroomDockSpace");
        if (!DockBuilderGetNode(dockspaceId_))
        {
            DockBuilderRemoveNode(dockspaceId_);
            DockBuilderAddNode(dockspaceId_, ImGuiDockNodeFlags_None);

            ImGuiID dockMain = dockspaceId_;
            ImGuiID dockUp = DockBuilderSplitNode(dockMain, ImGuiDir_Up, 0.98f, nullptr, &dockMain);
            const ImGuiID dockDown =
                DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.02f, nullptr, &dockMain);
            DockBuilderGetNode(dockspaceId_)->ChildNodes[1]->LocalFlags |=
                ImGuiDockNodeFlags_NoResize;

            ImGuiID dockRight =
                DockBuilderSplitNode(dockUp, ImGuiDir_Right, 0.175f, nullptr, &dockUp);
            ImGuiID dockLeft = DockBuilderSplitNode(dockUp, ImGuiDir_Left, 0.2f, nullptr, &dockUp);
            const ImGuiID dockDownRightId =
                DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.5f, nullptr, &dockRight);

            ImGuiID dockLeftLeft =
                DockBuilderSplitNode(dockLeft, ImGuiDir_Left, 0.2f, nullptr, &dockLeft);
            const ImGuiID dockLeftLeftDown =
                DockBuilderSplitNode(dockLeftLeft, ImGuiDir_Down, 0.5f, nullptr, &dockLeftLeft);
            const ImGuiID dockLeftRight =
                DockBuilderSplitNode(dockLeft, ImGuiDir_Right, 0.8f, nullptr, &dockLeft);
            DockBuilderGetNode(dockLeftRight)->LocalFlags = ImGuiDockNodeFlags_CentralNode;

			DockBuilderDockWindow("Scene", dockRight);
			DockBuilderDockWindow("InfoBar", dockDown);
			DockBuilderDockWindow("View", dockLeftLeft);
			DockBuilderDockWindow("Transform", dockLeftLeftDown);
			DockBuilderDockWindow("Properties", dockDownRightId);
        }

	    DockSpace(dockspaceId_, ImVec2(0.0f, 0.0f), dockspaceFlags_);

		DrawMenuBar();

		End();
	}
}

void ShowRoomRenderer::DrawImGuizmo()
{
	const Mat4f camView = camera_.GenerateViewMatrix();
	const Mat4f camProj = camera_.GenerateProjectionMatrix();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	switch (selectedNode_)
	{
		case NONE: break;
		case LIGHT:
		{
			Mat4f lightModel = Transform3d::Transform(pointLight_.position, lightAngles_, lightScale_);
			if (Manipulate(
				&camView[0][0],
				&camProj[0][0],
				manipulateOperation_,
				ImGuizmo::MODE::WORLD,
				&lightModel[0][0]))
			{
				Vec3f pos, scale;
				EulerAngles rot;
				lightModel.Decompose(pos, rot, scale);

				pointLight_.position = pos;
				spotLight_.position  = pointLight_.position;

				lightScale_ = scale;

				lightAngles_         = rot;
				dirLight_.direction  = Quaternion::FromEuler(rot) * Vec3f::down;
				spotLight_.direction = dirLight_.direction;
			}
			break;
		}
		case MODEL:
		{
			if (!modelObj_.IsLoaded()) break;
			Manipulate(
				&camView[0][0],
				&camProj[0][0],
				manipulateOperation_,
				ImGuizmo::MODE::WORLD,
				&modelMat_[0][0]);
			break;
		}
		default:
		{
			if (!modelObj_.IsLoaded()) break;
			auto& mesh = modelObj_.GetMesh(selectedNode_ - selectionOffset_);
            const Vec3f pos = mesh.GetAabb().CalculateCenter();
			Mat4f modelMat = mesh.GetModelMatrix() * modelMat_ * Transform3d::TranslationMatrixFrom(pos);
			if (Manipulate(
				&camView[0][0],
				&camProj[0][0],
				manipulateOperation_,
				ImGuizmo::MODE::WORLD,
				&modelMat[0][0]))
				mesh.SetModelMatrix(modelMat * modelMat_.Inverse() * Transform3d::TranslationMatrixFrom(-pos));
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
				const std::string path = OpenFileExplorer("Load Model", "Model files", {"obj"});
				if (!path.empty())
				{
					selectedNode_ = NONE;
					if (modelObj_.IsLoaded())
					{
						modelObj_.Destroy();
						textureManager_.Destroy();
					}

					//model_.LoadModel(path);
					modelObj_.LoadModel(path);
					isModelLoading_ = true;
					modelLoadingStart_ = std::chrono::high_resolution_clock::now();
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
		if (BeginMenu("Camera"))
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

void ShowRoomRenderer::DrawInfoBar() const
{
	using namespace ImGui;

	PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
	if (Begin("InfoBar"))
	{
		auto* dockNode = GetWindowDockNode();
		if (dockNode)
			dockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;

		if (isModelLoading_)
			Text("Loading Model...");
		else if (modelObj_.GetMeshCount() != 0)
			Text("Model loaded in %.3fs", modelLoadingTime_);

		PopStyleVar(3);
		End();
	}
}

void ShowRoomRenderer::DrawSceneHierarchy()
{
	using namespace ImGui;

	auto& meshes= modelObj_.GetMeshes();
	if (Begin("Scene"))
	{
        if (GetWindowDockNode()) GetWindowDockNode()->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

		PushItemWidth(-1.0f);
		const float height = GetTextLineHeightWithSpacing();
		ImGui::Image(reinterpret_cast<ImTextureID>(searchIcon_), ImVec2(height, height)); SameLine();
		searchFilter_.Draw();

		PopItemWidth(); Separator();

		SetCursorPosX(GetCursorPosX() - 8.0f);
		SetCursorPosY(GetCursorPosY());
		BeginChild("Scene##Child");
		{
			//Light Node
			{
				std::string lightName;
				switch (lightType_)
				{
					case NO_LIGHT:
						lightName = "No Light";
						break;
					case POINT:
						lightName = "Point Light";
						break;
					case SUN:
						lightName = "Sun";
						break;
					case SPOT:
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
			if (!modelObj_.IsLoaded())
			{
				rootNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				rootName = "No Model Loaded";
			}
			else
			{
				rootName = modelObj_.GetName();
			}

			if (selectedNode_ == MODEL)
				rootNodeFlags |= ImGuiTreeNodeFlags_Selected;

			SetNextItemOpen(true);
			const bool isOpen = TreeNodeEx(rootName.c_str(), rootNodeFlags);
			if (IsItemClicked())
				selectedNode_ = MODEL;

			if (isOpen && modelObj_.IsLoaded())
			{
				for (auto& mesh : meshes) DrawMeshImGuiObj(mesh);

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
		if (GetWindowDockNode()) GetWindowDockNode()->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
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
    Begin("View");
    {
		Text("View");
		Separator();

		if (GetWindowDockNode()) GetWindowDockNode()->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		PushItemWidth(-1.0f);
		float width = CalcItemWidth();
		PushItemWidth(width - CalcTextSize(" Focal Length").x);
		if (DragFloat("Focal Length",
				&focalLength_,
				1.0f,
				1.0f,
				5000.0f,
				"%.4f",
				ImGuiSliderFlags_Logarithmic))
			camera_.fovY = 2 * Atan(0.5f * sensorSize_ / focalLength_);

		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        DragFloat("Clip Start", &camera_.nearPlane);
        PopStyleVar();
        DragFloat("Clip End", &camera_.farPlane);
        PopItemWidth();

        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        Text("Position:");
        width = CalcItemWidth();
        PushItemWidth(width - CalcTextSize(" X").x);
        DragFloat("X", &camera_.position.x, 0.1f);
        DragFloat("Y", &camera_.position.y, 0.1f);
        PopStyleVar();
        DragFloat("Z", &camera_.position.z, 0.1f);
        PopItemWidth();
        PopItemWidth();

        End();
    }

    Begin("Transform");
    {
        Text("Transform");
        Separator();

        if (GetWindowDockNode()) GetWindowDockNode()->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
        switch (selectedNode_)
        {
            case NONE: break;
            case LIGHT: DrawLightTransform(); break;
            case MODEL: DrawModelTransform(); break;
            default: DrawMeshTransform(modelObj_.GetMesh(selectedNode_ - selectionOffset_)); break;
        }

        End();
    }
}

void ShowRoomRenderer::DrawLightTransform()
{
	using namespace ImGui;
	PushItemWidth(-1.0f);
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Position:");
	float width = CalcItemWidth();
	PushItemWidth(width - CalcTextSize(" X").x);
	if (DragFloat("X##pointLight_.position", &pointLight_.position.x, 0.1f))
		spotLight_.position = pointLight_.position;
	if (DragFloat("Y##pointLight_.position", &pointLight_.position.y, 0.1f))
		spotLight_.position = pointLight_.position;
	PopStyleVar();
	if (DragFloat("Z##pointLight_.position", &pointLight_.position.z, 0.1f))
		spotLight_.position = pointLight_.position;
	PopItemWidth();

	auto anglesF = Vec3f(lightAngles_);
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Rotation:");
	width = CalcItemWidth();
	PushItemWidth(width - CalcTextSize(" X").x);
	if (DragFloat("X##anglesF", &anglesF.x, 0.1f))
	{
		lightAngles_.x = degree_t(anglesF.x);
		dirLight_.direction = Quaternion::FromEuler(lightAngles_) * Vec3f::down;
		spotLight_.direction = dirLight_.direction;
	}
	if (DragFloat("Y##anglesF", &anglesF.y, 0.1f))
	{
		lightAngles_.y = degree_t(anglesF.y);
		dirLight_.direction = Quaternion::FromEuler(lightAngles_) * Vec3f::down;
		spotLight_.direction = dirLight_.direction;
	}
	PopStyleVar();
	if (DragFloat("Z##anglesF", &anglesF.z, 0.1f))
	{
		lightAngles_.z = degree_t(anglesF.z);
		dirLight_.direction = Quaternion::FromEuler(lightAngles_) * Vec3f::down;
		spotLight_.direction = dirLight_.direction;
	}
	PopItemWidth();

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Scale:");
	width = CalcItemWidth();
	PushItemWidth(width - CalcTextSize(" X").x);
	DragFloat("X##lightScale_", &lightScale_.x, 0.1f, kSmallFloat);
	DragFloat("Y##lightScale_", &lightScale_.y, 0.1f, kSmallFloat);
	PopStyleVar();
	DragFloat("Z##lightScale_", &lightScale_.z, 0.1f, kSmallFloat);
	PopItemWidth();
	PopItemWidth();
}

void ShowRoomRenderer::DrawModelTransform()
{
	using namespace ImGui;
	Vec3f pos, scale;
	EulerAngles angles;
	modelMat_.Decompose(pos, angles, scale);

	PushItemWidth(-1.0f);
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Position:");
	float width = CalcItemWidth();
	PushItemWidth(width - CalcTextSize(" X").x);
	DragFloat("X##pos", &pos.x, 0.1f);
	DragFloat("Y##pos", &pos.y, 0.1f);
	PopStyleVar();
	DragFloat("Z##pos", &pos.z, 0.1f);
	PopItemWidth();

	auto anglesF = Vec3f(angles);
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Rotation:");
	width = CalcItemWidth();
	PushItemWidth(width - CalcTextSize(" X").x);
	if (DragFloat("X##anglesF", &anglesF.x, 0.1f))
		angles.x = degree_t(anglesF.x);
	if (DragFloat("Y##anglesF", &anglesF.y, 0.1f))
		angles.y = degree_t(anglesF.y);
	PopStyleVar();
	if (DragFloat("Z##anglesF", &anglesF.z, 0.1f))
		angles.z = degree_t(anglesF.z);
	PopItemWidth();

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Scale:");
	width = CalcItemWidth();
	PushItemWidth(width - CalcTextSize(" X").x);
	DragFloat("X##scale", &scale.x, 0.1f, kSmallFloat);
	DragFloat("Y##scale", &scale.y, 0.1f, kSmallFloat);
	PopStyleVar();
	DragFloat("Z##scale", &scale.z, 0.1f, kSmallFloat);
	PopItemWidth();
	PopItemWidth();

	modelMat_ = Transform3d::Transform(pos, angles, scale);
}

void ShowRoomRenderer::DrawMeshTransform(sr::MeshObj& mesh)
{
    using namespace ImGui;
    Vec3f pos, scale;
    EulerAngles angles;
    const Mat4f modelMat = mesh.GetModelMatrix();
    modelMat.Decompose(pos, angles, scale);

    PushItemWidth(-1.0f);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    Text("Position:");
    float width = CalcItemWidth();
    PushItemWidth(width - CalcTextSize(" X").x);
    DragFloat("X##pos", &pos.x, 0.1f);
    DragFloat("Y##pos", &pos.y, 0.1f);
    PopStyleVar();
    DragFloat("Z##pos", &pos.z, 0.1f);
    PopItemWidth();

    auto anglesF = Vec3f(angles);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    Text("Rotation:");
    width = CalcItemWidth();
    PushItemWidth(width - CalcTextSize(" X").x);
    if (DragFloat("X##anglesF", &anglesF.x, 0.1f))
        angles.x = degree_t(anglesF.x);
    if (DragFloat("Y##anglesF", &anglesF.y, 0.1f))
        angles.y = degree_t(anglesF.y);
    PopStyleVar();
    if (DragFloat("Z##anglesF", &anglesF.z, 0.1f))
        angles.z = degree_t(anglesF.z);
    PopItemWidth();

    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    Text("Scale:");
    width = CalcItemWidth();
    PushItemWidth(width - CalcTextSize(" X").x);
    DragFloat("X##scale", &scale.x, 0.1f, kSmallFloat);
    DragFloat("Y##scale", &scale.y, 0.1f, kSmallFloat);
    PopStyleVar();
    DragFloat("Z##scale", &scale.z, 0.1f, kSmallFloat);
    PopItemWidth();
    PopItemWidth();

    mesh.SetModelMatrix(Transform3d::Transform(pos, angles, scale));
}

void ShowRoomRenderer::DrawMeshImGuiObj(sr::MeshObj& mesh)
{
	if (!searchFilter_.PassFilter(mesh.GetName().data())) return;

	using namespace ImGui;
	const std::size_t nodeId = modelObj_.GetMeshId(mesh) + selectionOffset_;
	const std::string popupName = std::string("RenameNode##").append(std::to_string(mesh.GetVao()));
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (selectedNode_ == nodeId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	TreeNodeEx(mesh.GetName().data(), nodeFlags);
	if (IsItemClicked())
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

	if (BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		TextCentered("NekoShowroom");
		TextCentered("Version 0.1");
		NewLine();
		TextCentered("Copyright © 2020-2020");
		TextCentered("SAE Institute Geneva");

		Separator();
		if (ButtonCentered("OK", ImVec2(100.0f, 0.0f))) CloseCurrentPopup();

		EndPopup();
	}

	if (BeginPopupModal("NoFeature", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		TextCentered("This feature is not yet implemented");

		Separator();
		if (ButtonCentered("OK", ImVec2(100.0f, 0.0f))) CloseCurrentPopup();

		EndPopup();
	}
}

void ShowRoomRenderer::DrawLightProperties()
{
	using namespace ImGui;
	Text("Light");
	Separator();

	Combo("Type", reinterpret_cast<int*>(&lightType_), lightLabels_);

	if (lightType_ == NO_LIGHT) return;
	PushItemWidth(-1.0f);
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

	Text("Intensity");
	SameLine();
	if (DragFloat("##pointLight_.intensity",
			&pointLight_.intensity,
			1.0f,
			0.0f,
			1'000.0f,
			"%.3f",
			ImGuiSliderFlags_Logarithmic))
	{
		dirLight_.intensity  = pointLight_.intensity;
		spotLight_.intensity = pointLight_.intensity;
	}

	Text("Ambient Color");
	SameLine();
	if (ColorEdit4("##pointLight_.ambient", &pointLight_.ambient[0], ImGuiColorEditFlags_NoInputs))
	{
		dirLight_.ambient  = pointLight_.ambient;
		spotLight_.ambient = pointLight_.ambient;
	}

	Text("Diffuse Color"); SameLine();
	if (ColorEdit4("##pointLight_.diffuse", &pointLight_.diffuse[0], ImGuiColorEditFlags_NoInputs))
	{
		dirLight_.diffuse = pointLight_.diffuse;
		spotLight_.diffuse = pointLight_.diffuse;
	}

	Text("Specular"); SameLine();
	if (DragFloat("##spotLight_.specular", &pointLight_.specular, 0.01f, 0.0f, 1.0f))
	{
		dirLight_.specular = pointLight_.specular;
		spotLight_.specular = pointLight_.specular;
	}
	PopItemWidth();
}

void ShowRoomRenderer::DrawModelProperties()
{
	using namespace ImGui;
	if (modelObj_.IsLoaded()) Text("%s", modelObj_.GetName().data());
	else Text("%s", "No model loaded");
	Separator();
	if (!modelObj_.IsLoaded()) return;

	Vec3f pos, scale;
	EulerAngles angles;
	modelMat_.Decompose(pos, angles, scale);
	auto anglesF = Vec3f(angles.x.value(), angles.y.value(), angles.z.value());

	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	Text("Position X "); SameLine();
    const float textPos1 = GetCursorPosX();
	DragFloat("##pos.x", &pos.x, 0.1f, kMinFloat, kMaxFloat);

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Y "); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##pos.y", &pos.y, 0.1f, kMinFloat, kMaxFloat);
	PopStyleVar();

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Z"); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##pos.z", &pos.z, 0.1f, kMinFloat, kMaxFloat);

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
	DragFloat("##scale.x", &scale.x, 0.1f, kSmallFloat, kMaxFloat);

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Y "); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##scale.y", &scale.y, 0.1f, kSmallFloat, kMaxFloat);
	PopStyleVar();

	SetCursorPosX(textPos1 - CalcTextSize("X ").x); Text("Z"); SameLine();
	SetCursorPosX(textPos1);
	DragFloat("##scale.z", &scale.z, 0.1f, kSmallFloat, kMaxFloat);

	modelMat_ = Transform3d::Transform(pos, angles, scale);
}

void ShowRoomRenderer::DrawMeshProperties()
{
	using namespace ImGui;
	auto& mesh = modelObj_.GetMesh(selectedNode_ - selectionOffset_);
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

	mesh.UpdateTextures();

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
	const float buttonOffset   = itemHeight * 2.0f + textLineHeight * 2.0f + itemHeight / 2.0f;
	if (index != INVALID_INDEX)
	{
		PushStyleVar(ImGuiStyleVar_ItemSpacing, Vec2f::zero);
		ImGui::Image(reinterpret_cast<ImTextureID>(textures[index].name),
		             Vec2f(itemHeight)); SameLine();
		PushItemWidth(GetWindowSize().x - buttonOffset);

		const std::string nameId = "##texName" + std::to_string(static_cast<int>(type));
		InputText(nameId.c_str(), &textures[index].sName, ImGuiTextFlags_None); SameLine();

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
		PushStyleVar(ImGuiStyleVar_ItemSpacing, Vec2f(0.0f, spacing.y));
		PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 255));

		const std::string nameId = "New##texNew" + std::to_string(static_cast<int>(type));
		if (Button(nameId.c_str(), Vec2f(windowWidth / 2.0f - spacing.x, 0.0f)))
			popupNamesStack_.emplace_back("NoFeature");
		SameLine();
		PopStyleVar(1);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing * 4.0f);

		const std::string openId = "Open##texNew" + std::to_string(static_cast<int>(type));
		if (Button(openId.c_str(), Vec2f(windowWidth / 2.0f - spacing.x, 0.0f)))
			OpenTexture(type, textures);
		PopStyleVar(2);
		PopStyleColor();
	}
}

void ShowRoomRenderer::OpenTexture(const size_t index, std::vector<sr::Texture>& textures)
{
    const std::string path = OpenFileExplorer("Load Texture", "Texture", {"png", "jpg"});
	if (!path.empty())
	{
		textures[index].textureId = textureManager_.LoadTexture(path, Texture::TextureFlags::DEFAULT);
		textures[index].name = INVALID_TEXTURE_NAME;
		textures[index].sName = path.substr(path.find_last_of('/') + 1, path.size());
	}
}

void ShowRoomRenderer::OpenTexture(
	sr::Texture::TextureType type,
	std::vector<sr::Texture>& textures)
{
    const std::string path = OpenFileExplorer("Load Texture", "Texture", {"png", "jpg"});
	if (!path.empty())
	{
        textures.emplace_back();
        textures.back().type = type;
        textures.back().textureId =
            textureManager_.LoadTexture(path, Texture::TextureFlags::DEFAULT);
#ifdef WIN32
		textures.back().sName = path.substr(path.find_last_of('\\') + 1, path.size());
#elif linux
		textures.back().sName = path.substr(path.find_last_of('/') + 1, path.size());
#endif
	}
}

void ShowRoomRenderer::DrawPointLight()
{
	using namespace ImGui;
	Text("Radius "); SameLine();
	if (DragFloat("##spotLight_.radius", &spotLight_.radius, 0.01f))
		pointLight_.radius = spotLight_.radius;
}

void ShowRoomRenderer::DrawDirectionalLight()
{
	using namespace ImGui;
}

void ShowRoomRenderer::DrawSpotLight()
{
	using namespace ImGui;
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

std::string ShowRoomRenderer::OpenFileExplorer(const std::string& title,
    const std::string& fileTypeName,
    const std::vector<std::string>& typeFilter,
    bool saveFile)
{
    char filename[1024];
#ifdef _WIN32
    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = nullptr;
    ofn.lpstrTitle   = title.c_str();

	std::string filter;
    for (std::size_t i = 0; i < typeFilter.size(); ++i)
    {
        filter += "*." + typeFilter[i];
        if (i != typeFilter.size() - 1) filter += ";";
    }

	filter = fileTypeName + " (" + filter + ')' + '\0' + filter + '\0';
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFile   = filename;
    ofn.nMaxFile    = IM_ARRAYSIZE(filename);
    ofn.Flags       = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) return std::string(filename);
#elif linux
	std::string data;
	const int maxBuffer = 256;
	char buffer[maxBuffer];
	FILE* stream = popen("echo $XDG_CURRENT_DESKTOP", "r");
	if (stream)
	{
		while (!feof(stream))
			if (fgets(buffer, maxBuffer, stream) != nullptr) data.append(buffer);
		pclose(stream);
	}

	FILE* f;
    std::string command;
    if (data == "KDE\n")
    {
        command = "kdialog ";
        if (!title.empty()) command += "--title='" + title + "' ";
        if (saveFile) command += "--getsavefilename ";
        else command += "--getopenfilename ";
        command += "'" + lastPath_ + "' ";
        if (!fileTypeName.empty()) command += "'" + fileTypeName;
        if (!typeFilter.empty())
        {
            command += " (";
            for (std::size_t i = 0; i < typeFilter.size(); ++i)
            {
                command += "*." + typeFilter[i];
                if (i != typeFilter.size() - 1) command += " ";
            }
            command += ")'";
        }

        f = popen(command.c_str(), "r");
    }
    else
    {
        command = "zenity --file-selection ";
        if (!title.empty()) command += "--title='" + title + "' ";
        if (saveFile) command += "--save ";
        if (!fileTypeName.empty()) command += "--file-filter='" + fileTypeName;
        if (!typeFilter.empty())
        {
            command += " (";
            for (std::size_t i = 0; i < typeFilter.size(); ++i)
            {
                command += typeFilter[i];
                if (i != typeFilter.size() - 1) command += ", ";
            }
            command += ") | ";
            for (std::size_t i = 0; i < typeFilter.size(); ++i)
            {
                command += "*." + typeFilter[i];
                if (i != typeFilter.size() - 1) command += ' ';
            }
            command += "' ";
        }

        f = popen(command.c_str(), "r");
    }

    if (fgets(filename, 1024, f))
    {
        std::string path = filename;
        lastPath_ = path.substr(0, path.find_last_of('/') + 1);
        return path.substr(0, path.size() - 1);
    }
#endif

    return "";
}
}    // namespace neko

bool ImGui::ButtonCentered(std::string_view label, const ImVec2& size)
{
	const float framePaddingX = GetStyle().FramePadding.x;
	const float textSize      = CalcTextSize(label.data()).x;

	NewLine();
	if (size.x != 0.0f) SameLine(GetWindowSize().x / 2.0f - size.x + (size.x / 2.0f));
	else
		SameLine(GetWindowSize().x / 2.0f - textSize + (textSize / 2.0f) - framePaddingX);
	return Button(label.data(), size);
}

void ImGui::TextCentered(std::string_view text)
{
    const float textSize = CalcTextSize(text.data()).x;

    NewLine();
    SameLine(GetWindowSize().x / 2.0f - textSize + (textSize / 2.0f));
    Text("%s", text.data());
}

int ImGui::InputTextCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		auto* str = static_cast<std::string*>(data->UserData);
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = const_cast<char*>(str->c_str());
	}

	return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
{
	flags |= ImGuiInputTextFlags_CallbackResize;
	return InputText(label,
		const_cast<char*>(str->c_str()),
		str->capacity() + 1,
		flags,
		InputTextCallback,
		static_cast<void*>(str));
}

bool ImGui::Combo(const char* label,
	int* currentItem,
	const std::vector<std::string>& items,
	const int heightInItems)
{
	const auto lambda = [](void* data, const int idx, const char** outText)
	{
		*outText = static_cast<const std::vector<std::string>*>(data)->at(idx).c_str();
		return true;
	};

	return Combo(label, currentItem, lambda, (void*) &items, items.size(), heightInItems);
}

bool ImGui::ImageButton(const std::string_view itemId,
	const neko::TextureName userTextureId,
	const ImVec2& size,
	const ImVec2& uv0,
	const ImVec2& uv1,
	const int framePadding,
	const ImVec4& bgCol,
	const ImVec4& tintCol)
{
	ImGuiContext& g     = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if (window->SkipItems) return false;

	auto* texId      = reinterpret_cast<ImTextureID>(userTextureId);
	const ImGuiID id = window->GetID(itemId.data());
	const ImVec2 padding =
		framePadding >= 0 ? ImVec2(float(framePadding), float(framePadding)) : g.Style.FramePadding;
	return ImageButtonEx(id, texId, size, uv0, uv1, padding, bgCol, tintCol);
}
