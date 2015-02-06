cmake_minimum_required (VERSION 2.8)
CMAKE_POLICY(SET CMP0015 NEW)

# Name of the project
project (Twilight)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})


IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# For Clang, set GNU++11
	LIST(APPEND CMAKE_CXX_FLAGS "-std=gnu++11 -stdlib=libc++")
ELSE ()
	LIST(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
ENDIF()

# Turn on the ability to create folders to organize projects (.vcproj)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Sub-directories where more CMakeLists.txt exist
add_subdirectory(src)