#version 330

layout(triangles) in;
in vec3 fragmentPositions[3];

layout(triangle_strip, max_vertices=3) out;
out vec3 fragmentPosition, normal;

void main()
{
    vec3 a = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
    vec3 b = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
    vec3 N = normalize(cross(b, a));
    for (int i = 0; i < gl_in.length(); ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        fragmentPosition = fragmentPositions[i];
        normal = N;
        EmitVertex();
    }
    EndPrimitive();
}