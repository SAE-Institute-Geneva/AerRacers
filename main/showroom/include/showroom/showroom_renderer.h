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

#include <SDL_events.h>
#include "ImGuizmo.h"
#include "sdl_engine/sdl_engine.h"

#include "graphics/lights.h"
#include "showroom/camera.h"
#include "showroom/gizmos_renderer.h"
#include "showroom/model/model.h"
#include "showroom/mouse_events.h"

namespace neko
{
constexpr float maxFloat = std::numeric_limits<float>::max();
constexpr float minFloat = std::numeric_limits<float>::lowest();
constexpr float smallFloat = 0.1f;

struct Framebuffer
{
    explicit Framebuffer(const Vec2u size) : size(size) {}

	virtual void Destroy()
	{
		glDeleteFramebuffers(1, &fbo);
	}

	void Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	static void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    static void Clear(const Color4& color, bool clearDepth = true)
    {
		glClearColor(color.x, color.y, color.z, color.w);
		glClear(GL_COLOR_BUFFER_BIT | (clearDepth ? GL_DEPTH_BUFFER_BIT : 0));
	}

	void RetrieveDepth() const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCheckError();
	}

	Vec2u size;
	unsigned fbo = 0;
};

struct BloomFbo : Framebuffer
{
    explicit BloomFbo(const Vec2u size) : Framebuffer(size) {}

    void Destroy() override
    {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(3, &colorBuffers[0]);
        glDeleteRenderbuffers(1, &depthRbo);
    }

    unsigned colorBuffers[3] {};
    unsigned depthRbo = 0;
};

struct BlurFbo : Framebuffer
{
    explicit BlurFbo(const Vec2u size) : Framebuffer(size) {}

    void Destroy() override
    {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &colorBuffer);
    }

    unsigned colorBuffer = 0;
};

class ShowRoomEngine;
class ShowRoomRenderer final
    : public SystemInterface,
      public DrawImGuiInterface,
      public sdl::SdlEventSystemInterface,
      public RenderCommandInterface
{
public:
	explicit ShowRoomRenderer(ShowRoomEngine& engine);

    void Init() override;
    void Update(seconds dt) override;
    void Render() override;
    void DrawImGui() override;
    void Destroy() override;

    void OnEvent(const SDL_Event& event) override;

private:
    //Render
    void CreateFramebuffers();
    void UpdateShader(const gl::Shader& shader) const;
	void DrawGrid();

    //Utils
    std::string OpenFileExplorer(const std::string& title = "",
        const std::string& fileTypeName                   = "",
        const std::vector<std::string>& typeFilter        = {},
        bool saveFile                                     = false);

    //Main
	void CreateDockableWindow();
	void DrawImGuizmo();
	void DrawMenuBar();
	void DrawInfoBar();
	void DrawSceneHierarchy();
	void DrawPropertiesWindow();
	void DrawToolWindow();

	//Tool Window
	void DrawLightTransform();
	void DrawModelTransform();
	void DrawMeshTransform(sr::Mesh& mesh);

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

    std::string lastPath_ = ". ";

	Job preRender_;
	sr::TextureManager textureManager_;
	sr::GizmoRenderer gizmoRenderer_;
	sr::ShowRoomCamera3D camera_;
    sr::MouseEvents mouseEvents_;
    bool isResized_ = false;

    Mat4f modelMat_;
	sr::Model model_;
	gl::Shader shader_;
	bool isModelLoading_;

	gl::Shader screenShader_;
	gl::Shader blurShader_;
	BloomFbo bloomFbo_;
	BlurFbo blurFbo_;
	gl::RenderQuad screenQuad_{Vec3f::zero, Vec2f::one * 2};

	//Lights
	LightType lightType_ = SUN;
	PointLight pointLight_;
	DirectionalLight dirLight_;
	SpotLight spotLight_;
	EulerAngles lightAngles_ = EulerAngles(45.0f, 0.0f, 0.0f);
	Vec3f lightScale_ = Vec3f::one;

	//Dock Window
    ImGuiID dockspaceId_;
	const ImGuiDockNodeFlags dockspaceFlags_ =
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

static bool Combo(const char* label, int* currentItem, const std::vector<std::string>& items, int heightInItems = -1);

bool ImageButton(
	std::string_view itemId,
	neko::TextureName userTextureId,
	const ImVec2& size,
	const ImVec2& uv0 = ImVec2(neko::Vec2f::zero),
	const ImVec2& uv1 = ImVec2(neko::Vec2f::one),
	int framePadding = -1,
	const ImVec4& bgCol = ImVec4(neko::Color::clear),
	const ImVec4& tintCol = ImVec4(neko::Color::white));
}