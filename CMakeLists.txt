cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cxxTools CXX)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/cmake_macro_install_uninstall/macro.cmake)

OPTION(BUILD_TEST "Build test" OFF)
#OPTION(BUILD_STATIC "Build static library?" OFF)
#SET(LIBRARY_TYPE)
#IF(BUILD_STATIC)
#	SET(LIBRARY_TYPE STATIC)
#ELSE()
#	SET(LIBRARY_TYPE SHARED)
#ENDIF()

# If on Mac OS X:
IF(${CMAKE_SYSTEM} MATCHES "Darwin")
    # Make sure that C++11 warnings are disabled.
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-extensions")

    # Make sure that the template depth is sufficient.
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=512")

    IF(${CMAKE_SYSTEM} MATCHES "Darwin-13.")
        # If on Mac OS X 10.9 (Mavericks), use the libstdc++ implementation of the C++ Standard Library and prevent C++11 code from being compiled.
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libstdc++")
        ADD_DEFINITIONS(-DNO_CPP11)
    ELSE()
        # Otherwise, use the libc++ implementation of the C++ Standard Library, and enable C++11 support.
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -std=c++11")
    ENDIF()
ENDIF()

find_package(Eigen3 REQUIRED)

file(GLOB headers
	"include/*.h"
	"include/*.hpp"
)

file(GLOB sources
	"source/*.cc"
	"source/*.cpp"
)

SET(targetname CxxTools)
setVersion(${targetname}_VERSION 0 0 2)

add_library(${targetname} ${LIBRARY_TYPE} ${headers} ${sources})
target_include_directories(${targetname} PRIVATE include ${EIGEN3_INCLUDE_DIR})
set_target_properties(${targetname} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${targetname} PROPERTIES CXX_STANDARD 11)
#set_property(TARGET ${targetname} PROPERTY POSITION_INDEPENDENT_CODE ON)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/SetEasyInstall.cmake)


IF(BUILD_TEST)
	ADD_SUBDIRECTORY(test)
ENDIF()