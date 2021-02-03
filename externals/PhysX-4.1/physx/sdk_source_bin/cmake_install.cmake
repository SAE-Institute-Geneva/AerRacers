# Install script for directory: /media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/compiler/cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PhysX")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include/unix" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/PsUnixAoS.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/PsUnixFPU.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/PsUnixInlineAoS.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/PsUnixIntrinsics.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/PsUnixTrigConstants.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include/unix/neon" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/neon/PsUnixNeonAoS.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/neon/PsUnixNeonInlineAoS.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include/unix/sse2" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/sse2/PsUnixSse2AoS.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/unix/sse2/PsUnixSse2InlineAoS.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/unix/PxUnixIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/unix" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/unix/PxUnixIntrinsics.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxFoundation.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/foundation" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/foundation/PxAssert.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/foundation/PxFoundationConfig.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/foundation/PxMathUtils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/Ps.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsAlignedMalloc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsAlloca.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsAllocator.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsAoS.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsArray.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsAtomic.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsBasicTemplates.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsBitUtils.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsBroadcast.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsCpu.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsFoundation.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsFPU.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsHash.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsHashInternals.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsHashMap.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsHashSet.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsInlineAllocator.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsInlineAoS.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsInlineArray.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsIntrinsics.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsMathUtils.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsMutex.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsPool.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsSList.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsSocket.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsSort.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsSortInternals.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsString.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsSync.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsTempAllocator.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsThread.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsTime.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsUserAllocated.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsUtilities.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsVecMath.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsVecMathAoSScalar.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsVecMathAoSScalarInline.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsVecMathSSE.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsVecMathUtilities.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsVecQuat.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/foundation/include/PsVecTransform.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/Px.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxAllocatorCallback.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxProfiler.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxSharedAssert.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxBitAndData.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxBounds3.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxErrorCallback.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxErrors.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxFlags.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxIntrinsics.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxIO.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxMat33.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxMat44.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxMath.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxMemory.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxPlane.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxPreprocessor.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxQuat.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxSimpleTypes.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxStrideIterator.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxTransform.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxUnionCast.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxVec2.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxVec3.h;/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation/PxVec4.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/install/linux/PxShared/include/foundation" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/Px.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxAllocatorCallback.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxProfiler.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxSharedAssert.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxBitAndData.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxBounds3.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxErrorCallback.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxErrors.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxFlags.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxIntrinsics.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxIO.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxMat33.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxMat44.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxMath.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxMemory.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxPlane.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxPreprocessor.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxQuat.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxSimpleTypes.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxStrideIterator.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxTransform.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxUnionCast.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxVec2.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxVec3.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/../pxshared/include/foundation/PxVec4.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gpu" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/gpu/PxGpu.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cudamanager" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cudamanager/PxCudaContextManager.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cudamanager/PxCudaMemoryManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxActor.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxAggregate.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxArticulationReducedCoordinate.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxArticulationBase.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxArticulation.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxArticulationJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxArticulationJointReducedCoordinate.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxArticulationLink.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxBatchQuery.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxBatchQueryDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxBroadPhase.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxClient.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxConstraint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxConstraintDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxContact.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxContactModifyCallback.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxDeletionListener.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxFiltering.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxForceMode.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxImmediateMode.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxLockedData.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxMaterial.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxPhysics.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxPhysicsAPI.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxPhysicsSerialization.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxPhysicsVersion.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxPhysXConfig.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxPruningStructure.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxQueryFiltering.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxQueryReport.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxRigidActor.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxRigidBody.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxRigidDynamic.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxRigidStatic.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxScene.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxSceneDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxSceneLock.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxShape.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxSimulationEventCallback.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxSimulationStatistics.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxVisualizationParameter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxBase.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxCollection.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxCoreUtilityTypes.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxMetaData.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxMetaDataFlags.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxPhysicsInsertionCallback.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxPhysXCommonConfig.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxRenderBuffer.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxSerialFramework.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxSerializer.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxStringTable.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxTolerancesScale.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxTypeInfo.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/common/PxProfileZone.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pvd" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/pvd/PxPvdSceneClient.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/pvd/PxPvd.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/pvd/PxPvdTransport.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/collision" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/collision/PxCollisionDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/solver" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/solver/PxSolverDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/PxConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/characterkinematic" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/characterkinematic/PxBoxController.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/characterkinematic/PxCapsuleController.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/characterkinematic/PxController.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/characterkinematic/PxControllerBehavior.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/characterkinematic/PxControllerManager.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/characterkinematic/PxControllerObstacles.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/characterkinematic/PxExtended.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geometry" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxBoxGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxCapsuleGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxConvexMesh.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxConvexMeshGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxGeometryHelpers.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxGeometryQuery.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxHeightField.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxHeightFieldDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxHeightFieldFlag.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxHeightFieldGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxHeightFieldSample.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxMeshQuery.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxMeshScale.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxPlaneGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxSimpleTriangleMesh.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxSphereGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxTriangle.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxTriangleMesh.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxTriangleMeshGeometry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geometry/PxBVHStructure.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geomutils" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geomutils/GuContactBuffer.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/geomutils/GuContactPoint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cooking" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/PxBVH33MidphaseDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/PxBVH34MidphaseDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/Pxc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/PxConvexMeshDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/PxCooking.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/PxMidphaseDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/PxTriangleMeshDesc.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/cooking/PxBVHStructureDesc.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/extensions" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxBinaryConverter.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxBroadPhaseExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxCollectionExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxConstraintExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxContactJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxConvexMeshExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxD6Joint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxD6JointCreate.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxDefaultAllocator.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxDefaultCpuDispatcher.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxDefaultErrorCallback.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxDefaultSimulationFilterShader.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxDefaultStreams.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxDistanceJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxContactJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxExtensionsAPI.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxFixedJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxJointLimit.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxMassProperties.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxPrismaticJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxRaycastCCD.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxRepXSerializer.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxRepXSimpleType.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxRevoluteJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxRigidActorExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxRigidBodyExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxSceneQueryExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxSerialization.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxShapeExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxSimpleFactory.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxSmoothNormals.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxSphericalJoint.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxStringTableExt.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/extensions/PxTriangleMeshExt.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/filebuf" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/filebuf/PxFileBuf.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleComponents.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleDrive.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleDrive4W.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleDriveNW.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleDriveTank.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleNoDrive.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleSDK.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleShaders.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleTireFriction.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleUpdate.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleUtil.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleUtilControl.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleUtilSetup.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleUtilTelemetry.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/vehicle/PxVehicleWheels.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/fastxml/include" TYPE FILE FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/source/fastxml/include/PsFastXml.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/task" TYPE FILE FILES
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/task/PxCpuDispatcher.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/task/PxTask.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/task/PxTaskDefine.h"
    "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/include/task/PxTaskManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysXFoundation_static_64.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysX_static_64.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysXCharacterKinematic_static_64.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysXPvdSDK_static_64.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysXCommon_static_64.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysXCooking_static_64.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysXExtensions_static_64.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/linux.clang/Release" TYPE STATIC_LIBRARY FILES "/media/Data/Repos/AerRacers/externals/PhysX-4.1/physx/bin/linux.clang/Release/libPhysXVehicle_static_64.a")
endif()

