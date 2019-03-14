cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cxxTools CXX)

set(CXXTOOLS_MAJOR_VERSION 0)
set(CXXTOOLS_MINOR_VERSION 0)
set(CXXTOOLS_PATCH_VERSION 1)
set(CXXTOOLS_VERSION 
	${CXXTOOLS_MAJOR_VERSION}.${CXXTOOLS_MINOR_VERSION}.${CXXTOOLS_PATCH_VERSION})

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/macros.cmake)
#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

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

file(GLOB HEADERS
	"include/*.h"
	"include/*.hpp"
)

file(GLOB SOURCES
	"source/*.cc"
	"source/*.cpp"
)

if(OpenCV_LIBS)

	file(GLOB HEADERS_CV
	    "include/opencv/*.h"
	    "include/opencv/*.hpp"
	)
	file(GLOB SOURCES_CV
	    "source/opencv/*.cc"
	    "source/opencv/*.cpp"
	)

	list(APPEND HEADERS ${HEADERS_CV})
	list(APPEND SOURCES ${SOURCES_CV})
endif(OpenCV_LIBS)

add_library(CxxTools ${HEADERS} ${SOURCES})
target_include_directories(CxxTools PRIVATE include ${EIGEN3_INCLUDE_DIR})
set_target_properties(CxxTools PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(CxxTools PROPERTIES CXX_STANDARD 11)

#####
# Install parameters
#####
SET(OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(INSTALL_LIB_DIR ${OUTPUT_PATH}/lib CACHE PATH "Installation directory for libraries")
SET(INSTALL_BIN_DIR ${OUTPUT_PATH} CACHE PATH "Installation directory for executables")
SET(INSTALL_INCLUDE_DIR ${OUTPUT_PATH}/include/Cxxtools CACHE PATH "Installation directory for header files")
SET(INSTALL_CMAKE_DIR ${OUTPUT_PATH}/cmake)

#####
# INSTALL 
#####
if(NOT(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT EQUAL FALSE))
    set (CMAKE_INSTALL_PREFIX ${OUTPUT_PATH} CACHE PATH "default install path" FORCE )
endif()

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    #message("var: " ${var})
  endif()
endforeach()

SET(PUBLIC_HEADERS)
FOREACH(var ${HEADERS})
  SET(PUBLIC_HEADERS "${PUBLIC_HEADERS}" "${var}")

ENDFOREACH(var ${HEADERS})
#message("PUBLIC_HEADERS: " ${PUBLIC_HEADERS})
SET_TARGET_PROPERTIES(CxxTools PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
UNSET(PUBLIC_HEADERS)

INSTALL(TARGETS  CxxTools 
	EXPORT CxxToolsTargets
	RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT bin
	ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
	LIBRARY DESTINATION  ${INSTALL_LIB_DIR} COMPONENT lib
	INCLUDES DESTINATION ${EIGEN3_INCLUDE_DIR} ${INSTALL_INCLUDE_DIR}/../ #${OUTPUT_PATH}/include
	PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT include# ${OUTPUT_PATH} include
)

setInstall(CxxTools CxxToolsTargets ${CXXTOOLS_VERSION} ${INSTALL_LIB_DIR} ${INSTALL_BIN_DIR} ${INSTALL_INCLUDE_DIR} ${INSTALL_CMAKE_DIR})

