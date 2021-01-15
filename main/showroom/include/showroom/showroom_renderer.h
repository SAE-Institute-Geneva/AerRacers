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
#include <array>
#include <memory>
#include <vector>

#include <SDL_events.h>
#include "ImGuizmo.h"
#include "sdl_engine/sdl_engine.h"

#include "gl/shape.h"
#include "graphics/lights.h"
#include "showroom/camera.h"
#include "showroom/model/model.h"
#include "showroom/showroom_engine.h"

namespace neko
{
const static float maxFloat = std::numeric_limits<float>::max();
const static float minFloat = std::numeric_limits<float>::lowest();
const static float smallFloat = 0.0001f;

struct Plane
{
	Plane() = default;
	explicit Plane(const neko::Vec3f& point, const neko::Vec3f& normal)
	   : point(point), normal(normal)
	{}

	explicit Plane(const Vec3f& pointA, const Vec3f& pointB, const Vec3f& pointC)
	   : point(pointA), normal(CalculateNormalFrom(pointA, pointB, pointC))
	{}

	[[nodiscard]] static neko::Vec3f CalculateNormalFrom(
		const Vec3f& pointA,
		const Vec3f& pointB,
		const Vec3f& pointC)
	{
		Vec3f vecA = pointA - pointB;
		Vec3f vecB = pointC - pointB;
		return Vec3f::Cross(vecA, vecB).Normalized();
	}

	[[nodiscard]] float Distance(const neko::Vec3f& pointA) const
	{
		return Vec3f::Dot(point - pointA, normal);
	}

	neko::Vec3f point   = Vec3f::zero;
	neko::Vec3f normal  = Vec3f::up;
};

class ShowRoomEngine;
class ShowRoomRenderer
		: public SystemInterface,
		public DrawImGuiInterface,
		public sdl::SdlEventSystemInterface,
		public RenderCommandInterface
{
public:
	explicit ShowRoomRenderer(ShowRoomEngine& engine) : engine_(engine) {}

    void Init() override;
    void Update(seconds dt) override;
    void Render() override;
    void DrawImGui() override;
    void Destroy() override;

    void OnEvent(const SDL_Event& event) override;

private:
    //Render
    void UpdateShader(const gl::Shader& shader);

	//Main
	void CreateDockableWindow();
	void DrawImGuizmo();
	void DrawMenuBar();
	void DrawSceneHierarchy();
	void DrawPropertiesWindow();
	void DrawToolWindow();

	//Scene Window
	void DrawMeshImGui(sr::Mesh& mesh);
	void DrawMeshImGuiObj(sr::Mesh& mesh);
	void DrawPopups();

	//Properties Window
	void DrawLightProperties();
	void DrawModelProperties();
	void DrawMeshProperties();
	void DrawTextureInput(
		size_t index,
		sr::Texture::TextureType type,
		std::vector<sr::Texture>& textures);
	void OpenTexture(size_t index, std::vector<sr::Texture>& textures);
	void OpenTexture(sr::Texture::TextureType type, std::vector<sr::Texture>& textures);

	//Light properties
	void DrawPointLight();
	void DrawDirectionalLight();
	void DrawSpotLight();

	ShowRoomEngine& engine_;

	enum LightType : uint8_t
	{
		NO_LIGHT = 0,
		POINT,
		SUN,
		SPOT
	};

	enum SelectedNode : uint8_t
	{
		NONE = 0,
		LIGHT,
		MODEL
	};

	Job preRender_;
	gl::TextureManager textureManager_;
	sr::ShowRoomCamera3D camera_;

    Mat4f modelMat_;
	sr::Model model_;
	gl::Shader shader_;

	//Lights
	LightType lightType_ = LightType::SUN;
	PointLight pointLight_;
	DirectionalLight directionalLight_;
	SpotLight spotLight_;
	EulerAngles lightOrientation_;

	//Dock Window
	static const ImGuiDockNodeFlags dockspaceFlags_ =
			ImGuiDockNodeFlags_NoDockingInCentralNode |
			ImGuiDockNodeFlags_AutoHideTabBar |
			ImGuiDockNodeFlags_PassthruCentralNode;
	std::vector<std::string> popupNamesStack_;
	ImGuizmo::OPERATION manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE;

	//Scene Window
	ImGuiTextFilter searchFilter_;
	TextureName searchIcon_ = INVALID_TEXTURE_NAME;

	//Tools Window
	float focalLength_ = 50.0f;
	float sensorSize_ = 36.0f;

	//Meshes Window
	bool popupToggled_ = false;
	XXH64_hash_t popup_ = 0;
	std::string newNameBuffer_;
	uint8_t selectedNode_ = 0;
	uint8_t selectionOffset_ = 3;
	TextureName folderIcon_ = INVALID_TEXTURE_NAME;
	TextureName deleteIcon_ = INVALID_TEXTURE_NAME;

	//Properties Window
	std::vector<std::string> lightLabels_ = { "No Light", "Point", "Sun", "Spot" };
};
}

namespace ImGui
{
bool ButtonCentered(std::string_view label, const ImVec2& size = ImVec2(0, 0));
void TextCentered(std::string_view text);

static int InputTextCallback(ImGuiInputTextCallbackData* data);
bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags);

static bool Combo(const char* label, int* currentItem, const std::vector<std::string>& outText, int heightInItems = -1);

bool ImageButton(
	std::string_view itemId,
	neko::TextureName userTextureId,
	const ImVec2& size,
	const ImVec2& uv0 = neko::Vec2f::zero,
	const ImVec2& uv1 = neko::Vec2f::one,
	int framePadding = -1,
	const ImVec4& bgCol = neko::Color::clear,
	const ImVec4& tintCol = neko::Color::white);
}