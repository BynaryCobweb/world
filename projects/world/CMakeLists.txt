
#Récuperation des sources
file(GLOB_RECURSE
	WORLD_SOURCES
	./*)

# Visual studio source groups
assign_source_group(${WORLD_SOURCES})

# Register target
add_library(world ${WORLD_BUILD_MODE} ${WORLD_SOURCES})

# External libraries
if (${WORLD_BUILD_OPENCV_MODULES})
	if (${WORLD_HAS_OPENCV})
		message(STATUS "Build OpenCV modules")
		target_link_libraries(world ${OpenCV_LIBS})
        target_compile_definitions(world PRIVATE WORLD_BUILD_OPENCV_MODULES=1)
	else()
		message(STATUS "Cannot build OpenCV modules : Missing dependencies (OpenCV)")
	endif()
endif()

# Register target
target_link_libraries(world ${THIRD_PARTY_LIBRARIES})
target_include_directories(world PUBLIC ${THIRD_PARTY_INCLUDES})

# Install target
install(TARGETS world
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION staticlib)
install(DIRECTORY .
	DESTINATION include/world
	FILES_MATCHING REGEX "\\.(h|inl)$"
	PATTERN "*_p.h" EXCLUDE)
