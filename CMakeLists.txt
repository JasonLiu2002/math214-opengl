cmake_minimum_required(VERSION 3.20)
project(math214_opengl)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories(SYSTEM ${GLUT_INCLUDE_DIRS})
include_directories(SYSTEM ${GLEW_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})

add_executable(math214_opengl main.cpp)

target_link_libraries(math214_opengl ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
