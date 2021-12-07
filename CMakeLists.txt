cmake_minimum_required(VERSION 3.20)
project(math214_opengl)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_FIND_FRAMEWORK Last)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
# Hack for now
include_directories(/opt/homebrew/include /usr/local)

add_executable(math214_opengl main.cpp)

target_link_libraries(math214_opengl ${OPENGL_LIBRARIES} glfw ${GLEW_LIBRARIES} ${GLM_LIBRARIES})
