SET(CMAKE_GENERATED ${CMAKE_BINARY_DIR}/CMakeCache.txt
	${CMAKE_BINARY_DIR}/cmake_install.cmake  
	${CMAKE_BINARY_DIR}/Makefile
	${CMAKE_BINARY_DIR}/CMakeFiles
	${CMAKE_BINARY_DIR}/CLEAN_ALL.cmake
	)

FOREACH(file_name ${CMAKE_GENERATED})
	IF(EXISTS ${file_name})
		FILE(REMOVE_RECURSE ${file_name})
	ENDIF()
ENDFOREACH(file_name)
