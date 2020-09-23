LIST(APPEND aerlib_libs Neko_Core sdl_engine gles3_wrapper)

file(GLOB_RECURSE aerlib_src include/*.h src/*.cpp)

add_library(aerlib STATIC ${aerlib_src})
target_link_libraries(aerlib PUBLIC ${aerlib_libs})

target_include_directories(aerlib PUBLIC "include/")
neko_lib_config(aerlib)
set_target_properties (aerlib PROPERTIES FOLDER Neko/Common)

