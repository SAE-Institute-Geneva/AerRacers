#pragma once
/* ----------------------------------------------------
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

 Author : Floreau Luca
 Co-Author :
 Date : 29.09.2020
---------------------------------------------------------- */

#include "sdl_engine/sdl_camera.h"

#include "aer/draw_system.h"
#include "aer/editor/editor_tool_manager.h"
#include "aer/gizmos_renderer.h"
#include "aer/inputs_binding_manager.h"
#include "aer/log.h"
#include "aer/managers/render_manager.h"
#include "aer/scene.h"
#include "aer/tag.h"
#include "engine/transform.h"

#ifdef NEKO_GLES3
	#include "gl/model.h"
#endif

namespace neko::aer
{
class AerEngine;

class DrawSystem final : public SystemInterface,
						 public sdl::SdlEventSystemInterface,
						 public DrawImGuiInterface
{
public:
	explicit DrawSystem(AerEngine& engine);
	void Init() override;
	void DrawImGui() override;

	void Update(seconds dt) override;
	void Destroy() override;
	void OnEvent(const SDL_Event& event) override;

protected:
	sdl::MovableCamera3D camera_;
	AerEngine& engine_;

	EntityManager entityManager_;
	SceneManager sceneManager_;

#ifdef NEKO_GLES3
	gl::TextureManager textureManager_;
	gl::ModelManager modelManager_;
#endif

	EditorToolManager toolManager_;

	//Components Manager
	Transform3dManager transform3dManager_;
	RenderManager renderManager_;

	//Service Locator
	std::unique_ptr<LogManager> logManager_;
	std::unique_ptr<TagManager> tagManager_;
	std::unique_ptr<InputBindingManager> boundInputManager_;

#ifdef NEKO_GLES3
	std::unique_ptr<GizmosRenderer> gizmosRenderer_;
#elif NEKO_VULKAN
	std::unique_ptr<NekoGizmosRenderer> gizmosRenderer_;
#endif

	Entity testEntity_;
};
}    // namespace neko::aer
