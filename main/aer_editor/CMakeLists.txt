if(Neko_GLES3)
    include_directories("include/")

    file(GLOB_RECURSE aer_editor_src include/*.h src/*.cpp)

    add_executable(aer_editor ${aer_editor_src})
    target_link_libraries(aer_editor PUBLIC Neko_Core aer_lib sdl_engine gles3_wrapper PhysX)
    neko_bin_config(aer_editor)
    data_generate(aer_editor)
    set_target_properties (aer_editor_Generate_Data PROPERTIES FOLDER Aer/Main)

    add_dependencies(aer_editor_Generate_Data GenerateDataTool)
    add_dependencies(aer_editor aer_editor_Generate_Data)

    if(EMSCRIPTEN)
        get_target_property(bin_link_flags aer_editor LINK_FLAGS)
        set_target_properties(aer_editor PROPERTIES LINK_FLAGS
                "${bin_link_flags} -s USE_SDL=2 -s USE_WEBGL2=1 ")
    endif()
    set_target_properties (aer_editor PROPERTIES FOLDER Aer/Main)
endif()
