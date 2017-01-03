# qsculpt
3D modeling application aimed to use 3D sculpting modeling techniques.

Currently, this application is being used as a test bed for learning techniques to create this kind of applications. 

Build instructions
====

The sources have only been tested under MacOS 10.10. Library dependencies can be obtained from Brew repository which includes all the dependencies needed.

Library dependencies:
- Boost
- Qt 4.8
- CMake
- SDL2
- Eigen
- Assimp

Windows build
====

* Requires Visual Studio 2015
* Install Boost
  * Boost library's binaries should be installed in %BOOST_ROOT%/lib for 32 bit libs and/or %BOOST_ROOT/lib64 for 64 bit binaries
  * Set BOOST_ROOT environment variable accordingly 
* Install SDL2
* Install SDL2_Image
* Install Assimp
* Install CMake
* Install Qt

1. Download source files and unpack them
2. Create a build directory
  - cd <qsculpt source>
  - mkdir build
3. Run CMake
  - cd build
  - cmake -G "Visual Studio 2015" ..
4. Open generated qsculpt.sln in Visual Studio

macOS build
===
TODO

Linux build
===
TODO
