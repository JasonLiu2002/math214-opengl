#version 330 core

layout(location = 0) in vec3 vertex;

out vec3 fragmentPositions;

uniform mat4 model, view, projection;

void main() {
    fragmentPositions = vec3(model * vec4(vertex, 1.0));
    gl_Position = projection * view * model * vec4(vertex, 1.0);
}
