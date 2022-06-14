#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

GLsizei winWidth=400,winHeight=300;

void init()
{
    glClearColor(1.0,1.0,1.0,0.0);
}

void showAxis()
{
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_LINES);
        glVertex3i(0,0,0);
        glVertex3i(10,0,0);
    glEnd();
    glFlush();
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_LINES);
        glVertex3i(0,0,0);
        glVertex3i(0,10,0);
    glEnd();
    glFlush();
    glColor3f(0.0,1.0,1.0);
    glBegin(GL_LINES);
        glVertex3i(0,0,0);
        glVertex3i(0,0,10);
    glEnd();
    glFlush();
}

void displayPolyhedra()
{
    glClear(GL_COLOR_BUFFER_BIT);
    gluLookAt(5.0,5.0,5.0,0.0,0.0,0.0,0.0,1.0,0.0);
    showAxis();
    glColor3f(0.0,0.0,1.0);
    glutWireCube(1.0);

    glTranslatef(-6.0,-5.0,0.0);
    glutWireDodecahedron();

    glTranslatef(8.6,8.6,2.0);
    glutWireTetrahedron();

    glTranslatef(-3.0,-1.0,0.0);
    glutWireOctahedron();

    glScalef(0.8,0.8,0.8);
    glTranslatef(4.3,-2.0,0.5);
    glutWireIcosahedron();

    glTranslatef(-1.0,0.0,-3.0);
    glRotatef(180,0.0,1.0,0.0);
    glutWireTeapot(1.0);

    glFlush();
}

void keyboardFunc(unsigned char key,int x,int y)
{
    switch(key)
    {
        case 'q':
        case 'Q':
        case 27:
            exit(0);
            break;
    }
}

void winReshapeFcn(GLint newWidth,GLint newHeight)
{
    glViewport(0,0,newWidth,newHeight);
    glMatrixMode(GL_PROJECTION);
    glFrustum(-1.0,1.0,-1.0,1.0,2.0,20.0);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(450,300);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("Polyhedra");
    init();
    glutDisplayFunc(displayPolyhedra);
    glutReshapeFunc(winReshapeFcn);
    glutKeyboardFunc(keyboardFunc);
    glutMainLoop();
    return 0;
}