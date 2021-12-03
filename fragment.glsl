#version 330 core

in vec3 fragmentPosition, transformedNormal;

out vec4 fragmentColor;

uniform vec3 lightPosition, viewPosition, lightColor, objectColor;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    // Diffuse
    vec3 normal = normalize(transformedNormal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    // Combination
    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragmentColor = vec4(result, 1.0);
}