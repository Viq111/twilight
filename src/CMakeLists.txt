# Use static Boost libraries to link
set(Boost_USE_STATIC_LIBS  ON)

# Find Boost packages
find_package( Boost COMPONENTS chrono iostreams system thread date_time )

# Find OpenCL packages
find_package( OpenCL REQUIRED )

if(Boost_FOUND AND OpenCL_FOUND)

    # Link Boost to project
    link_directories( ${Boost_LIBRARY_DIRS} )
    include_directories( ${Boost_INCLUDE_DIRS} )

    # Link OpenCL to project
    include_directories( ${OpenCL_INCLUDE_DIRS} )

    # Source files
    file (GLOB SOURCES
          "*.h"
          "*.cpp")

    # Executable name
    SET (EXECUTABLE_NAME twilight)

    # Set Properties->General->Configuration Type to Application(.exe)
    # Adds sources to the Solution Explorer
    add_executable(twilight ${SOURCES})

    # Properties->Linker->Input->Additional Dependencies
    target_link_libraries(
        ${EXECUTABLE_NAME}
        ${Boost_LIBRARIES}
        ${OpenCL_LIBRARIES}
    )

    # Creates a folder "executables" and adds target 
    # project (app.vcproj) under it
    set_property(TARGET twilight PROPERTY FOLDER "executables")
else()

    if (NOT Boost_FOUND)
	message( FATAL_ERROR "Boost library not found !" )
    endif()

    if (NOT OpenCL_FOUND)
        message( FATAL_ERROR "OpenCL library not found !" )
    endif()

endif()
