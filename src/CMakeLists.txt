# Find Boost packages
find_package( Boost COMPONENTS chrono iostreams thread )

if(Boost_FOUND)

    # Link Boost to project
    link_directories( ${Boost_LIBRARY_DIRS} )
    include_directories( ${Boost_INCLUDE_DIRS} )

    # Source files
    file (GLOB SOURCES
          "*.h"
          "*.cpp")

    # Executable name
    SET (EXECUTABLE_NAME twilight)

    # Properties->C/C++->General->Additional Include Directories
    include_directories(.)

    # Set Properties->General->Configuration Type to Application(.exe)
    # Adds sources to the Solution Explorer
    add_executable(twilight ${SOURCES})

    # Properties->Linker->Input->Additional Dependencies
    target_link_libraries(
	   ${EXECUTABLE_NAME}
        ${Boost_LIBRARIES}
    )

    # Creates a folder "executables" and adds target 
    # project (app.vcproj) under it
    set_property(TARGET twilight PROPERTY FOLDER "executables")
endif()