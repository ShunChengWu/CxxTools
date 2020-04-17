####################################
# Set is an script which provide easy installation & uninstallation options for the 
# given target. To use this script the following variables must be set:
#  targetname - the name of your target 
#  headers - the headers where this target required
#  ${targetname}_VERSION - the version number. Must be major.minor.patch. you can use
#                          the macro setVersion in 
#			   cmake_macro_install_uninstall/macro.cmake to set
#  
# This script set the default output as:
# if CMAKE_INSTALL_PREFIX not set: 
#   PREFIX = ${PROJECT_SOURCE_DIR}/bin
# else:
#   PREFIX = ${CMAKE_INSTALL_PREFIX}
# INSTALL_LIB_DIR = ${PREFIX}/lib
# INSTALL_BIN_DIR = ${PREFIX}/bin
# INSTALL_INCLUDE_DIR = ${PREFIX}/include
# INSTALL_CMAKE_DIR = ${PREFIX}/cmake
# 
# You can change them by passing -DINSTALL_CMAKE_DIR=/your/path
#
# Please make sure the cmake_macro_install_uninstall folder is in the same folder as
# this file
#
####################################

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/cmake_macro_install_uninstall/macro.cmake)

#####
# Install parameters
#####

# change CMAKE_INSTALL_PREFIX default setting
SET(OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    #message("Change CMAKE_INSTALL_PREFIX to " ${OUTPUT_PATH})
    set (CMAKE_INSTALL_PREFIX ${OUTPUT_PATH} CACHE PATH "default install path" FORCE )
endif()

# set output variables
SET(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
SET(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
SET(INSTALL_INCLUDE_DIR include/${targetname} CACHE PATH "Installation directory for header files")
SET(INSTALL_CMAKE_DIR CACHE PATH "Installation directory for cmake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

#####
# INSTALL 
#####
# set PUBLIC_HEADERS
SET(PUBLIC_HEADERS)
FOREACH(var ${headers})
  SET(PUBLIC_HEADERS "${PUBLIC_HEADERS}" "${var}")
ENDFOREACH(var ${headers})
SET_TARGET_PROPERTIES(${targetname} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
UNSET(PUBLIC_HEADERS)

get_target_property(${targetname}_INCLUDE_DIRS ${targetname} INTERFACE_INCLUDE_DIRECTORIES)

IF(NOT ${targetname}_INCLUDE_DIRS)
	set(${targetname}_INCLUDE_DIRS "")
ENDIF(NOT ${targetname}_INCLUDE_DIRS)

#message("INSTALL_BIN_DIR: " ${INSTALL_BIN_DIR})
#message("INSTALL_LIB_DIR: " ${INSTALL_LIB_DIR})
#message("INSTALL_INCLUDE_DIR: " ${INSTALL_INCLUDE_DIR})
#message("INSTALL_CMAKE_DIR: " ${INSTALL_CMAKE_DIR})
#message("${targetname}_INCLUDE_DIRS: " ${${targetname}_INCLUDE_DIRS})

INSTALL(TARGETS  ${targetname} 
	EXPORT ${targetname}Targets
	RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT bin
	ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
	LIBRARY DESTINATION  ${INSTALL_LIB_DIR} COMPONENT lib
	INCLUDES DESTINATION ${${targetname}_INCLUDE_DIRS} include  #${OUTPUT_PATH}/include
	PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT include# ${OUTPUT_PATH} include
)

setInstall(${targetname} ${targetname}Targets ${${targetname}_VERSION} ${INSTALL_LIB_DIR} ${INSTALL_BIN_DIR} ${INSTALL_INCLUDE_DIR} ${INSTALL_CMAKE_DIR})