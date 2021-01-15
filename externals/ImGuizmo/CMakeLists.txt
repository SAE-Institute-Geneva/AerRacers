
add_library(ImGuizmo STATIC ImCurveEdit.cpp ImGradient.cpp ImGuizmo.cpp ImSequencer.cpp)
set_target_properties (ImGuizmo PROPERTIES FOLDER Externals)
target_include_directories(ImGuizmo PUBLIC "./")
target_link_libraries(ImGuizmo PRIVATE Neko_Core imgui)
add_compile_definitions("IMGUI_DEFINE_MATH_OPERATORS=1")
if(Emscripten)
    #set_property(TARGET imgui APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--shared-memory,--no-check-features ")
endif()