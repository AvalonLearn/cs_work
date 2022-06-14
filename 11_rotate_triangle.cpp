#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define DEG_TO_RAD 0.017453

float theta=30.0;
float length=200.0;
float x=300,y=200;

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,640.0,0.0,480.0);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_POLYGON);
        glVertex2f(x,y);
        glVertex2f(x+length*cos(theta*DEG_TO_RAD),y+length*sin(theta*DEG_TO_RAD));
        glVertex2f(x+length*cos((theta+30)*DEG_TO_RAD),y+length*sin((theta+30)*DEG_TO_RAD));
    glEnd();
    glutSwapBuffers();
}

void idleFunc(){
    theta+=0.1;
    if(theta>=360.0) theta-=360.0;
    glutPostRedisplay();
}

void myMouse(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN){
        theta+=5.0;
        if(theta>=360.0) theta-=360.0;
    }
    if(button==GLUT_RIGHT_BUTTON&&state==GLUT_DOWN){
        theta-=5.0;
        if(theta<0.0) theta+=360.0;
    }
    glutPostRedisplay();
}

void myKeyboard(unsigned char key,int x,int y){
    if(key=='a'||key=='A')theta+=5.0;
    if(key=='d'||key=='D')theta-=5.0;
    if(key=='c') exit(0);
    if(theta>360) theta-=360;
    if(theta<0) theta+=360;
    glutPostRedisplay();
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(640,480);
    glutCreateWindow("Draw Triangle with Double Buffer");

    init();
    glutDisplayFunc(display);
    glutMouseFunc(myMouse);
    glutKeyboardFunc(myKeyboard);
    //glutIdleFunc(idleFunc);
    glutMainLoop();
    return 0;
}