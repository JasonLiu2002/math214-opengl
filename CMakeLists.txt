cmake_minimum_required(VERSION 3.20)
project(math214_opengl)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_FIND_FRAMEWORK Last)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
# Hack for now
include_directories(/opt/homebrew/include)
# include_directories(${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS}, ${GLM_INCLUDE_DIRS})

add_executable(math214_opengl main.cpp)

target_link_libraries(math214_opengl ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${GLM_LIBRARIES})
