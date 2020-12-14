#include "editor/tool/inspector.h"

namespace neko::aer
{
    Inspector::Inspector(ToolType type, int id, std::string name)
        : EditorToolInterface(type, id, name)
    {
    }
	
    void Inspector::Init() {}
    void Inspector::Update(seconds dt) {}
    void Inspector::Destroy() {}

    void Inspector::DrawImGui()
    {
        //If is True Display Window
        if (isVisible) {
            if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(),
                &isVisible)) {

                
            	


                
            }
            else {

                ImGui::Text("Test");
                ImGui::End();
            }
        }
    }
	
    void Inspector::OnEvent(const SDL_Event& event) {}

}