#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>
#include <GLUT/glut.h>

namespace fs = std::filesystem;

GLuint linkShaders(GLuint vertexShaderHandle, GLuint fragmentShaderHandle) {
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShaderHandle);
    glAttachShader(programHandle, fragmentShaderHandle);
    glLinkProgram(programHandle);
    GLint success;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success) {
        std::array<char, 512> infoLog{};
        glGetProgramInfoLog(programHandle, infoLog.size(), nullptr, infoLog.data());
        std::cout << "Shader linking failed:\n" << infoLog.data() << std::endl;
    }
    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
    return programHandle;
}

GLuint compileShader(fs::path const& path, GLenum shaderType) {
    std::ifstream file(path);
    std::stringstream stream;
    stream << file.rdbuf();
    std::string shaderSource = stream.str();
    GLuint shaderHandle = glCreateShader(shaderType);
    GLchar const* shaderSourcePtr = shaderSource.c_str();
    glShaderSource(shaderHandle, 1, &shaderSourcePtr, nullptr);
    glCompileShader(shaderHandle);
    GLint success;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::array<char, 512> infoLog{};
        glGetShaderInfoLog(shaderHandle, infoLog.size(), nullptr, infoLog.data());
        std::cerr << "Shader compilation failed:\n" << infoLog.data() << std::endl;
    }
    return shaderHandle;
}

void loadObj(fs::path const& path, std::vector<glm::vec3>& out_vertices, std::vector<glm::ivec3>& out_faces) {
    std::ifstream file(path);
    char type;
    while (file >> type) {
        if (type == 'v') {
            float x, y, z;
            file >> x >> y >> z;
            out_vertices.emplace_back(x, y, z);
        } else if (type == 'f') {
            int v1, v2, v3;
            file >> v1 >> v2 >> v3;
            out_faces.emplace_back(v1, v2, v3);
        }
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(960, 514);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Math 214 OpenGL");

    std::vector<glm::vec3> out_vertices;
    std::vector<glm::ivec3> out_faces;
    loadObj("teapot.obj", out_vertices, out_faces);

    GLuint vertexShaderHandle = compileShader("vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShaderHandle = compileShader("fragment.glsl", GL_FRAGMENT_SHADER);
    GLuint programHandle = linkShaders(vertexShaderHandle, fragmentShaderHandle);

    glutDisplayFunc(renderScene);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glutMainLoop();
    return 0;
}
