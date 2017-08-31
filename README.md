# PBR Visualizer

This is the source code for the application used in the publication "An Interactive Information VIsualiszation Approach to Physically-based Rendering" to visualise the physically-based rendering process. See [here](https://graphics.tudelft.nl/Publications-new/2016/SAHDEE16/) for the paper. 

## Overview

The application was intended as a real-time visualisation front-end of a physically-based rendering process. 

An adapted and simplified [Embree](https://embree.github.io/) renderer is included which will render an according to an Embree scene description file and visualise a sample of the data associated with its light paths, individual rays and intersection points. Two different scenes may be run similtaneously, offering a comparative visualisation useful for detecting differences in scene properties or rendering settings.

Visualisations include an advanced interactive parallel coordinates plot, a preview of the renderer, a 3D view of the scene geometry and ray tracing animation. 

## Prerequisites 

* UNIX system (tested on Ubuntu 17.04 and MacOS)
* OpenGL & GLUT
* EMBREE Rendering Kernels [[LINK](https://embree.github.io/downloads.html)]
* C++14 compiler (tested with GCC)
* CMake to generate build files

## Building

Create a destination build dir, and use CMake to configure and generate build files. I recommend using cmake-gui to easily check and verify any missing dependencies. Some things to note :

* Make sure you set the BUILD_SINGLE_RAY_DEVICE option)
* Set CMAKE_CXX_COMPILER to `/usr/bin/g++`
* Set the EMBREE_INCLUDE_PATH to your `<embree_install_path>/include`
* Set the EMBREE_LIBRARY to your `<embree_install_path>/lib/libembree.so`
* It should be able to resolve the GLUT and GL dependendecies on its own. They should point to `libGLU.so` and `libGL.so` files.

Look at any subsequent errors and try and resolve any other required dependencies or errors (Feel free to ask me for help!)

When the generation is sucessful, your build directory should be populated with many things, amongst which there should be a `Makefile`. Go the directory and run `make -j4` (change the 4 to the number of cores you have available)

_Any problems? Contact me!_