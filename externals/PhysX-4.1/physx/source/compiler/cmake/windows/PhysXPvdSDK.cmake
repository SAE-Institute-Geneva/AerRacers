##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##  * Neither the name of NVIDIA CORPORATION nor the names of its
##    contributors may be used to endorse or promote products derived
##    from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## Copyright (c) 2018-2019 NVIDIA Corporation. All rights reserved.

#
# Build PhysXPvdSDK
#

IF(PX_USE_NVTX)
	FIND_PACKAGE(nvToolsExt $ENV{PM_nvToolsExt_VERSION} REQUIRED)
	MESSAGE("Using nvtx lib: ${NVTOOLSEXT_LIB} path: ${NVTOOLSEXTSDK_PATH}")
	SET(NV_TOOLS_EXT_LIB ${NVTOOLSEXT_LIB})
ENDIF()

SOURCE_GROUP(resource FILES ${PHYSXPVDSDK_RESOURCE_FILE})

SOURCE_GROUP(include\\windows FILES ${PHYSXPVDSDK_PLATFORM_HEADERS})

SOURCE_GROUP(src\\src\\windows FILES ${PHYSXPVDSDK_PLATFORM_SOURCE})

SET(PHYSXPVDSDK_PLATFORM_FILES
	${PHYSXPVDSDK_RESOURCE_FILE}
	${PHYSXPVDSDK_PLATFORM_HEADERS}
	${PHYSXPVDSDK_PLATFORM_SOURCE}
)

SET(PHYSXPVDSDK_PLATFORM_INCLUDES
	${NVTOOLSEXT_INCLUDE_DIRS}
)

SET(PHYSXPVDSDK_LIBTYPE STATIC)

SET(PHYSXPVDSDK_PLATFORM_LINKED_LIBS ${NV_TOOLS_EXT_LIB})

# Use generator expressions to set config specific preprocessor definitions
SET(PHYSXPVDSDK_COMPILE_DEFS
	# Common to all configurations
	${PHYSX_WINDOWS_COMPILE_DEFS};PX_PHYSX_STATIC_LIB;${PHYSX_LIBTYPE_DEFS};${PHYSXGPU_LIBTYPE_DEFS}

	$<$<CONFIG:Debug>:${PHYSX_WINDOWS_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:Checked>:${PHYSX_WINDOWS_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:Profile>:${PHYSX_WINDOWS_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:Release>:${PHYSX_WINDOWS_RELEASE_COMPILE_DEFS};>
)

IF(NV_USE_GAMEWORKS_OUTPUT_DIRS AND PHYSXPVDSDK_LIBTYPE STREQUAL "STATIC")
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_DEBUG "PhysXPvdSDK_static${CMAKE_DEBUG_POSTFIX}")
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_CHECKED "PhysXPvdSDK_static${CMAKE_CHECKED_POSTFIX}")
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_PROFILE "PhysXPvdSDK_static${CMAKE_PROFILE_POSTFIX}")
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_RELEASE "PhysXPvdSDK_static${CMAKE_RELEASE_POSTFIX}")
ELSE()
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_DEBUG "PhysXPvdSDK${CMAKE_DEBUG_POSTFIX}")
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_CHECKED "PhysXPvdSDK${CMAKE_CHECKED_POSTFIX}")
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_PROFILE "PhysXPvdSDK${CMAKE_PROFILE_POSTFIX}")
	SET(PHYSXPVDSDK_COMPILE_PDB_NAME_RELEASE "PhysXPvdSDK${CMAKE_RELEASE_POSTFIX}")
ENDIF()

IF(PHYSXPVDSDK_LIBTYPE STREQUAL "SHARED")
	INSTALL(FILES $<TARGET_PDB_FILE:PhysXPvdSDK> 
		DESTINATION $<$<CONFIG:Debug>:${PX_ROOT_LIB_DIR}/Debug>$<$<CONFIG:Release>:${PX_ROOT_LIB_DIR}/Release>$<$<CONFIG:Checked>:${PX_ROOT_LIB_DIR}/Checked>$<$<CONFIG:Profile>:${PX_ROOT_LIB_DIR}/Profile> OPTIONAL)
ELSE()	
	INSTALL(FILES ${PHYSX_ROOT_DIR}/$<$<CONFIG:Debug>:${PX_ROOT_LIB_DIR}/Debug>$<$<CONFIG:Release>:${PX_ROOT_LIB_DIR}/Release>$<$<CONFIG:Checked>:${PX_ROOT_LIB_DIR}/Checked>$<$<CONFIG:Profile>:${PX_ROOT_LIB_DIR}/Profile>/$<$<CONFIG:Debug>:${PHYSXPVDSDK_COMPILE_PDB_NAME_DEBUG}>$<$<CONFIG:Checked>:${PHYSXPVDSDK_COMPILE_PDB_NAME_CHECKED}>$<$<CONFIG:Profile>:${PHYSXPVDSDK_COMPILE_PDB_NAME_PROFILE}>$<$<CONFIG:Release>:${PHYSXPVDSDK_COMPILE_PDB_NAME_RELEASE}>.pdb
		DESTINATION $<$<CONFIG:Debug>:${PX_ROOT_LIB_DIR}/Debug>$<$<CONFIG:Release>:${PX_ROOT_LIB_DIR}/Release>$<$<CONFIG:Checked>:${PX_ROOT_LIB_DIR}/Checked>$<$<CONFIG:Profile>:${PX_ROOT_LIB_DIR}/Profile> OPTIONAL)
ENDIF()


