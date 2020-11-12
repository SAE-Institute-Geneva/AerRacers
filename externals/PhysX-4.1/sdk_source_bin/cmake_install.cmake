# Install script for directory: C:/Storage/AerRacers/externals/PhysX-4.1/physx/source/compiler/cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Neko")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include/windows" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/windows/PsWindowsAoS.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/windows/PsWindowsFPU.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/windows/PsWindowsInclude.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/windows/PsWindowsInlineAoS.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/windows/PsWindowsIntrinsics.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/windows/PsWindowsTrigConstants.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/Neko/include/foundation/windows/PxWindowsIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/Neko/include/foundation/windows" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/windows/PxWindowsIntrinsics.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/Neko/include/foundation/unix/PxUnixIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/Neko/include/foundation/unix" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/unix/PxUnixIntrinsics.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXFoundationd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXFoundation.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXFoundation.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXFoundation.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxFoundation.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/foundation" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/foundation/PxAssert.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/foundation/PxFoundationConfig.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/foundation/PxMathUtils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/Ps.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsAlignedMalloc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsAlloca.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsAllocator.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsAoS.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsArray.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsAtomic.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsBasicTemplates.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsBitUtils.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsBroadcast.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsCpu.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsFoundation.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsFPU.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsHash.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsHashInternals.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsHashMap.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsHashSet.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsInlineAllocator.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsInlineAoS.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsInlineArray.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsIntrinsics.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsMathUtils.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsMutex.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsPool.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsSList.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsSocket.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsSort.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsSortInternals.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsString.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsSync.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsTempAllocator.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsThread.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsTime.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsUserAllocated.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsUtilities.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsVecMath.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsVecMathAoSScalar.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsVecMathAoSScalarInline.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsVecMathSSE.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsVecMathUtilities.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsVecQuat.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/foundation/include/PsVecTransform.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/Neko/include/foundation/Px.h;C:/Program Files (x86)/Neko/include/foundation/PxAllocatorCallback.h;C:/Program Files (x86)/Neko/include/foundation/PxProfiler.h;C:/Program Files (x86)/Neko/include/foundation/PxSharedAssert.h;C:/Program Files (x86)/Neko/include/foundation/PxBitAndData.h;C:/Program Files (x86)/Neko/include/foundation/PxBounds3.h;C:/Program Files (x86)/Neko/include/foundation/PxErrorCallback.h;C:/Program Files (x86)/Neko/include/foundation/PxErrors.h;C:/Program Files (x86)/Neko/include/foundation/PxFlags.h;C:/Program Files (x86)/Neko/include/foundation/PxIntrinsics.h;C:/Program Files (x86)/Neko/include/foundation/PxIO.h;C:/Program Files (x86)/Neko/include/foundation/PxMat33.h;C:/Program Files (x86)/Neko/include/foundation/PxMat44.h;C:/Program Files (x86)/Neko/include/foundation/PxMath.h;C:/Program Files (x86)/Neko/include/foundation/PxMemory.h;C:/Program Files (x86)/Neko/include/foundation/PxPlane.h;C:/Program Files (x86)/Neko/include/foundation/PxPreprocessor.h;C:/Program Files (x86)/Neko/include/foundation/PxQuat.h;C:/Program Files (x86)/Neko/include/foundation/PxSimpleTypes.h;C:/Program Files (x86)/Neko/include/foundation/PxStrideIterator.h;C:/Program Files (x86)/Neko/include/foundation/PxTransform.h;C:/Program Files (x86)/Neko/include/foundation/PxUnionCast.h;C:/Program Files (x86)/Neko/include/foundation/PxVec2.h;C:/Program Files (x86)/Neko/include/foundation/PxVec3.h;C:/Program Files (x86)/Neko/include/foundation/PxVec4.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/Neko/include/foundation" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/Px.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxAllocatorCallback.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxProfiler.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxSharedAssert.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxBitAndData.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxBounds3.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxErrorCallback.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxErrors.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxFlags.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxIntrinsics.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxIO.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxMat33.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxMat44.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxMath.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxMemory.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxPlane.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxPreprocessor.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxQuat.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxSimpleTypes.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxStrideIterator.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxTransform.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxUnionCast.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxVec2.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxVec3.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/../pxshared/include/foundation/PxVec4.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gpu" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/gpu/PxGpu.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cudamanager" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cudamanager/PxCudaContextManager.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cudamanager/PxCudaMemoryManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common/windows" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/windows/PxWindowsDelayLoadHook.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysX.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysX.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysX.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxActor.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxAggregate.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxArticulationReducedCoordinate.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxArticulationBase.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxArticulation.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxArticulationJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxArticulationJointReducedCoordinate.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxArticulationLink.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxBatchQuery.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxBatchQueryDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxBroadPhase.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxClient.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxConstraint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxConstraintDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxContact.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxContactModifyCallback.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxDeletionListener.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxFiltering.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxForceMode.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxImmediateMode.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxLockedData.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxMaterial.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxPhysics.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxPhysicsAPI.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxPhysicsSerialization.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxPhysicsVersion.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxPhysXConfig.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxPruningStructure.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxQueryFiltering.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxQueryReport.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxRigidActor.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxRigidBody.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxRigidDynamic.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxRigidStatic.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxScene.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxSceneDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxSceneLock.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxShape.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxSimulationEventCallback.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxSimulationStatistics.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxVisualizationParameter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxBase.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxCollection.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxCoreUtilityTypes.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxMetaData.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxMetaDataFlags.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxPhysicsInsertionCallback.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxPhysXCommonConfig.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxRenderBuffer.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxSerialFramework.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxSerializer.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxStringTable.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxTolerancesScale.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxTypeInfo.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/common/PxProfileZone.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pvd" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/pvd/PxPvdSceneClient.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/pvd/PxPvd.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/pvd/PxPvdTransport.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/collision" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/collision/PxCollisionDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/solver" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/solver/PxSolverDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/PxConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/debug/PhysXCharacterKinematic_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/checked/PhysXCharacterKinematic_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/profile/PhysXCharacterKinematic_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/release/PhysXCharacterKinematic_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/characterkinematic" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/characterkinematic/PxBoxController.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/characterkinematic/PxCapsuleController.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/characterkinematic/PxController.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/characterkinematic/PxControllerBehavior.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/characterkinematic/PxControllerManager.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/characterkinematic/PxControllerObstacles.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/characterkinematic/PxExtended.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXCommond.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXCommon.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXCommon.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXCommon.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geometry" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxBoxGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxCapsuleGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxConvexMesh.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxConvexMeshGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxGeometryHelpers.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxGeometryQuery.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxHeightField.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxHeightFieldDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxHeightFieldFlag.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxHeightFieldGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxHeightFieldSample.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxMeshQuery.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxMeshScale.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxPlaneGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxSimpleTriangleMesh.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxSphereGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxTriangle.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxTriangleMesh.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxTriangleMeshGeometry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geometry/PxBVHStructure.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geomutils" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geomutils/GuContactBuffer.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/geomutils/GuContactPoint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXCookingd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXCooking.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXCooking.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXCooking.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cooking" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/PxBVH33MidphaseDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/PxBVH34MidphaseDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/Pxc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/PxConvexMeshDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/PxCooking.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/PxMidphaseDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/PxTriangleMeshDesc.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/cooking/PxBVHStructureDesc.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/debug/PhysXExtensions_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/checked/PhysXExtensions_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/profile/PhysXExtensions_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/release/PhysXExtensions_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/extensions" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxBinaryConverter.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxBroadPhaseExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxCollectionExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxConstraintExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxContactJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxConvexMeshExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxD6Joint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxD6JointCreate.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxDefaultAllocator.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxDefaultCpuDispatcher.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxDefaultErrorCallback.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxDefaultSimulationFilterShader.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxDefaultStreams.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxDistanceJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxContactJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxExtensionsAPI.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxFixedJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxJointLimit.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxMassProperties.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxPrismaticJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxRaycastCCD.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxRepXSerializer.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxRepXSimpleType.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxRevoluteJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxRigidActorExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxRigidBodyExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxSceneQueryExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxSerialization.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxShapeExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxSimpleFactory.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxSmoothNormals.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxSphericalJoint.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxStringTableExt.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/extensions/PxTriangleMeshExt.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/filebuf" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/filebuf/PxFileBuf.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/debug/PhysXVehicle_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/checked/PhysXVehicle_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/profile/PhysXVehicle_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/release/PhysXVehicle_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleComponents.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleDrive.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleDrive4W.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleDriveNW.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleDriveTank.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleNoDrive.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleSDK.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleShaders.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleTireFriction.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleUpdate.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleUtil.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleUtilControl.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleUtilSetup.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleUtilTelemetry.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/vehicle/PxVehicleWheels.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/fastxml/include" TYPE FILE FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/source/fastxml/include/PsFastXml.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/debug/PhysXPvdSDK_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/checked/PhysXPvdSDK_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/profile/PhysXPvdSDK_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/release/PhysXPvdSDK_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/debug/PhysXTask_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/checked/PhysXTask_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/profile/PhysXTask_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1//physx/bin/win.x86_64.vc142.mt/release/PhysXTask_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/task" TYPE FILE FILES
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/task/PxCpuDispatcher.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/task/PxTask.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/task/PxTaskDefine.h"
    "C:/Storage/AerRacers/externals/PhysX-4.1//physx/include/task/PxTaskManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXFoundationd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXFoundation.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXFoundation.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXFoundation.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXFoundationd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXFoundation.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXFoundation.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXFoundation.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysX.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysX.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysX.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysX.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysX.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysX.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXCharacterKinematic_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXCharacterKinematic_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXCharacterKinematic_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXCharacterKinematic_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXPvdSDK_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXPvdSDK_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXPvdSDK_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXPvdSDK_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXCommond.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXCommon.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXCommon.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXCommon.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXCommond.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXCommon.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXCommon.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXCommon.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXCookingd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXCooking.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXCooking.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXCooking.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXCookingd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXCooking.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXCooking.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE SHARED_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXCooking.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXExtensions_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXExtensions_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXExtensions_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXExtensions_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXVehicle_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXVehicle_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXVehicle_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXVehicle_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/debug/PhysXTask_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/checked/PhysXTask_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/profile/PhysXTask_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/Storage/AerRacers/externals/PhysX-4.1/bin/win.x86_64.vc142.mt/release/PhysXTask_static.lib")
  endif()
endif()

