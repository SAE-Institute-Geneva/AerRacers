include_directories("include/")

file(GLOB_RECURSE aer_game_src include/*.h src/*.cpp)

file(GLOB_RECURSE shaders_src ${CMAKE_SOURCE_DIR}/data/shaders/aer_racer/*.vert ${CMAKE_SOURCE_DIR}/data/shaders/aer_racer/*.frag)

source_group("Shaders"	FILES ${shaders_src})

add_executable(aer_game ${aer_game_src} ${shaders_src})
target_link_libraries(aer_game PUBLIC Neko_Core aer_lib sdl_engine gles3_wrapper)
neko_bin_config(aer_game)
data_generate(aer_game)
set_target_properties (aer_game_Generate_Data PROPERTIES FOLDER Aer/Main)

if(EMSCRIPTEN)
    get_target_property(bin_link_flags aer_game LINK_FLAGS)
    set_target_properties(aer_game PROPERTIES LINK_FLAGS
            "${bin_link_flags} -s USE_SDL=2 -s USE_WEBGL2=1 ")
endif()
set_target_properties (aer_game PROPERTIES FOLDER Aer/Main)