#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

out vec3 fragmentPosition, transformedNormal;

uniform mat4 model, view, projection;

void main() {
    fragmentPosition = vec3(model * vec4(vertex, 1.0));
    transformedNormal = mat3(transpose(inverse(model))) * normal;
    gl_Position = projection * view * model * vec4(vertex, 1.0);
}