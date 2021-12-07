#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

GLuint linkShaders(std::vector<GLuint> const& shaderHandles) {
    GLuint programHandle = glCreateProgram();
    for (GLuint handle: shaderHandles)
        glAttachShader(programHandle, handle);
    glLinkProgram(programHandle);
    GLint success;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success) {
        std::array<char, 512> infoLog{};
        glGetProgramInfoLog(programHandle, infoLog.size(), nullptr, infoLog.data());
        std::cout << "Shader linking failed:\n" << infoLog.data() << std::endl;
    }
    for (GLuint handle: shaderHandles)
        glDeleteShader(handle);
    return programHandle;
}

GLuint compileShader(fs::path const& path, GLenum shaderType) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to read file");
    }
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
        throw std::runtime_error("Shader compilation failed");
    }
    return shaderHandle;
}

glm::mat4 calculateProjectionMatrix(float fovY, float aspect, float zNear, float zFar) {
    float tanHalfFovY = tan(fovY / 2.0f);

    glm::mat4 proj(0.0f);
    proj[0][0] = 1.0f / (aspect * tanHalfFovY);
    proj[1][1] = 1.0f / (tanHalfFovY);
    proj[2][2] = -(zFar + zNear) / (zFar - zNear);
    proj[2][3] = -1.0f;
    proj[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
    return proj;
}

glm::mat4 calculateViewMatrix(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    glm::vec3 direction = target - position;
    glm::vec3 right = glm::cross(direction, up);
    glm::mat4 camera;
    camera[0] = glm::vec4(right, 0.0f);
    camera[1] = glm::vec4(up, 0.0f);
    camera[2] = glm::vec4(direction, 0.0f);
    camera[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    camera = glm::transpose(camera);
    glm::mat4 cameraPos(1.0f);
    cameraPos[3] = glm::vec4(-1.0f * position, 1.0f);
    return camera * cameraPos;
}

// TODO: Implement transformations on the model
glm::mat4 calculateModelMatrix() {
    return glm::mat4(1.0);
}

void loadObj(fs::path const& path, std::vector<glm::vec3>& out_vertices, std::vector<glm::ivec3>& out_faces) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to read file");
    }
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
    try {
        const static int WIDTH = 1024;
        const static int HEIGHT = 768;

        // Initializing GLFW
        if (!glfwInit()) {
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
        glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

        // Loading the Utah teapot
        std::vector<glm::vec3> vertices;
        std::vector<glm::ivec3> faces;
        loadObj("teapot.obj", vertices, faces);

        GLuint vertexShaderHandle = compileShader("vertex.glsl", GL_VERTEX_SHADER);
        GLuint fragmentShaderHandle = compileShader("fragment.glsl", GL_FRAGMENT_SHADER);
        GLuint geometryShaderHandle = compileShader("geometry.glsl", GL_GEOMETRY_SHADER);
        GLuint programHandle = linkShaders({vertexShaderHandle, geometryShaderHandle, fragmentShaderHandle});

        GLuint modelHandle = glGetUniformLocation(programHandle, "model");
        GLuint viewHandle = glGetUniformLocation(programHandle, "view");
        GLuint projectionHandle = glGetUniformLocation(programHandle, "projection");
        [[maybe_unused]] GLuint lightPositionHandle = glGetUniformLocation(programHandle, "lightPosition");
        GLuint viewPositionHandle = glGetUniformLocation(programHandle, "viewPosition");
        GLuint lightColorHandle = glGetUniformLocation(programHandle, "lightColor");
        GLuint objectColorHandle = glGetUniformLocation(programHandle, "objectColor");

        GLuint vertexArrayHandle;
        glGenVertexArrays(1, &vertexArrayHandle);
        glBindVertexArray(vertexArrayHandle);

        GLuint vertexBufferHandle;
        glGenBuffers(1, &vertexBufferHandle);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW);

        GLuint elementBufferHandle;
        glGenBuffers(1, &elementBufferHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(decltype(faces)::value_type), faces.data(), GL_STATIC_DRAW);

        while (!glfwWindowShouldClose(window)) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                break;
            }
            glClearColor(0.1765f, 0.1647f, 0.1804f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 model = calculateModelMatrix();
            glm::mat4 view = calculateViewMatrix(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
            float aspect = static_cast<float>(WIDTH) / HEIGHT;
            glm::mat4 projection = calculateProjectionMatrix(glm::radians(45.0f), aspect, 0.1f, 100.0f);

            view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
            projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

            glUseProgram(programHandle);
            glUniformMatrix4fv(modelHandle, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewHandle, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, glm::value_ptr(projection));

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);

            glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_SHORT, nullptr);

            glDisableVertexAttribArray(0);

            glfwSwapBuffers(window);
            glfwPollEvents(); // Check for mouse/key movements
        }

        glDeleteBuffers(1, &vertexBufferHandle);
        glDeleteBuffers(1, &elementBufferHandle);
        glDeleteProgram(programHandle);
        glDeleteVertexArrays(1, &vertexArrayHandle);

        glfwTerminate();
        return EXIT_SUCCESS;
    } catch (std::exception const& exception) {
        return EXIT_FAILURE;
    }
}
