
if (WIN32)
	file(GLOB_RECURSE FMOD_SRC *.h *.hpp)
	add_library(fmod INTERFACE ${FMOD_SRC})
    file(GLOB_RECURSE FMOD_LIBS *.lib)
    target_include_directories(fmod INTERFACE "win_x64/inc/")
    set(TEST_EXE_PATH  ${CMAKE_BINARY_DIR}/common/aer_lib/)
    set(GAME_EXE_PATH  ${CMAKE_BINARY_DIR}/main/aer_game/)
    set(EDITOR_EXE_PATH  ${CMAKE_BINARY_DIR}/main/aer_editor/)
	add_custom_target(fmodCopyBinaries
	  # todo: check if debug and release folder exist
	  # test debug version
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmod.dll                ${TEST_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodL.dll               ${TEST_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudio.dll          ${TEST_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudioL.dll         ${TEST_EXE_PATH}/Debug
	  
	  # test debug version
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmod.dll                ${TEST_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodL.dll               ${TEST_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudio.dll          ${TEST_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudioL.dll         ${TEST_EXE_PATH}/Release
	  
	  # editor debug version
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmod.dll                ${EDITOR_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodL.dll               ${EDITOR_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudio.dll          ${EDITOR_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudioL.dll         ${EDITOR_EXE_PATH}/Debug
	  
	  # editor debug version
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmod.dll                ${EDITOR_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodL.dll               ${EDITOR_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudio.dll          ${EDITOR_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudioL.dll         ${EDITOR_EXE_PATH}/Release
	  
	  # game debug version
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmod.dll                ${GAME_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodL.dll               ${GAME_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudio.dll          ${GAME_EXE_PATH}/Debug
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudioL.dll         ${GAME_EXE_PATH}/Debug
	  
	  # game debug version
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmod.dll                ${GAME_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodL.dll               ${GAME_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudio.dll          ${GAME_EXE_PATH}/Release
	  COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_ROOT}/win_x64/lib/fmodstudioL.dll         ${GAME_EXE_PATH}/Release
	  

	  # Output Message
	  COMMENT "Copying fmod binaries from '${FMOD_ROOT}/win_x64/lib/' to '${TEST_EXE_PATH}', '${EDITOR_EXE_PATH}' and '${GAME_EXE_PATH}'" VERBATIM
	)
    target_link_libraries(fmod INTERFACE ${FMOD_LIBS})

	add_dependencies(fmod fmodCopyBinaries)
	set_target_properties(fmodCopyBinaries PROPERTIES FOLDER Externals)
elseif (UNIX)
    file(GLOB_RECURSE FMOD_SRC *.h *.hpp)
    add_library(fmod INTERFACE ${FMOD_SRC})   
    file(GLOB_RECURSE FMOD_LIBS *.so)
    target_link_libraries(fmod INTERFACE ${FMOD_LIBS})
    target_include_directories(fmod INTERFACE "linux_x86_64/inc/")
endif ()
set_target_properties(fmod PROPERTIES FOLDER Externals)
target_link_libraries(fmod INTERFACE Neko_Core)