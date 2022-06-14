#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int startX, startY, endX, endY;

inline int myRound (const double a){
    return int(a+0.5);
}

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    //������͸����
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,200.0,0.0,150.0);
    //ʹ����ͶӰ����������ϵ��ά�������������ӳ�䵽��Ļ�ϣ�0��0������200��150��
}

void ddaLine(int x1,int y1,int x2,int y2){
    int diffX=x2-x1;
    int diffY=y2-y1;
    double incrementX,incrementY;
    int steps=fabs(diffX)>fabs(diffY)?fabs(diffX):fabs(diffY);
    incrementX=double(diffX)/steps;
    incrementY=double(diffY)/steps;
    double x=x1,y=y1;
    glVertex2i(x,y);
    int i;
    for(i=0;i<steps;i++){
        x+=incrementX;
        y+=incrementY;
        glVertex2i(x,y);
    }
}

void lineSegment(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(0.0,0.4,0.2);
    glBegin(GL_POINTS);
        ddaLine(startX,startY,endX,endY);
    glEnd();
    glFlush();
}

int main(int argc,char** argv){
    //���ÿ���̨����chcp 65001
    //ѧ�����27��
    printf("���������(180,15)��");
    scanf("%d,%d",&startX,&startY);
    printf("�������յ�(10,145)��");
    scanf("%d,%d",&endX,&endY);

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(50,100);
    glutInitWindowSize(400,300);
    glutCreateWindow("DDA Line Program");
    init();
    glutDisplayFunc(lineSegment);
    glutMainLoop();
    return 0;
}