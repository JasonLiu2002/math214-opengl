#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

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

int main(int argc, char** argv) {
    const static int WIDTH = 1024;
    const static int HEIGHT = 768;

    // Initializing GLFW
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a 1024x768 window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Math 214 OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // Initializing GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return 1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

    // Loading the Utah teapot
    std::vector<glm::vec3> outVertices;
    std::vector<glm::ivec3> outFaces;
    loadObj("teapot.obj", outVertices, outFaces);

//    GLuint vertexShaderHandle = compileShader("vertex.glsl", GL_VERTEX_SHADER);
//    GLuint fragmentShaderHandle = compileShader("fragment.glsl", GL_FRAGMENT_SHADER);
//    GLuint programHandle = linkShaders(vertexShaderHandle, fragmentShaderHandle);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, outVertices.size() * sizeof(glm::vec3), &outVertices[0], GL_STATIC_DRAW);

    GLuint faceBuffer;
    glGenBuffers(1, &faceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, faceBuffer);
    glBufferData(GL_ARRAY_BUFFER, outFaces.size() * sizeof(glm::vec3), &outFaces[0], GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            break;
        }
        // Check for mouse/key movements
        glfwPollEvents();
        // Clear color buffer
        glClearColor(0.1765f, 0.1647f, 0.1804f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate MVP from inputs



        // Draw
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
