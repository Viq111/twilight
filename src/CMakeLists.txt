file (GLOB SOURCES
      "*.h"
      "*.cpp")

# Properties->C/C++->General->Additional Include Directories
include_directories(.)

# Set Properties->General->Configuration Type to Application(.exe)
# Adds sources to the Solution Explorer
add_executable(twilight ${SOURCES})

# Properties->Linker->Input->Additional Dependencies
target_link_libraries(src)

# Creates a folder "executables" and adds target 
# project (app.vcproj) under it
set_property(TARGET twilight PROPERTY FOLDER "executables")

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
# install(TARGETS app
#          RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)