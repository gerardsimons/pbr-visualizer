# PBR Visualizer

This is the source code for the application used in the publication "An Interactive Information VIsualiszation Approach to Physically-based Rendering" to visualise the physically-based rendering process. See [here](https://graphics.tudelft.nl/Publications-new/2016/SAHDEE16/) for the paper. 

## Overview

The application was intended as a real-time visualisation front-end of a physically-based rendering process. 

An adapted and simplified [Embree](https://embree.github.io/) renderer is included which will render an according to an Embree scene description file and visualise a sample of the data associated with its light paths, individual rays and intersection points. Two different scenes may be run similtaneously, offering a comparative visualisation useful for detecting differences in scene properties or rendering settings.

Visualisations include an advanced interactive parallel coordinates plot, a preview

The application was written in C++ on MacOS using XCode with a Clang C++11 compiler. No attempt has been made to migrate to Linux or Windows C++ compilers. In principle a Linux running a Clang C++11 compiler should be similar.

## Prerequisites 

* OpenGL
* EMBREE Rendering Kernels [[LINK](https://embree.github.io/downloads.html)]
* Clang C++11 compiler
* CMake to generate build files (optional)

