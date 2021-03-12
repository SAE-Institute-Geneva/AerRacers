function(add_data_folder BINARY)
    file(GLOB_RECURSE DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/*")
    set(COPY_DATA_NAME "${BINARY}_Copy_Data")
    foreach (DATA ${DATA_FILES})
        if (IS_DIRECTORY ${DATA})
            continue()
        endif ()
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
    endforeach ()
    add_custom_target(${COPY_DATA_NAME} DEPENDS ${DATA_BINARY_FILES} ${DATA_FILES})
    add_dependencies(${BINARY} ${COPY_DATA_NAME})
endfunction()


