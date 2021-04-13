cmake_minimum_required(VERSION 3.16)
project(Neko)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes different utility functions
include(cmake/data.cmake)
include(cmake/data_generation.cmake)
include(cmake/neko_executable.cmake)
add_compile_definitions("FMT_HEADER_ONLY=1")

# Selects the correct toolchain to use
message(STATUS "CMake System Name: ${CMAKE_SYSTEM_NAME}")
if (MSVC)
    include(cmake/platforms/msvc.cmake)
elseif (UNIX)
	string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    if (BUILD_TYPE STREQUAL "DEBUG")
        message(VERBOSE "Enabled '-fstandalone-debug' for LLDB")
        add_compile_options(-fstandalone-debug)
    endif ()

    execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
    message(STATUS "UNIX Architecture: ${ARCHITECTURE}")
    if ("${ARCHITECTURE}" MATCHES "arm")
        include(cmake/platforms/arm.cmake)
    elseif ("${ARCHITECTURE}" MATCHES "aarch")
        include(cmake/platforms/arm.cmake)
    else ()
        include(cmake/platforms/linux.cmake)
    endif ()
endif ()

# Sets paths
set_property(GLOBAL PROPERTY USE_FOLDERS On)
set(NEKO_CORE_DIR "${CMAKE_SOURCE_DIR}/core" CACHE INTERNAL "")
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/externals" CACHE INTERNAL "Externals folder")
set(UTILITIES_DIR "${EXTERNAL_DIR}/utilities" CACHE INTERNAL "")
set(MAIN_PROJECTS_DIR "${CMAKE_SOURCE_DIR}/main")
set(TOOL_PROJECTS_DIR "${CMAKE_SOURCE_DIR}/tools")

# Different CMake variables
set(Aer_Lib ON CACHE BOOL "Activate Aer_Lib")
set(Aer_Test ON CACHE BOOL "Activate Aer Test")
set(Neko_Assert ON CACHE BOOL "Activate Neko Assertion")
set(Neko_Benchmark OFF CACHE BOOL "Activate Benchmark")
set(Neko_FMOD ON CACHE BOOL "Activate FMOD")
set(Neko_Freetype ON CACHE BOOL "Activate Freetype")
set(Neko_GLES3 ON CACHE BOOL "Activate OpenGL ES 3.0")
set(Neko_ImGui ON CACHE BOOL "Activate ImGui")
set(Neko_KTX ON CACHE BOOL "Activate KTX")
set(Neko_PhysFS ON CACHE BOOL "Activate PhysFS Wrapper")
set(Neko_PhysX ON CACHE BOOL "Activate PhysX")
set(Neko_Profile OFF CACHE BOOL "Activate Profiling with Easy Profile")
set(Neko_SDL2 ON CACHE BOOL "Activate SDL2")
set(Neko_SFML_NET OFF CACHE BOOL "Activate SFML Net Wrapper")
set(Neko_Test ON CACHE BOOL "Activate Neko Test")
set(Neko_VULKAN ON CACHE BOOL "Activate Vulkan")

# Enables google test
if(Neko_Test OR Aer_Test)
    enable_testing()
    include(cmake/test.cmake)
    set(GOOGLE_TEST_DIR ${EXTERNAL_DIR}/googletest-1.8.1)
    set(BUILD_GMOCK OFF CACHE INTERNAL "")
    set(INSTALL_GTEST OFF CACHE INTERNAL "")
    add_subdirectory(${GOOGLE_TEST_DIR})

    set_target_properties(gtest PROPERTIES FOLDER Externals/GTest)
    set_target_properties(gtest_main PROPERTIES FOLDER Externals/GTest)
endif()

# Enables neko_assert
if(Neko_Assert)
    add_compile_definitions("NEKO_ASSERT=1")
endif()

# Enables Google Benchmark
if (Neko_Benchmark)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "")
    set(GOOGLE_BENCH_DIR "${EXTERNAL_DIR}/benchmark-1.5.2" CACHE STRING "")
    set(HAVE_STD_REGEX ON CACHE BOOL "")
    add_subdirectory(${GOOGLE_BENCH_DIR})
    add_subdirectory("benchmark/")
    neko_lib_config(benchmark)
    set_target_properties(benchmark PROPERTIES FOLDER Externals/GTest)
    set_target_properties(benchmark_main PROPERTIES FOLDER Externals/GTest)
