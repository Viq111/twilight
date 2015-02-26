# Installation

## Installing Boost library

Download Boost source code. As far as you can, take the 1.57.0 version of Boost library. It's the one we'll use to code.

Unzip it in a folder whose path doesn't contain any accent (otherwise, it can't compile correctly). Open a command line and get in Boost folder.

    cd tools/build
    bootstrap
    b2 install --prefix=/path/to/boost_1_57_0

Add `/path/to/boost_1_57_0/bin` to the `PATH` environment variable. Then, go back in Boost folder.

    b2 -j4 --toolset=msvc --build-type=complete --with-chrono --with-iostreams --with-thread --with-date_time stage

Then, an environment variable `BOOST_ROOT` needs to be created whose value is `/path/to/boost_1_57_0`.

## Installing CMake

We use CMake for building Twilight. We can generate Makefiles and Visual Studio project files easily with it. It's also easier to include a new library (such like Boost, for example :p)

First, install CMake.

Secondly, one can create Visual Studio project files, either by the command line our by the GUI

#### By the command line

Create a `build` directory in your project folder

    mkdir build

Create Visual Studio project files

    cd build
    cmake ..

#### By the GUI

Create a `build` directory in your project folder and start CMake GUI

Browse source and choose the folder of Twilight project. Browse build and choose the `build` folder in Twilight project.

Then click on Configure and choose your compiler. It should be `Visual Studio 12 2013 Win64`. If everything went well, then click on Generate to create project files.

## Let's code ...

In both cases, you should find a `.sln` file in the `build` directory. You can open it with Visual Studio.

And let's code ... :p
