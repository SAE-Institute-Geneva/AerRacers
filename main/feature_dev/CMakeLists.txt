if (Neko_GLES3)
    include_directories("include/")

    file(GLOB_RECURSE feature_dev_src include/*.h src/*.cpp)

    add_executable(feature_dev ${feature_dev_src})
    target_link_libraries(feature_dev PUBLIC Neko_Core aer_lib sdl_engine gles3_wrapper PhysX)
    neko_bin_config(feature_dev)
    data_generate(feature_dev)
    set_target_properties(feature_dev PROPERTIES UNITY_BUILD ON)
    set_target_properties(feature_dev_Generate_Data PROPERTIES UNITY_BUILD ON)

    add_dependencies(feature_dev_Generate_Data GenerateDataTool)
    add_dependencies(feature_dev feature_dev_Generate_Data)

    if (EMSCRIPTEN)
        get_target_property(bin_link_flags feature_dev LINK_FLAGS)
        set_target_properties(feature_dev PROPERTIES LINK_FLAGS
                "${bin_link_flags} -s USE_SDL=2 -s USE_WEBGL2=1 ")
    else ()
        target_precompile_headers(feature_dev PRIVATE "include/dev/feature_dev_pch.h")
    endif ()
    set_target_properties(feature_dev PROPERTIES FOLDER Aer/Main)
endif ()
