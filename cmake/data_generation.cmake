find_package (Python3 REQUIRED COMPONENTS Interpreter)
message(STATUS "Python Interpreter: ${Python3_EXECUTABLE}")

file(GLOB_RECURSE SCRIPT_FILES "${CMAKE_SOURCE_DIR}/scripts/*.py")
file(GLOB_RECURSE VALIDATOR_FILES "${CMAKE_SOURCE_DIR}/validator/*.json")

source_group("Scripts"   FILES ${SCRIPT_FILES})
source_group("Validator" FILES ${VALIDATOR_FILES})
function(data_generate BINARY)
    set(CMAKE_BUILD_TYPE Release)
    file(GLOB_RECURSE AER_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.aermat"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.aerscene"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.aershader")
    file(GLOB_RECURSE FONT_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.ttf")
    file(GLOB_RECURSE FMOD_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.bank")
    file(GLOB_RECURSE IMG_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.bmp"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.dds"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.gif"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.hdr"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.jpeg"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.jpg"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.ktx"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.pic"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.png"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.psd"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.tga")
    file(GLOB_RECURSE MATERIAL_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.mat"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.mtl")
    file(GLOB_RECURSE MODEL_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.obj"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.fbx")
    file(GLOB_RECURSE PKG_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.pkg_json")
    file(GLOB_RECURSE SKY_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.skybox")
    file(GLOB_RECURSE SND_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.ogg"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.wav")
    file(GLOB_RECURSE TEXT_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.json"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.scene"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.txt")
    file(GLOB_RECURSE SHADER_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.comp"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.frag"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.geom"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.glsl"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.tesc"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.tese"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/*.vert")
	file(GLOB_RECURSE VK_SHADER_FILES
			"${CMAKE_CURRENT_SOURCE_DIR}/data/*.vk")

    set(data_generate_name "${BINARY}_Generate_Data")
    source_group("Data/AerFiles"    FILES ${AER_FILES})
    source_group("Data/Font"        FILES ${FONT_FILES})
    source_group("Data/Fmod"        FILES ${FMOD_FILES})
    source_group("Data/Img"         FILES ${IMG_FILES})
    source_group("Data/Materials"   FILES ${MATERIAL_FILES})
    source_group("Data/Model"       FILES ${MODEL_FILES})
    source_group("Data/Package"     FILES ${PKG_FILES})
    source_group("Data/Skybox"		FILES ${SKY_FILES})
    source_group("Data/Snd"			FILES ${SND_FILES})
    source_group("Data/Text"        FILES ${TEXT_FILES})
    source_group("Shaders"		    FILES ${SHADER_FILES})
    source_group("VkShaders"		FILES ${VK_SHADER_FILES})
    list(APPEND DATA_FILES
            ${AER_FILES}
            ${FONT_FILES}
            ${FMOD_FILES}
            ${IMG_FILES}
            ${MATERIAL_FILES}
            ${MODEL_FILES}
            ${PKG_FILES}
            ${SHADER_FILES}
            ${SKY_FILES}
            ${SND_FILES}
            ${TEXT_FILES}
            ${VK_SHADER_FILES})

    foreach (DATA ${DATA_FILES})
        get_filename_component(FILE_NAME ${DATA} NAME)
        get_filename_component(PATH_NAME ${DATA} DIRECTORY)
        get_filename_component(EXTENSION ${DATA} EXT)
        file(RELATIVE_PATH PATH_NAME "${CMAKE_CURRENT_SOURCE_DIR}" ${PATH_NAME})
        set(DATA_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")
        add_custom_command(
                OUTPUT ${DATA_OUTPUT}
                DEPENDS ${DATA} ${SCRIPT_FILES}
                DEPENDS
                COMMAND ${CMAKE_COMMAND} -E env SRC_FOLDER=${CMAKE_SOURCE_DIR} DATA_SRC_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/data" DATA_BIN_FOLDER="${CMAKE_CURRENT_BINARY_DIR}/data"
                "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/validator/asset_validator.py" "${DATA}" "${DATA_OUTPUT}"

        )
        list(APPEND DATA_BINARY_FILES ${DATA_OUTPUT})
    endforeach ()

    add_custom_target(${data_generate_name} DEPENDS ${DATA_BINARY_FILES} ${DATA_FILES})
    add_dependencies(${BINARY} ${data_generate_name})
endfunction()

if (MSVC)
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
    else ()
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
    endif ()
elseif (UNIX)
    set(GLSL_VALIDATOR "glslangValidator")
endif ()

find_program(GLSL_VALIDATOR_FOUND ${GLSL_VALIDATOR})
IF(NOT GLSL_VALIDATOR_FOUND)
    message(FATAL_ERROR "Please install VulkanSDK and put it in path (current path: $ENV{VULKAN_SDK})")
ENDIF()

add_custom_target(GenerateDataTool)
set_target_properties(GenerateDataTool PROPERTIES FOLDER __DO_THAT_FIRST_IN_RELEASE)
add_custom_command(TARGET GenerateDataTool
        COMMAND ${CMAKE_COMMAND} -E env
        BASISU_EXE=$<TARGET_FILE:basisu>
        GLSLANG_VALIDATOR_EXE=${GLSL_VALIDATOR}
        IMAGE_FORMAT_EXE=$<TARGET_FILE:image_format>
        SRC_FOLDER=${CMAKE_CURRENT_SOURCE_DIR}
        VALIDATE_JSON_EXE=$<TARGET_FILE:validate_json>
        VALIDATOR_FOLDER=${CMAKE_SOURCE_DIR}/validator/
        "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/data_tool_env.py")

add_dependencies(GenerateDataTool validate_json)
if (Neko_KTX)
    add_dependencies(GenerateDataTool basisu image_format)
endif ()