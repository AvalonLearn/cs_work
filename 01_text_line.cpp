#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    //������͸����
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,200.0,0.0,150.0);
    //ʹ����ͶӰ����������ϵ��ά�������������ӳ�䵽��Ļ�ϣ�0��0������200��150��
}

void lineSegment(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0,0.4,0.2);
    glBegin(GL_LINES);
        glVertex2i(180,15);
        glVertex2i(10,145);
    glEnd();
    glFlush();
}

int main(int argc,char** argv){
    //���ÿ���̨����chcp 65001
    //ѧ�����27��
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(50,100);
    glutInitWindowSize(400,300);
    glutCreateWindow("An Example OpenGL Program");
    init();
    glutDisplayFunc(lineSegment);
    glutMainLoop();
    return 0;
}