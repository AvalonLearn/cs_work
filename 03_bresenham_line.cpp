#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int startX, startY, endX, endY;

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    //红绿蓝透明度
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,200.0,0.0,150.0);
    //使用正投影将世界坐标系二维矩形区域的内容映射到屏幕上（0，0）到（200，150）
}

void bresenhamLine(int x1,int y1,int x2,int y2){
    if(x1>x2){
        int temp=x1;
        x1=x2;
        x2=temp;
        temp=y1;
        y1=y2;
        y2=temp;
    }

    int diffX=x2-x1;
    int diffY=y2-y1;
    int x=x1,y=y1;
    int d;
    double k=double(diffY)/double(diffX);
    glVertex2i(x,y);
    int w=1;
    if(k<=0) w=-1;
    //if(k>0){
        if(abs(k<=1)){
            //printf("0<k<=1\n");
            d=w*diffX-2*diffY;
            while(x!=x2){
                x++;
                if(w*d<0){
                    y=y+w;
                    d=d+w*2*diffX-2*diffY;
                }else{
                    y=y;
                    d=d-2*diffY;
                }
                //printf("x=%d,y=%d\n",x,y);
                glVertex2i(x,y);
            }
        }else{
            //printf("k>1\n");
            d=w*2*diffX-diffY;
            while(y!=y2){
                y++;
                if(w*d>0){
                    x=x+1;
                    d=d+w*2*diffX-2*diffY;
                }else{
                    x=x;
                    d=d+w*2*diffX;
                }
                //printf("x=%d,y=%d\n",x,y);
                glVertex2i(x,y);
            }
        }
    /*
    }else{
        //k<=0
        if(k>=-1){
            printf("-1<=k<=0\n");
            d=-diffX-2*diffY;
            while(x!=x2){
                x++;
                if(d>=0){
                    y=y-1;
                    d=d-2*diffX-2*diffY;
                }else{
                    y=y;
                    d=d-2*diffY;
                }
                printf("x=%d,y=%d\n",x,y);
                glVertex2i(x,y);
            }
        }else{
            printf("k<-1\n");
            d=-2*diffX-diffY;
            while(y!=y2){
                y--;
                if(d<0){
                    x=x+1;
                    d=d-2*diffX-diffY;
                }else{
                    x=x;
                    d=d-2*diffX;
                }
                printf("x=%d,y=%d\n",x,y);
                glVertex2i(x,y);
            }
        }
    }*/
}

void lineSegment(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_POINTS);
        bresenhamLine(startX,startY,endX,endY);
    glEnd();
    glFlush();
}

int main(int argv,char** argc){
    //设置控制台编码chcp 65001
    //学生编号27号
    printf("请输入起点(180,15)：");
    scanf("%d,%d",&startX,&startY);
    printf("请输入终点(10,145)：");
    scanf("%d,%d",&endX,&endY);

    glutInit(&argv,argc);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(50,100);
    glutInitWindowSize(400,300);
    glutCreateWindow("Bresenham Line Program");
    init();
    glutDisplayFunc(lineSegment);
    glutMainLoop();
    return 0;
}