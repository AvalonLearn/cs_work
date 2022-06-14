#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

GLsizei winWidth = 400, winHeight = 400;
int r;

void winReshapeFcn(int newWidth,int newHeight){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,(GLdouble)newWidth,0,(GLdouble)newHeight);
    glClear(GL_COLOR_BUFFER_BIT);
}

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    //红绿蓝透明度
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,400.0,0.0,400.0);
    //使用正投影将世界坐标系二维矩形区域的内容映射到屏幕上（0，0）到（200，150）
}

void setPixel(int x,int y,int x0,int y0){
    glBegin(GL_POINTS);
    glVertex2i(x+x0,y+y0);
    glVertex2i(-x+x0,y+y0);
    glVertex2i(y+y0,x+x0);
    glVertex2i(-y+y0,x+x0);
    glVertex2i(x+x0,-y+y0);
    glVertex2i(-x+x0,-y+y0);
    glVertex2i(y+y0,-x+x0);
    glVertex2i(-y+y0,-x+x0);
    glEnd();
    glFlush();
}

void bresenhamCircle(int r){
    int x0=winWidth/2,y0=winHeight/2;
    int x=0,y=r;
    double d=1.25-r;
    setPixel(x,y,x0,y0);
    while(x<y){
        x=x+1;
        if(d<=0){
            d=d+2*x+3;
        }else{
            d=d+2*(x-y)+5;
            y=y-1;
        }
        setPixel(x,y,x0,y0);
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0,1.0,1.0);
    glBegin(GL_POINTS);
    bresenhamCircle(r);
    glEnd();
    glFlush();
}

int main(int argv,char** argc){

    printf("请输入圆的半径：");
    scanf("%d",&r);

    glutInit(&argv,argc);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(500,150);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("Bresenham Circle Program");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(winReshapeFcn);

    glutMainLoop();
    return 0;
}