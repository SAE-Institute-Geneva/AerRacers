include_directories("include/")

file(GLOB_RECURSE vk_engine_src include/*.h src/*.cpp)

file(GLOB_RECURSE shaders_src ${CMAKE_SOURCE_DIR}/data/shaders/aer_racer/*.vert ${CMAKE_SOURCE_DIR}/data/shaders/aer_racer/*.frag)

source_group("Shaders"	FILES ${shaders_src})

add_executable(vk_engine src/main.cpp "src/vk_draw_system.cpp" "include/vulkan_engine/vulkan_engine_pch.h")
target_link_libraries(vk_engine PUBLIC Neko_Core sdl_engine vulkan_wrapper)
neko_bin_config(vk_engine)

set_target_properties(vk_engine PROPERTIES UNITY_BUILD ON)
if(NOT Emscripten)
    target_precompile_headers(vk_engine PRIVATE "include/vulkan_engine/vulkan_engine_pch.h")
endif()

if(EMSCRIPTEN)
    get_target_property(bin_link_flags vk_engine LINK_FLAGS)
    set_target_properties(vk_engine PROPERTIES LINK_FLAGS
            "${bin_link_flags} -s USE_SDL=2 ")
endif()
set_target_properties (vk_engine PROPERTIES FOLDER Neko\\Main)