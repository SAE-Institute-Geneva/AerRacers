#pragma once

/*
 MIT License

 Copyright (c) 2019 SAE Institute Switzerland AG

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
#include <engine/engine.h>
#include <engine/transform.h>
#include <engine/scene.h>
#include "entity_viewer.h"
#include "scene_viewer.h"
#include "inspector.h"
#include "editor_scene.h"
#include "log_viewer.h"
#include "previewer.h"
#include "physics_editor.h"
#include "editor_prefab.h"

#include <imgui.h>
#include "imfilebrowser.h"
#include "sfml_engine/sprite.h"
#include "sfml_engine/texture.h"
#include <engine/prefab.h>
#include "sfml_engine/engine.h"
#include "tools/engine_export.h"

namespace sf {
class Event;
}

namespace neko::editor
{
class BasicEditorSystem;
enum class FileOperation
{
    OPEN_SCENE,
    OPEN_PREFAB,
    SAVE_SCENE,
    SAVE_PREFAB,
    NONE,
};
enum class EditorMode : std::uint8_t
{
    SceneMode,
    PrefabMode,
    TextureMode,
    AnimMode
};



class NekoEditor : public neko::sfml::SfmlBasicEngine
{
public:
    NekoEditor();
    void Init() override;

    void EditorUpdate(float dt);

    void Destroy() override;

    //void SwitchEditorMode(EditorMode editorMode);
    //void SaveSceneEvent();
    //void SavePrefabEvent();

    void OnEvent(sf::Event& event) override;


protected:

    sf::RenderTexture sceneRenderTexture_;
    LogViewer logViewer_;

    ImGui::FileBrowser fileDialog_;
    EditorMode editorMode_ = EditorMode::SceneMode;
	std::vector<std::unique_ptr<BasicEditorSystem>> editorSystems_;
	Index currentSystemIndex = INVALID_INDEX;
	sfml::TextureManager textureManager_;
};

class BasicEditorSystem : public System
{
public:
	explicit BasicEditorSystem(Configuration& config);
	const std::string& GetSystemName() const { return editorSystemName_; }
	void SetSystemName(const std::string& name) { editorSystemName_ = name; }

	virtual void OnListingView() = 0;
	virtual void OnMainView() = 0;
	virtual void OnInspectorView() = 0;

	virtual void OnEvent(sf::Event& event){};
	virtual void OnLostFocus(){};
protected:
	std::string editorSystemName_;
	Configuration& config_;
};

class NekoEditorSystem : public BasicEditorSystem
{
public:
	explicit NekoEditorSystem(Configuration& config, sfml::TextureManager& textureManager);
protected:
	sf::RenderTexture screenRenderTexture_;
	NekoEditorExport editorExport_;
	EntityManager entityManager_;
	Position2dManager position2dManager_;
	Scale2dManager scale2dManager_;
	Rotation2dManager rotation2dManager_;
	sfml::Transform2dManager transform2dManager_;
	EditorSceneManager sceneManager_;
	box2d::BodyDef2dManager bodyDef2DManager_;
	sfml::SpriteManager spriteManager_;
	sfml::SpineManager spineManager_;
	BoxColliderDefManager boxColliderDefManager_;
	CircleColliderDefManager circleColliderDefManager_;
	PolygonColldierDefManager polygonColldierDefManager_;
	ColliderDefManager colliderManagerDefManager_;
	EditorPrefabManager prefabManager_;
	GraphicsManager graphicsManager_;
	EntityViewer entityViewer_;
	Inspector inspector_;
	SceneViewer sceneViewer_;
	
	FileOperation fileOperationStatus_ = FileOperation::NONE;
};

}
