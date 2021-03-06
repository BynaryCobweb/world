# Version minimum de cmake
cmake_minimum_required(VERSION 3.10)

# Declaration du projet
project(World)

# Includes cmake
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(cmake/WorldUtils.cmake)
include(3rdparty/rclib/RCLib.cmake)

# Options de l'utilisateur
option(WORLD_STATIC_BUILD "Build all libraries as static libraries" OFF)

option(WORLD_BUILD_TESTS "Build test targets" ON)
option(WORLD_BUILD_WORLD3D "Build the graphical demo" ON)
option(WORLD_BUILD_OPENCV_MODULES "Build the modules based on OpenCV" OFF)
option(WORLD_BUILD_VULKAN_MODULES "Build the modules based on Vulkan" ON)
option(WORLD_BUILD_PEACE "Build the native library for Peace Unity Plugin" ON)

# Setup variables for build configuration
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(${WORLD_STATIC_BUILD})
    set(WORLD_BUILD_MODE "STATIC")
    message(STATUS "Build libraries as static")
else()
    set(WORLD_BUILD_MODE "SHARED")
    message(STATUS "Build libraries as dynamic")
endif()

message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")

# Paramètres du projet
set(WORLD_VERSION 0.4)
set(WORLD_VERSION_MAJOR 0)
set(WORLD_VERSION_MINOR 4)
set(WORLD_VERSION_PATCH 0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Detection de la platforme
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WORLD_64_BITS ON)
    set(WORLD_BITS 64)
else()
    set(WORLD_32_BITS ON)
    set(WORLD_BITS 32)
endif()

message(STATUS "Detected ${WORLD_BITS} bits platform")

# Detection et paramètres du compilateur
if(CMAKE_CXX_COMPILER_ID MATCHES "^GNU$")
    message(STATUS "Compiler : GNU")
    set(WORLD_COMPILER_IS_GNU on)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -fPIC")

    if(CMAKE_BUILD_TYPE MATCHES "^Release$")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -s -Os -Wno-attributes")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^MSVC$")
    message(STATUS "Compiler : MSVC")
    set(WORLD_COMPILER_IS_MSVC on)
endif()


# Systeme dependant stuff
if(WIN32)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
endif()

if(WORLD_COMPILER_IS_MSVC)
	set(WORLD_BINARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
else()
	set(WORLD_BINARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()


# Dependances externes
message("--")
include(cmake/WorldDependencies.cmake)

# Dependances internes
add_subdirectory(3rdparty/)

include_directories(
        3rdparty/
		projects/)

# Ajout de World API
add_subdirectory(projects/world/)
# Ajout de VkWorld
add_subdirectory(projects/vkworld/)
# Ajout de World 3D
add_subdirectory(projects/world3D/)
# Ajout de Peace
add_subdirectory(projects/peace/)
# Ajout du répertoire de tests
add_subdirectory(tests/)

message("=====")

# Packaging
set(CPACK_PACKAGE_NAME "world")
set(CPACK_PACKAGE_VERSION_MAJOR ${WORLD_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${WORLD_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${WORLD_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "Bycob")
if (WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()

include(CPack)
