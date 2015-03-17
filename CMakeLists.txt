cmake_minimum_required (VERSION 2.8)
CMAKE_POLICY(SET CMP0015 NEW)

# Name of the project
project (controller)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# Turn on the ability to create folders to organize projects (.vcproj)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Sub-directories where more CMakeLists.txt exist
add_subdirectory(controller)