# Installation

## Installing Boost library

Download Boost source code. As far as you can, take the 1.57.0 version of Boost library. It's the one we'll use to code.

Unzip it in a folder whose path doesn't contain any accent (otherwise, it can't compile correctly). Open a command line and get in Boost folder.

    bootstrap
    b2 -j4 --toolset=msvc-12.0 address-model=64 --build-type=complete --with-chrono --with-iostreams --with-thread --with-date_time --prefix=build -q install

Then we have to change some few things in build folder. The build folder contains two directories : `include` and `lib`. You should have this structure :

```
├── boost_1_57_0
│   ├── ...
|   ├── ...
│   ├── build
│   │   ├── include
│   │   │   ├── boost-1_57
│   │   │   │   ├── boost
│   │   │   │   │   ├── ...
│   │   │   │   │   ├── ...
│   │   │   │   │   ├── version.hpp
│   │   │   │   │   ├── ...
│   │   ├── lib
│   ├── ...
```

You need to change the structure in order to obtain this one :

```
├── boost_1_57_0
│   ├── ...
|   ├── ...
│   ├── build
│   │   ├── boost
│   │   |   ├── ...
│   │   |   ├── ...
│   │   │   ├── version.hpp
│   │   │   ├── ...
│   │   ├── lib
│   ├── ...
```

Then, an environment variable `BOOST_ROOT` needs to be created whose value is `/path/to/boost_1_57_0/build`. You can even delete `boost_1_57_0` folder until you keep `BOOST_ROOT` pointing at your build directory.

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
