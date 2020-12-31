
function(add_data_folder binary)
    file(GLOB_RECURSE data_files "${CMAKE_CURRENT_SOURCE_DIR}/data/*")
    set(copy_data_name "${binary}_Copy_Data")
    foreach(DATA ${data_files})
        if(IS_DIRECTORY ${DATA})
            continue()
        endif()
        get_filename_component(FILE_NAME ${DATA} NAME)
        get_filename_component(PATH_NAME ${DATA} DIRECTORY)
        get_filename_component(EXTENSION ${DATA} EXT)
        file(RELATIVE_PATH PATH_NAME "${PROJECT_SOURCE_DIR}" ${PATH_NAME})
        set(DATA_OUTPUT "${PROJECT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")
        add_custom_command(
                OUTPUT ${DATA_OUTPUT}
                DEPENDS ${DATA}
                DEPENDS
                COMMAND ${CMAKE_COMMAND} -E copy ${DATA} "${PROJECT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}"
        )
        list(APPEND DATA_BINARY_FILES ${DATA_OUTPUT})
    endforeach()
    add_custom_target(
            ${copy_data_name}
            DEPENDS ${DATA_BINARY_FILES} ${DATA_FILES})
    add_dependencies(${binary} ${copy_data_name})

endfunction()


set(SUPPORTED_STAGES "vert" "frag" "geom" "comp" "tesc" "tese")
foreach(SHADER ${VK_SHADER_FILES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    get_filename_component(PATH_NAME ${SHADER} DIRECTORY)
    get_filename_component(EXTENSION ${SHADER} EXT)
    string(REPLACE ".vk" "" FILE_NAME ${FILE_NAME})

    file(RELATIVE_PATH PATH_NAME "${PROJECT_SOURCE_DIR}" ${PATH_NAME})
    set(SHADER_OUTPUT "${PROJECT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}.spv")
    
	foreach(SUPPORTED_STAGE ${SUPPORTED_STAGES})
		string(COMPARE EQUAL "${EXTENSION}" ".${SUPPORTED_STAGE}.vk" _cmp)
		if(_cmp)
			set(STAGE ${SUPPORTED_STAGE})
			break()
		endif()
	endforeach(SUPPORTED_STAGE)

    add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            DEPENDS ${SHADER}
            DEPENDS
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/${PATH_NAME}"
            COMMAND ${GLSL_VALIDATOR} -V -S "${STAGE}" "${SHADER}" -o ${SHADER_OUTPUT}
    )
    list(APPEND SHADER_BINARY_FILES ${SHADER_OUTPUT})
endforeach(SHADER)

add_custom_target(ShaderTarget DEPENDS ${SHADER_BINARY_FILES} ${shader_files})   