endif ()

# Enables FMOD
if (Neko_FMOD)
    set(FMOD_ROOT "${EXTERNAL_DIR}/FMOD" CACHE STRING "")
    if (WIN32)
        include_directories("${FMOD_ROOT}/win64/inc")
    elseif (UNIX)
        include_directories("${FMOD_ROOT}/linux_x86_64/inc")
    endif ()
    add_subdirectory("${FMOD_ROOT}")
    set_target_properties(fmod PROPERTIES UNITY_BUILD ON)

    add_subdirectory(common/fmod_wrapper)
endif ()

# Enables Freetype
if (Neko_Freetype)
    add_compile_definitions("NEKO_FREETYPE=1")
    set(FREETYPE_DIR "${EXTERNAL_DIR}/freetype-2.10.0" CACHE STRING "")
    include_directories("${FREETYPE_DIR}")
    add_subdirectory("${FREETYPE_DIR}")
    set_target_properties(freetype PROPERTIES FOLDER Externals)
endif ()

# Assimp variables
set(ASSIMP_VERSION 5.0.1)
set(ASSIMP_DIR "${EXTERNAL_DIR}/assimp-${ASSIMP_VERSION}")
set(ASSIMP_NO_EXPORT ON CACHE INTERNAL "")
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
set(ASSIMP_INSTALL OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER OFF CACHE BOOL "" FORCE)

add_subdirectory("${ASSIMP_DIR}")
set_target_properties(assimp IrrXML PROPERTIES FOLDER Externals)

# Tiny obj loader
add_compile_definitions("TINYOBJLOADER_IMPLEMENTATION=1")

# Enables the Render Engine
if (Neko_GLES3 AND NOT Neko_VULKAN)
    add_compile_definitions("NEKO_GLES3=1")

    add_subdirectory("${EXTERNAL_DIR}/glad")
    set(GLES3_WRAPPER_DIR "common/gles3_wrapper" CACHE INTERNAL "")
    add_subdirectory(${GLES3_WRAPPER_DIR})
elseif (Neko_VULKAN)
    add_compile_definitions("NEKO_VULKAN=1")

    set(VULKAN_WRAPPER_DIR "common/vulkan_wrapper" CACHE INTERNAL "")
    add_subdirectory(${VULKAN_WRAPPER_DIR})
endif ()

# Enables ImGui
if (Neko_ImGui)
    add_compile_definitions("NEKO_IMGUI=1")
    set(IMGUI_VERSION "1.79WIP" CACHE INTERNAL "")
    set(IMGUI_ROOT "${EXTERNAL_DIR}/imgui-${IMGUI_VERSION}" CACHE STRING "")
    include_directories("${IMGUI_ROOT}")
    add_subdirectory("${IMGUI_ROOT}")
    set_target_properties(imgui PROPERTIES UNITY_BUILD ON)
endif ()

# Enables KTX
if(Neko_KTX)
    set(KTX_DIR "${EXTERNAL_DIR}/KTX-Software")
    set(KTX_VERSION_FULL "v4.0.0-beta4" CACHE STRING "")
    add_subdirectory("${KTX_DIR}")
    add_dependencies(ktx mkvk)
endif()

# Enables PhysFS
if(Neko_PhysFS)
    set(PHYSFS_DIR "${EXTERNAL_DIR}/physfs-3.0.2" CACHE STRING "")
    add_subdirectory(${PHYSFS_DIR})

    add_subdirectory(common/physfs_wrapper)
endif()

