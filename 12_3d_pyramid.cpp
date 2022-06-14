#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

float theta=0.0;
float beta=0.0;

void drawPyramid()
{
    glBegin(GL_TRIANGLES);
      glColor3f(1.0,0.0,0.0);
      glVertex3f(0.0,1.0,0.0);
      glVertex3f(-1.0,-1.0,1.0);
      glVertex3f(1.0,-1.0,1.0);
      //����ǰ��ĺ�ɫ������

      glColor3f(0.0,1.0,0.0);
      glVertex3f(0.0,1.0,0.0);
      glVertex3f(1.0,-1.0,1.0);
      glVertex3f(1.0,-1.0,-1.0);
      //�����������ɫ������

      glColor3f(0.0,0.0,1.0);
      glVertex3f(0.0,1.0,0.0);
      glVertex3f(1.0,-1.0,-1.0);
      glVertex3f(-1.0,-1.0,-1.0);
      //���ƺ������ɫ������

      glColor3f(1.0,1.0,0.0);
      glVertex3f(0.0,1.0,0.0);
      glVertex3f(-1.0,-1.0,-1.0);
      glVertex3f(-1.0,-1.0,1.0);
      //��������Ļ�ɫ������
    glEnd();

    glBegin(GL_POLYGON);
      glColor3f(0.5,0.5,0.5);
      glVertex3f(-1.0,-1.0,1.0);
      glVertex3f(1.0,-1.0,1.0);
      glVertex3f(1.0,-1.0,-1.0);
      glVertex3f(-1.0,-1.0,-1.0);
      //���Ƶ���ĺ�ɫ������
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //�����ɫ����Ȼ���
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //����ģ�͹۲����
    //gluLookAt(2.0,2.0,2.0,0.0,0.0,0.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,-5.0);
    glRotatef(theta,0.0,1.0,0.0);
    glRotatef(beta,1.0,0.0,0.0);
    drawPyramid();
    glutSwapBuffers();
    //��������
}

void reshape(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluFrustum(-1.0,1.0,-1.0,1.0,3.1,10.0);
    //gluPerspective(45,1,0.1,10.0);
    glOrtho(-2.0,2.0,-2.0,2.0,2.0,10.0);
}

void init()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);
}

void myKeyboard(unsigned char key,int x,int y)
{
    if(key=='a'||key=='A') theta+=5.0;
    if(key=='d'||key=='D') theta-=5.0;
    if(key=='w'||key=='W') beta+=5.0;
    if(key=='s'||key=='S') beta-=5.0;
    if(key=='c'||key=='C') exit(0);
    if(theta>360) theta-=360;
    if(theta<0) theta+=360;
    glutPostRedisplay();
    //���µ��û��ƺ���
}

int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(450,100);
    glutCreateWindow("pyramid");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboard);
    init();
    glutMainLoop();
}