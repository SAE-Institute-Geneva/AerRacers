
#vulkan
LIST(APPEND VULKAN_WRAPPER_LIBS Neko_Core sdl_engine)

find_package(Vulkan REQUIRED)
include_directories(${VULKAN_INCLUDE_DIR})
LIST(APPEND VULKAN_WRAPPER_LIBS Vulkan::Vulkan)

file(GLOB_RECURSE vulkan_wrapper_src include/*.h src/*.cpp)
add_library(vulkan_wrapper STATIC ${vulkan_wrapper_src})
target_link_libraries(vulkan_wrapper PUBLIC ${VULKAN_WRAPPER_LIBS})

set_target_properties(vulkan_wrapper PROPERTIES UNITY_BUILD ON)

target_include_directories(vulkan_wrapper PUBLIC "include/")
neko_lib_config(vulkan_wrapper)
set_target_properties (vulkan_wrapper PROPERTIES FOLDER Neko/Common)