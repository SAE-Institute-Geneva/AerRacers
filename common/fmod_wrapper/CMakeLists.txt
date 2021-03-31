LIST(APPEND FMOD_WRAPPER_LIBS Neko_Core fmod sdl_engine)

file(GLOB_RECURSE fmod_wrapper_src include/*.h src/*.cpp)
add_library(fmod_wrapper STATIC ${fmod_wrapper_src})
target_link_libraries(fmod_wrapper PRIVATE ${FMOD_WRAPPER_LIBS})

neko_lib_config(fmod_wrapper)
target_include_directories(fmod_wrapper PUBLIC "include/")
set_target_properties (fmod_wrapper PROPERTIES FOLDER Neko/Common)