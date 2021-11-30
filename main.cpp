#include <GLUT/glut.h>
#include <iostream>

static int WINDOW_ID;

static void RenderSceneCB() {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
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
