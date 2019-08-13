# - Config file for the CxxTools package
# It defines the following variables
#  CxxTools_INCLUDE_DIRS - include directories for CxxTools
#  CxxTools_LIBRARIES    - libraries to link against
#  CxxTools_EXECUTABLE   - the bar executable

# Compute paths
get_filename_component(CxxTools_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(CxxTools_INCLUDE_DIRS "/home/deep/cxxtools;/home/deep/cxxtools/cmake-build-debug")

# Our library dependencies (contains definitions for IMPORTED targets)
if(NOT TARGET CxxTools AND NOT CxxTools_BINARY_DIR)
  include("${CxxTools_CMAKE_DIR}/CxxToolsTargets.cmake")
endif()

# These are IMPORTED targets created by CxxToolsTargets.cmake
set(CxxTools_LIBRARIES CxxTools)
#set(CxxTools_EXECUTABLE bar)
