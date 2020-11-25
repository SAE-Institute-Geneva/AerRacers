LIST(APPEND aer_lib_libs Neko_Core sdl_engine gles3_wrapper assimp_wrapper physx_wrapper)

file(GLOB_RECURSE aer_lib_src include/*.h src/*.cpp)

add_library(aer_lib STATIC ${aer_lib_src})
target_link_libraries(aer_lib PUBLIC ${aer_lib_libs})

target_include_directories(aer_lib PUBLIC "include/")
neko_lib_config(aer_lib)
set_target_properties (aer_lib PROPERTIES FOLDER Neko/Common)