# Enables PhysX
if(Neko_PhysX)
    #### Build PhysX library ####
    set(PHYSX_VERSION 4.1)
    set(PHYSX_DIR ${EXTERNAL_DIR}/PhysX-${PHYSX_VERSION})
    set(PHYSX_ROOT_DIR ${PHYSX_DIR}/physx)
    if (EXISTS "${PHYSX_ROOT_DIR}/include/PxConfig.h")
        message(STATUS "PhysX is already built")
    else () #if (BUILD_TYPE STREQUAL "RELEASE")
        message(STATUS "Rebuilding PhysX")
        if (MSVC)
            execute_process(COMMAND "${PHYSX_ROOT_DIR}/generate_projects.bat" windows)
        elseif (UNIX)
            execute_process(COMMAND "${PHYSX_ROOT_DIR}/generate_projects.sh" linux)
        endif ()
    endif ()

    # Call into PhysX's CMake scripts
    add_subdirectory("common/physx_wrapper")
endif()

# Enables Easy Profile
if (Neko_Profile)
    message(STATUS "Enabling profiling...")
    set(EASY_PROFILE_DIR "${EXTERNAL_DIR}/easy_profiler-2.1.0")
    add_compile_definitions("EASY_PROFILE_USE=1")
    add_subdirectory(${EASY_PROFILE_DIR})

    set_target_properties(easy_profiler PROPERTIES FOLDER Externals)
    set_target_properties(profiler_converter PROPERTIES FOLDER Externals)
    set_target_properties(profiler_sample PROPERTIES FOLDER Externals)
    set_target_properties(profiler_reader PROPERTIES FOLDER Externals)
    set_target_properties(profiler_sample_disabled_profiler PROPERTIES FOLDER Externals)
endif ()

# Enables SDL2
if (Neko_SDL2)
    set(SDL_VERSION 2.0.12)
    set(SDL_DIR ${EXTERNAL_DIR}/SDL2-${SDL_VERSION}/)
    set(SDL_STATIC ON CACHE INTERNAL "")
    set(SDL_SHARED OFF CACHE INTERNAL "")
    set(SDL_ENGINE_DIR ON CACHE INTERNAL "")
    add_subdirectory(${SDL_DIR})
    include_directories(${SDL_DIR}/include ${SDL_DIR}/src)

    add_subdirectory("common/sdl_engine")
    target_precompile_headers(sdl_engine PRIVATE "common/sdl_engine/include/sdl_wrapper_pch.h")
endif ()

# Enables SFML Net Wrapper
if (Neko_SFML_NET)
    set(SFML_DIR "${EXTERNAL_DIR}/SFML-2.5.1" CACHE STRING "")
    include_directories("${SFML_DIR}/include")
    add_subdirectory("${SFML_DIR}")
endif ()

# Enables aer_lib
if (Aer_Lib)
    add_subdirectory("common/aer_lib")
endif ()

# Includes Neko_Core
add_subdirectory(${NEKO_CORE_DIR})
target_precompile_headers(Neko_Core PRIVATE "${NEKO_CORE_DIR}/include/core_pch.h")

# Includes executables located in ${MAIN_PROJECTS_DIR}
message(STATUS "Looking for executables...")
file(GLOB MAIN_PROJECTS "${MAIN_PROJECTS_DIR}/*")
foreach (MAIN_PROJECT_PATH ${MAIN_PROJECTS})
    get_filename_component(MAIN_PROJECT_NAME ${MAIN_PROJECT_PATH} NAME)

    message(VERBOSE "${MAIN_PROJECT_PATH} found at: '${MAIN_PROJECT_NAME}'")
    set(${MAIN_PROJECT_NAME} ON CACHE BOOL "")
    if (${MAIN_PROJECT_NAME})
        add_subdirectory(${MAIN_PROJECT_PATH})
    endif ()
endforeach ()

# Includes external tools
message(STATUS "Looking for external tools...")
file(GLOB TOOL_PROJECTS "${TOOL_PROJECTS_DIR}/*")
foreach (TOOL_PROJECT_PATH ${TOOL_PROJECTS})
    get_filename_component(TOOL_PROJECT_NAME ${TOOL_PROJECT_PATH} NAME)

    message(VERBOSE "${TOOL_PROJECT_NAME} found at: '${TOOL_PROJECT_PATH}'")
    set(${TOOL_PROJECT_NAME} ON CACHE BOOL "")
    if (${TOOL_PROJECT_NAME})
        add_subdirectory(${TOOL_PROJECT_PATH})
    endif ()
endforeach ()