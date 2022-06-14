#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iterator>

using namespace std;

struct point{
    double x;
    double y;
    double z=1;

    point(double x,double y){
        this->x=x;
        this->y=y;
        this->z=1;
    }
};

vector<point> vertice;

int transformMatrix[3][3]={{1,0,0},{0,1,0},{50,50,1}};

int winWidth=400,winHeight=300;

void drawShape(){
    if(vertice.size()<2)
        return;
    glBegin(GL_LINES);
        for(int i=0;i<vertice.size()-1;i++){
            glVertex2i(vertice[i].x,vertice[i].y);
            glVertex2i(vertice[i+1].x,vertice[i+1].y);
        }
        glVertex2i(vertice[vertice.size()-1].x,vertice[vertice.size()-1].y);
        glVertex2i(vertice[0].x,vertice[0].y);
    glEnd();
    glFlush();
}

void transform(){
    int size=vertice.size();
    if (size<1) return;
    for(int i=0;i<size;i++){
        double x=vertice[i].x;
        double y=vertice[i].y;
        double z=vertice[i].z;
        double newX=transformMatrix[0][0]*x+transformMatrix[1][0]*y+transformMatrix[2][0]*z;
        double newY=transformMatrix[0][1]*x+transformMatrix[1][1]*y+transformMatrix[2][1]*z;
        double newZ=transformMatrix[0][2]*x+transformMatrix[1][2]*y+transformMatrix[2][2]*z;
        vertice[i].x=newX/newZ;
        vertice[i].y=newY/newZ;
        vertice[i].z=newZ/newZ;
        printf("vertice[%d]=%lf,%lf\n",i,vertice[i].x,vertice[i].y);
    }
    glColor3f(1.0,0.0,0.0);
    drawShape();
    glFlush();
}

void myMouse(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN){
        glColor3f(0.0,1.0,1.0);
        glBegin(GL_POINTS);
            glVertex2i(x-winWidth/2,winHeight/2-y);
            //glVertex2i(x,winHeight-y);
        glEnd();
        glFlush();
        //point p(x,winHeight-y);
        point p(x-winWidth/2,winHeight/2-y);
        vertice.push_back(p);
        printf("顶点:(%d,%d)\n",x,winHeight-y);
    }
    if(button==GLUT_RIGHT_BUTTON&&state==GLUT_DOWN){
        glColor3f(0.0,1.0,1.0);
        drawShape();
        transform();
    }
}

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    //红绿蓝透明度
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-(double)winWidth/2,(double)winWidth/2,-(double)winHeight/2,(double)winHeight/2);
    //gluOrtho2D(0.0,(double)winWidth,0.0,(double)winHeight);
    //使用正投影将世界坐标系二维矩形区域的内容映射到屏幕上（0，0）到（winWidth，winHeight）
}

void winReshapeFcn(int newWidth,int newHeight){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluOrtho2D(0.0,(GLdouble)newWidth,0.0,(GLdouble)newHeight);
    gluOrtho2D(-(GLdouble)newWidth/2,(GLdouble)newWidth/2,-(GLdouble)newHeight/2,(GLdouble)newHeight/2);
    glClear(GL_COLOR_BUFFER_BIT);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(2);
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_POINTS);
        glVertex2i(0,0);
    glEnd();
    //transform();
    glColor3f(0.0,1.0,1.0);
    drawShape();
    glFlush();
}


int main(int argv,char** argc){
    printf("请输入变换矩阵:\n");
    printf("第一行：");
    scanf("%d%d%d",&transformMatrix[0][0],&transformMatrix[0][1],&transformMatrix[0][2]);
    printf("第二行：");
    scanf("%d%d%d",&transformMatrix[1][0],&transformMatrix[1][1],&transformMatrix[1][2]);
    printf("第三行：");
    scanf("%d%d%d",&transformMatrix[2][0],&transformMatrix[2][1],&transformMatrix[2][2]);
    
    glutInit(&argv,argc);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(500,150);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("二维几何变换");

    init();
    glutDisplayFunc(display);
    glutMouseFunc(myMouse);

    glutMainLoop();
}