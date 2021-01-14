if(Neko_VULKAN)
    include_directories("include/")

    file(GLOB_RECURSE vk_engine_src include/*.h src/*.cpp)

    add_executable(vk_engine ${vk_engine_src})
    target_link_libraries(vk_engine PUBLIC Neko_Core sdl_engine vulkan_wrapper)
    neko_bin_config(vk_engine)
    data_generate(vk_engine)
    set_target_properties (vk_engine_Generate_Data PROPERTIES FOLDER Vk/Main)

    add_dependencies(vk_engine_Generate_Data GenerateDataTool)
    add_dependencies(vk_engine vk_engine_Generate_Data)

    if(EMSCRIPTEN)
        get_target_property(bin_link_flags vk_engine LINK_FLAGS)
        set_target_properties(vk_engine PROPERTIES LINK_FLAGS
                "${bin_link_flags} -s USE_SDL=2 ")
    endif()
    set_target_properties (vk_engine PROPERTIES FOLDER Vk/Main)
endif()