FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/devices)

file(GLOB RIV_SRC
    "*.cpp"
    "Data/*.cpp"
    "Geometry/*.cpp"
    "Graphics/*.cpp"
    "Octree/*.cpp"
    "Views/*.cpp"
)	

ADD_EXECUTABLE(riv_main ${RIV_SRC})

TARGET_LINK_LIBRARIES(riv_main device_singleray ${OPENGL_LIBRARY} ${GLUT_LIBRARY})