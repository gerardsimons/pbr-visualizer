FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
SET(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
FIND_PACKAGE(Qt5Widgets)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/devices)

set(CMAKE_AUTOMOC ON)

file(GLOB RIV_SRC
    "*.cpp"
    "Data/*.cpp"
    "Geometry/*.cpp"
    "Graphics/*.cpp"
    "Octree/*.cpp"
    "Views/*.cpp"

    "*.h"
    "Data/*.h"
    "Geometry/*.h"
    "Graphics/*.h"
    "Octree/*.h"
    "Views/*.h"
)	

ADD_EXECUTABLE(riv_main ${RIV_SRC})

qt5_use_modules(riv_main Widgets OpenGL)

TARGET_LINK_LIBRARIES(riv_main device_singleray ${OPENGL_LIBRARY} ${GLUT_LIBRARY})