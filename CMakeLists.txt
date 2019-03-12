cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cxxTool CXX)

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
include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(include)

file(GLOB HEADERS
	"include/*.h"
	"include/*.hpp"
)

file(GLOB SOURCES
	"source/*.cc"
	"source/*.cpp"
)
file(GLOB LIB_TOOLS
    "include/*.h"
    "include/*.hpp"
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

add_library(CxxTool ${HEADERS} ${SOURCES})

set_target_properties(CxxTool PROPERTIES LINKER_LANGUAGE CXX)


#####
# Install parameters
#####
SET(OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(INSTALL_CMAKE_PATH ${OUTPUT_PATH}/cmake)
SET(INSTALL_HEADER_PATH ${OUTPUT_PATH}/include/cpptool)
SET(INSTALL_LIB_PATH ${OUTPUT_PATH}/lib)

#####
# INSTALL 
#####
INSTALL(TARGETS  CxxTool
	ARCHIVE DESTINATION ${INSTALL_LIB_PATH} 
	LIBRARY DESTINATION  ${INSTALL_LIB_PATH} 
	INCLUDES DESTINATION "include/"
)

INSTALL(FILES ${HEADERS}
	DESTINATION ${INSTALL_HEADER_PATH} COMPONENT runtime
)

configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/FindCxxTool.cmake.in
		${INSTALL_CMAKE_PATH}/FindCxxTool.cmake
		IMMEDIATE @ONLY)

#######################
# Uninstall target, for "make uninstall"
#######################
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
