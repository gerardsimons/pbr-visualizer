## ========================================================================  -Wno-reorder##
## Copyright 2009-2013 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

SET(FLAGS_SSSE3 "-mssse3")
SET(FLAGS_SSE41 "-msse4.1")
SET(FLAGS_SSE42 "-msse4.2")
SET(FLAGS_AVX   "-mavx -mvzeroupper")
SET(FLAGS_AVX2  "-mavx2 -mvzeroupper")

SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_C_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-DDEBUG -g -fPIC -fvisibility-inlines-hidden -fvisibility=hidden -stdlib=libc++ -Wno-narrowing -Wno-reorder")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -g -O3 -Wstrict-aliasing=0 -ffast-math ")
SET(CMAKE_EXE_LINKER_FLAGS "")

SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++14")
ADD_DEFINITIONS("-std=c++14") 

IF (APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
ENDIF (APPLE)

SET(EXT "")
