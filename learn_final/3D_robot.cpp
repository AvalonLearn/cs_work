#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
 
#define SOLID 1
#define WIRE 2
 
#define stripeImageWidth 64
GLubyte stripeImage[3*stripeImageWidth];
//定义纹理图像
void makeStripeImage(void)
{
	int j;
	for (j = 0; j < stripeImageWidth; j++)
	{
		stripeImage[3*j] = 255;
		stripeImage[3*j+1] =255-2*j;
		stripeImage[3*j+2] =255;
	}
}
/* 参数设置 */
GLfloat sgenparams[] = {1.0, 1.0, 1.0, 0.0};

int moveX,moveY;
int spinX = 0;
int spinY = 0;
int des = 0;
 
void init() {
   //定义光源的颜色和位置
   GLfloat ambient[] = { 0.5, 0.8, 0.1, 0.1 };  
   GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  
   GLfloat position[] = { -80.0, 50.0, 25.0, 1.0 };  
   //选择光照模型
   GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };  
   GLfloat local_view[] = { 0.0 };  
   glClearColor(0.0, 0.0, 0.0, 0.0);   
   glShadeModel(GL_SMOOTH);  
   //设置环境光
   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);  
   //设置漫射光
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);  
   //设置光源位置
   glLightfv(GL_LIGHT0, GL_POSITION, position);  
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);  
   //启动光照
   glEnable(GL_LIGHTING);  
   //启用光源
   glEnable(GL_LIGHT0);  
   makeStripeImage();
	//函数设定从内存中读取纹理图并放到屏幕上的方式
	//指定内存中每个象素行起始的排列要求为字节排列（1）
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//定义纹理环境参数：调整当前亮度和颜色信息，使之适应纹理图像
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//纹理绕转使用重复方式
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//定义纹理放大和缩小函数均为GL_LINEAR
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//定义纹理
	glTexImage1D(GL_TEXTURE_1D, 0, 3, stripeImageWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, stripeImage);

	//控制纹理坐标的生成
	//指定单值纹理生成参数
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	//指定纹理坐标生成函数,系数由sgenparams指定
	glTexGenfv(GL_S, GL_OBJECT_PLANE, sgenparams);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glMaterialf (GL_FRONT, GL_SHININESS, 64.0);
}
 
//画球
void drawBall(double R, double x, double y,double z, int MODE) {
	glPushMatrix();
	glTranslated(x,y,z);
	if (MODE == SOLID) {
		glutSolidSphere(R,20,20);
	} else if (MODE ==WIRE) {
		glutWireSphere(R,20,20);
	}
	glPopMatrix();
}
 
//画半球
void drawHalfBall(double R, double x, double y,double z, int MODE) {
	glPushMatrix();
	glTranslated(x,y,z);
	GLdouble eqn[4]={0.0, 1.0, 0.0, 0.0};
	glClipPlane(GL_CLIP_PLANE0,eqn);
	glEnable(GL_CLIP_PLANE0);
	if (MODE == SOLID) {
		glutSolidSphere(R,20,20);
	} else if (MODE ==WIRE) {
		glutWireSphere(R,20,20);
	}
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
}
 
//画长方体
void drawSkewed(double l, double w, double h, double x, double y, double z, int MODE) {
	glPushMatrix();
	glScaled(l, w, h);
	glTranslated(x, y, z);
	if (MODE == SOLID) {
		glutSolidCube(1);
	} else if (MODE ==WIRE) {
		glutWireCube(1);
	}
	glPopMatrix();
}
 
void display(void) {
	//清除缓冲区颜色
	glClear(GL_COLOR_BUFFER_BIT);
	//定义白色
	glColor3f(1.0, 1.0, 1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//圆点放坐标中心
	glLoadIdentity();  
	//从哪个地方看
	gluLookAt(-2.0, -1.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
	glPushMatrix();
	glRotated(spinX, 0, 1, 0);
	glRotated(spinY, 1, 0, 0);
	glTranslated(0, 0, des);
	//头
	drawBall(2, 0, 1, 0, SOLID);
	//身体
	drawSkewed(5, 4.4, 4, 0, -0.75, 0, SOLID);
	//肩膀
	drawHalfBall(1, 3.5, -2.1, 0, SOLID);
	drawHalfBall(1, -3.5, -2.1, 0, SOLID);
	//胳膊
	drawSkewed(1, 3, 1, 3.5, -1.3, 0, SOLID);
	drawSkewed(1, 3, 1, -3.5, -1.3, 0, SOLID);
	//手
	drawBall(1, 3.5, -6.4, 0, SOLID);
	drawBall(1, -3.5, -6.4, 0, SOLID);
	//腿
	drawSkewed(1.2, 3, 2, 1, -2.4, 0, SOLID);
	drawSkewed(1.2, 3, 2, -1, -2.4, 0, SOLID);
	//脚
	drawSkewed(1.5, 1, 3, 0.9, -9.2, 0, SOLID);
	drawSkewed(1.5, 1, 3, -0.9, -9.2, 0, SOLID);
	glPopMatrix();
    glutSwapBuffers();
}
//鼠标点击事件
void mouseClick(int btn, int state, int x, int y) {
    moveX = x;
	moveY = y;
	GLfloat ambient[] = { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 0.1 };  
	 //设置环境光
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);  
	//启用光源
	glEnable(GL_LIGHT0);  
}
 
//键盘事件
void keyPressed(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        spinX -= 2;
        break;
    case 'd':
         spinX += 2;  
        break;
    case 'w':
        des += 2;
        break;
    case 's':
        des -= 2;
		break;
	}
	glutPostRedisplay();
}
// 鼠标移动事件 
void mouseMove(int x, int y) {
	int dx = x - moveX;
	int dy = y - moveY;
	printf("dx;%dx,dy:%dy\n",dx,dy);
	spinX += dx;
	spinY += dy;
	glutPostRedisplay();
	moveX = x;
	moveY = y;
}
 
void reshape(int w, int h) {
	//定义视口大小
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	//投影显示
	glMatrixMode(GL_PROJECTION);
	//坐标原点在屏幕中心
	glLoadIdentity();
	//操作模型视景
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}
 
int main(int argc, char** argv) {
	//初始化
	glutInit(&argc, argv);
	//设置显示模式
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//初始化窗口大小
	glutInitWindowSize(500, 500); 
	//定义左上角窗口位置
	glutInitWindowPosition(100, 100);
	//创建窗口
	glutCreateWindow(argv[0]);
	//初始化
	init();
	//显示函数
	glutDisplayFunc(display);
	//窗口大小改变时的响应
	glutReshapeFunc(reshape);
	//鼠标点击事件，鼠标点击或者松开时调用
    glutMouseFunc(mouseClick);
	//鼠标移动事件，鼠标按下并移动时调用
    glutMotionFunc(mouseMove);
	//键盘事件
	glutKeyboardFunc(keyPressed);
	//循环
	glutMainLoop();
	return 0;
}