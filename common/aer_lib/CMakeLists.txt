
list(APPEND aer_lib_libs Neko_Core sdl_engine physx_wrapper)
if (Neko_FMOD)
    list(APPEND aer_lib_libs fmod_wrapper)
endif ()
if (Neko_GLES3 AND NOT Neko_VULKAN)
    list(APPEND aer_lib_libs gles3_wrapper)
elseif (Neko_VULKAN)
    list(APPEND aer_lib_libs vulkan_wrapper)
endif ()

file(GLOB_RECURSE aer_lib_src include/*.h src/*.cpp)

add_library(aer_lib STATIC ${aer_lib_src})
target_link_libraries(aer_lib PUBLIC ${aer_lib_libs})
target_include_directories(aer_lib PUBLIC "include/")
set_target_properties (aer_lib PROPERTIES FOLDER Neko/Common)
neko_lib_config(aer_lib)

if(Aer_Test)
    add_neko_test(aer_lib)
endif()

