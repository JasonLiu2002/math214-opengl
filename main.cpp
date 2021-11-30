#include <GLUT/glut.h>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>

static int WINDOW_ID;

void load_obj(std::string const& path, std::vector<glm::vec3>& out_vertices, std::vector<glm::ivec3>& out_faces) {
    std::ifstream file;
    file.open(path);
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

static void RenderSceneCB() {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    std::vector<glm::vec3> out_vertices;
    std::vector<glm::ivec3> out_faces;
    load_obj("teapot.obj", out_vertices, out_faces);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(960, 514);
    glutInitWindowPosition(200, 200);
    WINDOW_ID = glutCreateWindow("Math 214 OpenGL");
    glutDisplayFunc(RenderSceneCB);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glutMainLoop();
    return 0;
}
