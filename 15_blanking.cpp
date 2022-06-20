#include <windows.h>
#include <GL/glut.h>
#include<stdio.h>
#include <stdlib.h>

GLfloat pos[] = {-2, 4, 5, 1},
        amb[] = {0.3, 0.3, 0.3, 1.0};
GLfloat front_amb_diff[] = {0.7, 0.5, 1.1, 1.0};
GLfloat back_amb_diff[] = {0.4, 0.7, 1.1, 1.0};
GLfloat spe[] = {0.25, 0.25, 0.25, 1.0};
GLfloat theta = 0, dt = 1, axes[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
int axis = 0;

void display(){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    if(axis<3){
        glRotated(theta,axes[axis][0],axes[axis][1],axes[axis][2]);
    }else{
        glPushMatrix();
        glRotated(theta, 0, 0, 1);
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glPopMatrix();
    }
    glutSolidTorus(.4, 1, 48, 96);
    //glutSolidSphere(1, 48, 96);
    //glutSolidTeapot(1);
    //glutSolidIcosahedron();
    //glutSolidDodecahedron();
    glPopMatrix();
    glutSwapBuffers();
}

void idle(){
    theta+=0.1;
    if(theta>=360.0){
        theta-=360.0;
        axis = (axis + 1) % 4;
    }
    glutPostRedisplay();
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Torus");
    glClearColor(1,1,0,0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1.0, 2, 8);
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,front_amb_diff);
    glMaterialfv(GL_BACK,GL_AMBIENT_AND_DIFFUSE,back_amb_diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 75);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0,0,-5);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}