#include "editor/tool/texture_tool.h"

neko::aer::TextureInterface::TextureInterface(ToolType type, int id, std::string name) 
	: EditorToolInterface(type, id, name)
{
}

void neko::aer::TextureInterface::Init()
{
}

void neko::aer::TextureInterface::Update(seconds dt)
{
}

void neko::aer::TextureInterface::DrawImGui()
{
	ImGui::Begin("Texture Tool");
	ImGui::Text("Hello World");
		//You can call ImGui drawing function
		ImGui::End();
}

void neko::aer::TextureInterface::Destroy()
{
}

void neko::aer::TextureInterface::OnEvent(const SDL_Event& event)
{
}
