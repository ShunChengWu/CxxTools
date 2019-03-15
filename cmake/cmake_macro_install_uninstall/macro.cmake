####################################
# This macro is used for offer "make install" && "make uninstall" for your target.
# Before calling this macro, you should have declared the INSTALL for your target.
# e.g: 
#INSTALL(TARGETS  NAME 
#	EXPORT TARGETNAME
#	RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT bin
#	ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
#	LIBRARY DESTINATION  ${INSTALL_LIB_DIR} COMPONENT lib
#	INCLUDES DESTINATION ${EIGEN3_INCLUDE_DIR} ${INSTALL_INCLUDE_DIR}/../
#	PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT include
#)
# 
# And make sure the following files exist in your ${PROJECT_SOURCE_DIR}/cmake
# cmake_uninstall.cmake.in  InstallConfig.cmake.in  InstallConfigVersion.cmake.in
#
# VARIABELS
# NAME - the name of your target
# TARGETNAME - The name of your target file ( usually ${NAME}Target )
# VERSION - the version number. should be M.N.P (Major.Minor.Patch)
# INSTALL_LIB_DIR - the location where libary files should go to
# INSTALL_BIN_DIR - the location where binary files should go to
# INSTALL_INCLUDE_DIR - the location where include files should go to
# INSTALL_CMAKE_DIR - the location where cmake files should go to
##################

macro(setInstall NAME TARGETNAME VERSION INSTALL_LIB_DIR INSTALL_BIN_DIR INSTALL_INCLUDE_DIR INSTALL_CMAKE_DIR)

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    #message("var: " ${var})
  endif()
endforeach()

# Create the ${NAME}Config.cmake and ${NAME}ConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")

set(INSTALL_TARGET_NAME ${NAME})
set(INSTALL_VERSION ${VERSION})
set(INSTALL_TARGETFILE_NAME ${TARGETNAME})

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(${PROJECT_SOURCE_DIR}/cmake/cmake_macro_install_uninstall/InstallConfig.cmake.in 
	"${PROJECT_BINARY_DIR}/${NAME}Config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(${PROJECT_SOURCE_DIR}/cmake/cmake_macro_install_uninstall/InstallConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${NAME}Config.cmake" @ONLY)

# ... for both
configure_file(${PROJECT_SOURCE_DIR}/cmake/cmake_macro_install_uninstall/InstallConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/${NAME}ConfigVersion.cmake" @ONLY)

# Install the ${NAME}Config.cmake and ${NAME}ConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${NAME}ConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT ${INSTALL_TARGETFILE_NAME} DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)


#######################
# Uninstall target, for "make uninstall"
#######################
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/cmake/cmake_macro_install_uninstall/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


endmacro(setInstall)



macro(setVersion VERSION major minor patch)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(PATCH_VERSION 1)
set(${VERSION} ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
endmacro(setVersion)