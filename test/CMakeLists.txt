cmake_minimum_required(VERSION 3.14)

project(TESTS CXX)

## TEST THREAD UNIT
SET(targetname test)
FIND_PACKAGE(GTest)
IF(GTEST_INCLUDE_DIR)
	find_package( Threads )
	SET(sources test.cpp)
	SET(headers)
	SET(templates)
	ADD_EXECUTABLE(${targetname} ${sources} ${headers} ${templates})
	TARGET_INCLUDE_DIRECTORIES(${targetname} PUBLIC ${GTEST_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(${targetname} PUBLIC ${GTEST_LIBRARY} PUBLIC ${GTEST_MAIN_LIBRARY})
	target_link_libraries(${targetname}
			PUBLIC CxxTools
			PUBLIC pthread
	)
ENDIF()
