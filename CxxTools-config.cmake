get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/CxxTools-targets.cmake)
get_filename_component(CXXTOOLS_INCLUDE_DIRS "${SELF_DIR}/include/cxxtools" ABSOLUTE)
