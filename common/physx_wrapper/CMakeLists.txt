
LIST(APPEND PHYSX_WRAPPER_LIBS Neko_Core PhysX PhysXCommon PhysXExtensions)
file(GLOB_RECURSE physx_wrapper_src include/*.h src/*.cpp)

add_library(physx_wrapper STATIC ${physx_wrapper_src})
target_link_libraries(physx_wrapper PUBLIC ${PHYSX_WRAPPER_LIBS})
target_include_directories(physx_wrapper PUBLIC "include/")
set_target_properties (physx_wrapper PROPERTIES FOLDER Neko/Common)