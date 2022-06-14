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

void quadSurfs()
{
    glClear(GL_COLOR_BUFFER_BIT);
    showAxis();
    glColor3f(0.0,0.0,1.0);
    gluLookAt(2.0,2.0,2.0,0.0,0.0,0.0,0.0,0.0,1.0);
    glPushMatrix();
    glTranslatef(1.0,1.0,0.0);
    glutWireSphere(0.75,8,6);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0,-0.5,0.5);
    glutWireCone(0.7,2.0,7,6);
    glPopMatrix();

    GLUquadricObj *cylinder;
    glPushMatrix();
    glTranslatef(-0.2,1.2,1);
    cylinder=gluNewQuadric();
    gluQuadricDrawStyle(cylinder,GLU_LINE);
    gluCylinder(cylinder,0.6,0.6,1.5,6,4);
    glPopMatrix();

    GLUquadricObj *disk;
    glPushMatrix();
    glTranslatef(-0.2,1.2,1);
    disk=gluNewQuadric();
    gluQuadricDrawStyle(disk,GLU_LINE);
    gluDisk(disk,0,1,50,20);
    glPopMatrix();

    glFlush();
}

void winReshapeFcn(GLint newWidth,GLint newHeight)
{
    glViewport(0,0,newWidth,newHeight);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-2.0,2.0,-2.0,2.0,0.0,5.0);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(450,300);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("Quadric Surfaces");
    init();
    glutDisplayFunc(quadSurfs);
    glutReshapeFunc(winReshapeFcn);
    glutMainLoop();
    return 0;
}