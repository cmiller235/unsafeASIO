cmake_minimum_required(VERSION 3.22.1)
project(unsafe)
# Group together source and header files for primary executable
file(GLOB_RECURSE MY_SOURCES CONFIGURED_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} "${MY_SOURCES}")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Find packages needed for primary executable
find_package(Boost 1.74 REQUIRED)
find_package(glog 0.4.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost glog::glog)

set(TEST_BINARY ${PROJECT_NAME}Test)
# Group together source and header files for test executable
file(GLOB_RECURSE TEST_SOURCES CONFIGURED_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")
add_executable(${TEST_BINARY} "${TEST_SOURCES}")

# Find packages needed for test executable
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
target_include_directories(${TEST_BINARY} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_CURRENT_SOURCE_DIR}/test/")
target_link_libraries(${TEST_BINARY} PUBLIC ${GTEST_LIBRARIES} pthread Boost::boost glog::glog)
