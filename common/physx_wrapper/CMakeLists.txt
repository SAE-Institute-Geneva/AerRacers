
set(PHYSX_INCLUDE_DIRS ${PHYSX_ROOT_DIR}/include/ ${PHYSX_ROOT_DIR}/../pxshared/include/)
set(PHYSX_LIBRARIES
		PhysXExtensions
		PhysX
		PhysXPvdSDK
		PhysXVehicle
		PhysXCharacterKinematic
		PhysXCooking
		PhysXCommon
		PhysXFoundation
		# SnippetUtils
		)

# has to match the TARGET_BUILD_PLATFORM in $ENV{PHYSX_PATH}/physix/buildtools/cmake_generate_projects.py
if (MSVC)
	set(TARGET_BUILD_PLATFORM "windows")
	set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
	set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
elseif (UNIX)
	set(TARGET_BUILD_PLATFORM "linux")
	set(CMAKE_VERBOSE_MAKEFILE ON)
	add_compile_options(-Wsuggest-override)
endif ()

set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR ${PROJECT_BINARY_DIR}/externals/PhysX-4.1)
set(PX_OUTPUT_BIN_DIR ${PROJECT_BINARY_DIR}/externals/PhysX-4.1)

list(APPEND PHYSX_WRAPPER_LIBS aer_lib Neko_Core gles3_wrapper ${PHYSX_LIBRARIES})
file(GLOB_RECURSE physx_wrapper_src include/*.h src/*.cpp)
add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public ${PHYSX_DIR})

add_library(physx_wrapper STATIC ${physx_wrapper_src})
target_link_libraries(physx_wrapper PUBLIC ${PHYSX_WRAPPER_LIBS})

if (MSVC)
	set(PHYSX_COMPILE_DEFS
			# Common to all configurations

			${PHYSX_WINDOWS_COMPILE_DEFS};${PHYSX_LIBTYPE_DEFS};${PHYSXGPU_LIBTYPE_DEFS}

			$<$<CONFIG:Debug>:${PHYSX_WINDOWS_DEBUG_COMPILE_DEFS};>
			$<$<CONFIG:Checked>:${PHYSX_WINDOWS_CHECKED_COMPILE_DEFS};>
			$<$<CONFIG:Profile>:${PHYSX_WINDOWS_PROFILE_COMPILE_DEFS};>
			$<$<CONFIG:Release>:${PHYSX_WINDOWS_RELEASE_COMPILE_DEFS};>
			)
elseif (UNIX)
	set(PHYSX_COMPILE_DEFS
			# Common to all configurations

			${PHYSX_LINUX_COMPILE_DEFS};${PHYSX_LIBTYPE_DEFS};${PHYSXGPU_LIBTYPE_DEFS}

			$<$<CONFIG:Debug>:${PHYSX_LINUX_DEBUG_COMPILE_DEFS};>
			$<$<CONFIG:Checked>:${PHYSX_LINUX_CHECKED_COMPILE_DEFS};>
			$<$<CONFIG:Profile>:${PHYSX_LINUX_PROFILE_COMPILE_DEFS};>
			$<$<CONFIG:Release>:${PHYSX_LINUX_RELEASE_COMPILE_DEFS};>
			)
endif ()

target_compile_definitions(physx_wrapper PRIVATE ${PHYSX_COMPILE_DEFS})
target_include_directories(physx_wrapper PUBLIC "include/")
set_target_properties (physx_wrapper PROPERTIES FOLDER Neko/Common)