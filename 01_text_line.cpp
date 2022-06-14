#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    //红绿蓝透明度
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,200.0,0.0,150.0);
    //使用正投影将世界坐标系二维矩形区域的内容映射到屏幕上（0，0）到（200，150）
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
    //设置控制台编码chcp 65001
    //学生编号27号
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