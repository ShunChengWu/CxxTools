# Install script for directory: /home/deep/cxxtools

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/deep/cxxtools/bin")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/deep/cxxtools/bin/lib/libCxxTools.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/deep/cxxtools/bin/lib" TYPE STATIC_LIBRARY FILES "/home/deep/cxxtools/cmake-build-debug/libCxxTools.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xincludex" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/deep/cxxtools/bin/include/CxxTools/AwaitingThread.hpp;/home/deep/cxxtools/bin/include/CxxTools/HDF5Reader.hpp;/home/deep/cxxtools/bin/include/CxxTools/HDF5Saver.hpp;/home/deep/cxxtools/bin/include/CxxTools/HDF5Tools.hpp;/home/deep/cxxtools/bin/include/CxxTools/LogUtil.hpp;/home/deep/cxxtools/bin/include/CxxTools/MemoryBlock.hpp;/home/deep/cxxtools/bin/include/CxxTools/PathTool.hpp;/home/deep/cxxtools/bin/include/CxxTools/RotInvConvertor.hpp;/home/deep/cxxtools/bin/include/CxxTools/Tools.hpp;/home/deep/cxxtools/bin/include/CxxTools/cmd.hpp;/home/deep/cxxtools/bin/include/CxxTools/misc.hpp;/home/deep/cxxtools/bin/include/CxxTools/parser.hpp;/home/deep/cxxtools/bin/include/CxxTools/progressBar.hpp;/home/deep/cxxtools/bin/include/CxxTools/regionalpointdescriptor.hpp;/home/deep/cxxtools/bin/include/CxxTools/spinhistogram.hpp;/home/deep/cxxtools/bin/include/CxxTools/thread_pool.hpp;/home/deep/cxxtools/bin/include/CxxTools/weightedunitvoxel.hpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/deep/cxxtools/bin/include/CxxTools" TYPE FILE FILES
    "/home/deep/cxxtools/include/AwaitingThread.hpp"
    "/home/deep/cxxtools/include/HDF5Reader.hpp"
    "/home/deep/cxxtools/include/HDF5Saver.hpp"
    "/home/deep/cxxtools/include/HDF5Tools.hpp"
    "/home/deep/cxxtools/include/LogUtil.hpp"
    "/home/deep/cxxtools/include/MemoryBlock.hpp"
    "/home/deep/cxxtools/include/PathTool.hpp"
    "/home/deep/cxxtools/include/RotInvConvertor.hpp"
    "/home/deep/cxxtools/include/Tools.hpp"
    "/home/deep/cxxtools/include/cmd.hpp"
    "/home/deep/cxxtools/include/misc.hpp"
    "/home/deep/cxxtools/include/parser.hpp"
    "/home/deep/cxxtools/include/progressBar.hpp"
    "/home/deep/cxxtools/include/regionalpointdescriptor.hpp"
    "/home/deep/cxxtools/include/spinhistogram.hpp"
    "/home/deep/cxxtools/include/thread_pool.hpp"
    "/home/deep/cxxtools/include/weightedunitvoxel.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/deep/cxxtools/bin/cmake/CxxToolsConfig.cmake;/home/deep/cxxtools/bin/cmake/CxxToolsConfigVersion.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/deep/cxxtools/bin/cmake" TYPE FILE FILES
    "/home/deep/cxxtools/cmake-build-debug/CMakeFiles/CxxToolsConfig.cmake"
    "/home/deep/cxxtools/cmake-build-debug/CxxToolsConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/deep/cxxtools/bin/cmake/CxxToolsTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}/home/deep/cxxtools/bin/cmake/CxxToolsTargets.cmake"
         "/home/deep/cxxtools/cmake-build-debug/CMakeFiles/Export/_home/deep/cxxtools/bin/cmake/CxxToolsTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}/home/deep/cxxtools/bin/cmake/CxxToolsTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}/home/deep/cxxtools/bin/cmake/CxxToolsTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/deep/cxxtools/bin/cmake/CxxToolsTargets.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/deep/cxxtools/bin/cmake" TYPE FILE FILES "/home/deep/cxxtools/cmake-build-debug/CMakeFiles/Export/_home/deep/cxxtools/bin/cmake/CxxToolsTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "/home/deep/cxxtools/bin/cmake/CxxToolsTargets-debug.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "/home/deep/cxxtools/bin/cmake" TYPE FILE FILES "/home/deep/cxxtools/cmake-build-debug/CMakeFiles/Export/_home/deep/cxxtools/bin/cmake/CxxToolsTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/deep/cxxtools/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
