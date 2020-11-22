
	set(PHYSX_INCLUDE_DIRS ${PHYSX_ROOT_DIR}/include/ ${PHYSX_ROOT_DIR}/../pxshared/include/ )
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

	set(CMAKE_CONFIGURATION_TYPES CACHE STRING "debug") #enum configTypes {debug checked profile release}
	set(TARGET_BUILD_PLATFORM "windows") # has to match the TARGET_BUILD_PLATFORM in $ENV{PHYSX_PATH}/physix/buildtools/cmake_generate_projects.py
	set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
	set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
	set(PX_GENERATE_STATIC_LIBRARIES OFF CACHE BOOL "Generate static libraries")
	set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
	set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
	set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
	set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
	set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
	set(CMAKEMODULES_VERSION "1.27")
	set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
	set(PX_OUTPUT_LIB_DIR ${CMAKE_SOURCE_DIR}/build/externals/PhysX-4.1)
	set(PX_OUTPUT_BIN_DIR ${CMAKE_SOURCE_DIR}/build/externals/PhysX-4.1)
	
LIST(APPEND PHYSX_WRAPPER_LIBS Neko_Core ${PHYSX_LIBRARIES})
file(GLOB_RECURSE physx_wrapper_src include/*.h src/*.cpp)
add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public ${PHYSX_DIR})

add_library(physx_wrapper STATIC ${physx_wrapper_src})
target_link_libraries(physx_wrapper PUBLIC ${PHYSX_WRAPPER_LIBS})
target_include_directories(physx_wrapper PUBLIC "include/")
set_target_properties (physx_wrapper PROPERTIES FOLDER Neko/Common)