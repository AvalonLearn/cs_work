#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

GLsizei winWidth = 400, winHeight = 300;

int wxl, wxr, wyb, wyt;
const int LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;
int ptCtr = 0;

class scrPt
{
public:
    GLint x, y;
};

void init()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    //红绿蓝透明度
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, (GLdouble)winWidth, 0.0, (GLdouble)winHeight);
    //使用正投影将世界坐标系二维矩形区域的内容映射到屏幕上（0，0）到（winWidth，winHeight）
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void drawLineSegment(scrPt p1, scrPt p2)
{
    glColor3f(0.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex2i(p1.x, p1.y);
    glVertex2i(p2.x, p2.y);
    glEnd();
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, GLdouble(newWidth), 0.0, GLdouble(newHeight));
    winWidth = newWidth;
    winHeight = newHeight;
}

void Encode(int x, int y, int *code)
{
    *code = 0;
    if (x < wxl)
        *code |= LEFT;
    if (x > wxr)
        *code |= RIGHT;
    if (y < wyb)
        *code |= BOTTOM;
    if (y > wyt)
        *code |= TOP;
}

void clip(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
    static scrPt endPt1, endPt2;
    int code1 = 0, code2 = 0;
    if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN)
    {
        if (ptCtr == 0)
        {
            endPt1.x = xMouse;
            endPt1.y = winHeight - yMouse;
            ptCtr = 1;
            glColor3f(0.0, 1.0, 1.0);
            glPointSize(2);
            glBegin(GL_POINTS);
            glVertex2i(endPt1.x, endPt1.y);
            glEnd();
            glFlush();
        }
        else
        {
            endPt2.x = xMouse;
            endPt2.y = winHeight - yMouse;
            glColor3f(0.0, 1.0, 1.0);
            glPointSize(2);
            glBegin(GL_POINTS);
            glVertex2i(endPt2.x, endPt2.y);
            glEnd();
            glFlush();
            drawLineSegment(endPt1, endPt2);
            ptCtr = 0;
        }
        glFlush();
    }
    if (button == GLUT_RIGHT_BUTTON && action == GLUT_DOWN)
    {
        while (1)
        {
            Encode(endPt1.x, endPt1.y, &code1);
            Encode(endPt2.x, endPt2.y, &code2);
            if ((code1 | code2) == 0)
            {
                drawLineSegment(endPt1, endPt2);
                glFlush();
                break;
            }
            else if (code1 & code2)
            {
                glClearColor(1.0, 1.0, 1.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT);
                glFlush();
                break;
            }
            else
            {
                printf("x1=%d,y1=%d,x2=%d,y2=%d\n", endPt1.x, endPt1.y, endPt2.x, endPt2.y);
                GLint x, y;
                int code=code1;
                if(code==0){code=code2;}
                double k = (double)(endPt2.y - endPt1.y) / (double)(endPt2.x - endPt1.x);
                printf("k=%lf\n", k);
                if(code&LEFT)
                {
                    x = wxl;
                    y = endPt1.y + k * (x - endPt1.x);
                }
                else if(code&RIGHT)
                {
                    x = wxr;
                    y = endPt1.y + k * (x - endPt1.x);
                }
                else if(code&BOTTOM)
                {
                    y = wyb;
                    x = endPt1.x + (y - endPt1.y) / k;
                }
                else if(code&TOP)
                {
                    y = wyt;
                    x = endPt1.x + (y - endPt1.y) / k;
                }
                if(code==code1){
                    endPt1.x=x;
                    endPt1.y=y;
                }else{
                    endPt2.x=x;
                    endPt2.y=y;
                }
            }
        }
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        printf("x1=%d,y1=%d,x2=%d,y2=%d\n", endPt1.x, endPt1.y, endPt2.x, endPt2.y);
        drawLineSegment(endPt1, endPt2);
        scrPt bound[4];
        bound[0].x = wxl;bound[0].y = wyb;
        bound[1].x = wxr;bound[1].y = wyb;
        bound[2].x = wxl;bound[2].y = wyt;
        bound[3].x = wxr;bound[3].y = wyt;
        drawLineSegment(bound[0], bound[1]);
        drawLineSegment(bound[2], bound[3]);
        drawLineSegment(bound[0], bound[2]);
        drawLineSegment(bound[1], bound[3]);
        glFlush();
    }
}

int main(int argv, char **argc)
{

    printf("请输入裁剪窗口的左下角与右上角的坐标：\n");
    printf(">>");
    scanf("%d %d %d %d", &wxl, &wyb, &wxr, &wyt);

    glutInit(&argv, argc);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cohen-Sutherland Line Clipping");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(winReshapeFcn);
    glutMouseFunc(clip);

    glutMainLoop();
}