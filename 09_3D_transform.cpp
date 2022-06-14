#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

GLsizei winWidth=500,winHeight=500;

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,1,0,100);
    //第一个参数为fov角，第二个参数是宽高比
    //后两个参数分别是最近可视距离和最远可视距离
}

void Move(){
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1, 0, -10);//向xyz平移的方向
	glColor3f(1, 0, 0);
	glutWireCube(1);

	glTranslatef(3, 0, 0);
	glColor3f(0, 1, 0);
	glutWireCube(1);
}

void Scale(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1, 0, -10);
	glColor3f(1, 0, 0);
	glutWireCube(1);

	glScalef(1.5, 1.5, 1.5);
	glColor3f(0, 1, 0);
	glutWireCube(1);
}

void RotateV(){
	//opengl三维旋转(绕平行于坐标轴的轴旋转)
	//此例为原点绕经过(0,0,-5)垂直于xz平面的旋转轴进行旋转
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//为了方便查看效果，将整体向z轴负向平移10个单位
	glTranslatef(0, -2, -10);
	glColor3f(0.0,0.0,1);
	glBegin(GL_LINES);
	//绘制出旋转轴
	glVertex3f(0, -10, -5);
	glVertex3f(0, 10, -5);
	glEnd();
	glFlush();
	for (int i = 0; i < 18; i++)
	{
		//进行逆平移
		glTranslatef(0, 0, -5);
		//进行所需的旋转
		glRotatef(20, 0, 1, 0);
		//将坐标轴平移至与y轴重合
		glTranslatef(0, 0, 5);
		glColor3f(1, 0, 0);
		glutWireCube(1);
	}
	glTranslatef(0, 4, 0);
	for (int i = 0; i < 18; i++)
	{
		//进行逆平移
		glTranslatef(0, 0, -5);
		//进行所需的旋转
		glRotatef(20, 0, 1, 0);
		//将坐标轴平移至与y轴重合
		glTranslatef(0, 0, 5);
		glColor3f(0, 1, 0);
		glutWireCube(1);
	}
}

void RotateAny(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1, 4, -20);
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	//绘制出旋转轴
	glVertex3f(-25, -30, -25);
	glVertex3f(20, 15, 20);
	glEnd();
	glColor3f(1, 0, 0);
	for (int i = 0; i < 36; i++)
	{
		//逆变换
		glTranslatef(5, 0, 5);
		glRotatef(-45, 0, 0, 1);
		glRotatef(45, 1, 0, 0);
		glRotatef(10, 0, 1, 0);
		//再绕x轴顺时针旋转45度使旋转轴与y轴重合
		glRotatef(-45, 1, 0, 0);
		//先绕z轴逆时针旋转45度使旋转轴位于yz平面上
		glRotatef(45, 0, 0, 1);
		//先将旋转轴平移至经过原点
		glTranslatef(-5, 0, -5);
		glutWireCube(1);
	}
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    //Move();
    //Scale();
    //RotateV();
    RotateAny();
    glFlush();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(winWidth,winHeight);
    glutInitWindowPosition(400,100);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);

    glutCreateWindow("三维几何变换");

    init();
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
