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

//void printMat4(glm::mat4 &mat) {
//    for(int i = 0; i < 4; ++i) {
//        for(int j = 0; j < 4; ++j) {
//            std::cout << mat[i][j] << "\t";
//        }
//        std::cout << "\n";
//    }
//}

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
    glm::vec3 direction = glm::normalize(target - position);
    up = glm::normalize(up);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    up = cross(right, direction);
    // Instead of doing the matrix multiplication, manually calculate for speed;
    glm::mat4 view(1.0f);
    view[0][0] = right.x;
    view[1][0] = right.y;
    view[2][0] = right.z;
    view[0][1] = up.x;
    view[1][1] = up.y;
    view[2][1] = up.z;
    view[0][2] =-direction.x;
    view[1][2] =-direction.y;
    view[2][2] =-direction.z;
    view[3][0] =-glm::dot(right, position);
    view[3][1] =-glm::dot(up, position);
    view[3][2] = glm::dot(direction, position);
    return view;
}

// TODO: Implement transformations on the model
glm::mat4 calculateModelMatrix() {
    return glm::mat4(1.0);
}

void loadObj(fs::path const& path, std::vector<glm::vec3>& out_vertices, std::vector<glm::uvec3>& out_faces) {
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
            unsigned int v1, v2, v3;
            file >> v1 >> v2 >> v3;
            out_faces.emplace_back(v1 - 1, v2 - 1, v3 - 1);
        }
    }
}

int main(int argc, char** argv) {
    try {
        const static int WIDTH = 1024;
        const static int HEIGHT = 768;

        // Initializing GLFW
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // Create a 1024x768 window
        GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Math 214 OpenGL", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to open GLFW window");
        }
        glfwMakeContextCurrent(window);

        // Initializing GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            throw std::runtime_error("Failed to initialize GLEW");
        }

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwPollEvents();
        glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

        // Loading the Utah teapot
        std::vector<glm::vec3> vertices;
        std::vector<glm::uvec3> faces;
        loadObj("teapot.obj", vertices, faces);

        GLuint vertexShaderHandle = compileShader("vertex.glsl", GL_VERTEX_SHADER);
        GLuint fragmentShaderHandle = compileShader("fragment.glsl", GL_FRAGMENT_SHADER);
        GLuint geometryShaderHandle = compileShader("geometry.glsl", GL_GEOMETRY_SHADER);
        GLuint programHandle = linkShaders({vertexShaderHandle, geometryShaderHandle, fragmentShaderHandle});

        GLuint modelHandle = glGetUniformLocation(programHandle, "model");
        GLuint viewHandle = glGetUniformLocation(programHandle, "view");
        GLuint projectionHandle = glGetUniformLocation(programHandle, "projection");
        GLuint lightPositionHandle = glGetUniformLocation(programHandle, "lightPosition");
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

        float aspect = static_cast<float>(WIDTH) / HEIGHT;
        glm::vec3 cameraPosition{0.0f, 0.0f, 10.0f};
        glm::mat4 model = calculateModelMatrix();
        glm::mat4 view = calculateViewMatrix(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        glm::mat4 projection = calculateProjectionMatrix(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        while (!glfwWindowShouldClose(window)) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                break;
            }
            glClearColor(0.1765f, 0.1647f, 0.1804f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_LESS);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::vec3 cameraPosition{10.0f, 1.0f, 10.0f}, target{0.0f, 0.0f, 0.0f};
            glm::mat4 model = calculateModelMatrix();
            glm::mat4 view = calculateViewMatrix(cameraPosition, target, {0.0f, 1.0f, 0.0f});
            float aspect = static_cast<float>(WIDTH) / HEIGHT;
            glm::mat4 projection = calculateProjectionMatrix(glm::radians(45.0f), aspect, 0.1f, 100.0f);

            glUseProgram(programHandle);
            glUniformMatrix4fv(modelHandle, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewHandle, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, glm::value_ptr(projection));

            glUniform3fv(lightPositionHandle, 1, glm::value_ptr(cameraPosition));
            glm::vec3 viewPosition{0.0f, 0.0f, 5.0f};
            glUniform3fv(viewPositionHandle, 1, glm::value_ptr(viewPosition));
            glm::vec3 lightColor{1.0f, 1.0f, 1.0f};
            glUniform3fv(lightColorHandle, 1, glm::value_ptr(lightColor));
            glm::vec3 objectColor{1.0f, 0.2f, 0.2f};
            glUniform3fv(objectColorHandle, 1, glm::value_ptr(objectColor));

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);

//            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
            glDrawElements(GL_TRIANGLES, faces.size() * 12, GL_UNSIGNED_INT, nullptr);

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
