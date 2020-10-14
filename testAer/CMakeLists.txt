

#Google Test


SET(Aer_TEST_DIR ${CMAKE_SOURCE_DIR}/testAer)
file(GLOB TEST_FILES ${Aer_TEST_DIR}/*.cpp)
add_executable(Aer_TEST ${TEST_FILES})

include_directories(${NEKO_CORE_DIR}/include)

target_link_libraries(Aer_TEST gtest gtest_main aer_lib Neko_Core sdl_engine gles3_wrapper)
neko_bin_config(Aer_TEST)
if(UNIX)
#target_compile_options(Aer_TEST PUBLIC "--coverage")
#target_link_options(Aer_TEST PUBLIC "--coverage")
endif()
set_target_properties (Aer_TEST PROPERTIES FOLDER Neko/Core)