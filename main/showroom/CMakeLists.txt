if (Neko_GLES3 AND NOT Neko_VULKAN)
    include_directories("include/")

    file(GLOB_RECURSE showroom_src include/*.h src/*.cpp)

    add_executable(showroom ${showroom_src})
    target_link_libraries(showroom PUBLIC Neko_Core sdl_engine gles3_wrapper ImGuizmo)
    neko_bin_config(showroom)
    data_generate(showroom)
    set_target_properties(showroom PROPERTIES FOLDER Showroom)
    set_target_properties(showroom_Generate_Data PROPERTIES FOLDER Showroom)

    add_dependencies(showroom_Generate_Data GenerateDataTool)
    add_dependencies(showroom showroom_Generate_Data)

    if (EMSCRIPTEN)
        get_target_property(bin_link_flags showroom LINK_FLAGS)
        set_target_properties(showroom PROPERTIES LINK_FLAGS
                "${bin_link_flags} -s USE_SDL=2 -s USE_WEBGL2=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 --preload-file ${CMAKE_BINARY_DIR}/data/@ ")
        set_target_properties(showroom PROPERTIES SUFFIX ".html")
    endif ()
    set_target_properties(showroom PROPERTIES UNITY_BUILD ON)

    if (NOT Emscripten)
        target_precompile_headers(showroom PRIVATE "include/showroom/showroom_pch.h")
    endif ()
endif ()