include_directories("include/")

file(GLOB_RECURSE aer_editor_src include/*.h src/*.cpp)

file(GLOB_RECURSE shaders_src ${CMAKE_SOURCE_DIR}/data/shaders/aer_racer/*.vert ${CMAKE_SOURCE_DIR}/data/shaders/aer_racer/*.frag)

source_group("Shaders"	FILES ${shaders_src})

add_executable(aer_editor ${aer_editor_src} ${shaders_src})

target_link_libraries(aer_editor PUBLIC Neko_Core aer_lib sdl_engine gles3_wrapper PhysX)
neko_bin_config(aer_editor)

if(EMSCRIPTEN)
    get_target_property(bin_link_flags aer_editor LINK_FLAGS)
    set_target_properties(aer_editor PROPERTIES LINK_FLAGS
            "${bin_link_flags} -s USE_SDL=2 -s USE_WEBGL2=1 ")
endif()
set_target_properties (aer_editor PROPERTIES FOLDER Neko\\Main)