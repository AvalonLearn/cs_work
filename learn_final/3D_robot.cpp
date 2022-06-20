#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
 
#define SOLID 1
#define WIRE 2
 
#define stripeImageWidth 64
GLubyte stripeImage[3*stripeImageWidth];
//��������ͼ��
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
/* �������� */
GLfloat sgenparams[] = {1.0, 1.0, 1.0, 0.0};

int moveX,moveY;
int spinX = 0;
int spinY = 0;
int des = 0;
 
void init() {
   //�����Դ����ɫ��λ��
   GLfloat ambient[] = { 0.5, 0.8, 0.1, 0.1 };  
   GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  
   GLfloat position[] = { -80.0, 50.0, 25.0, 1.0 };  
   //ѡ�����ģ��
   GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };  
   GLfloat local_view[] = { 0.0 };  
   glClearColor(0.0, 0.0, 0.0, 0.0);   
   glShadeModel(GL_SMOOTH);  
   //���û�����
   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);  
   //���������
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);  
   //���ù�Դλ��
   glLightfv(GL_LIGHT0, GL_POSITION, position);  
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);  
   //��������
   glEnable(GL_LIGHTING);  
   //���ù�Դ
   glEnable(GL_LIGHT0);  
   makeStripeImage();
	//�����趨���ڴ��ж�ȡ����ͼ���ŵ���Ļ�ϵķ�ʽ
	//ָ���ڴ���ÿ����������ʼ������Ҫ��Ϊ�ֽ����У�1��
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//����������������������ǰ���Ⱥ���ɫ��Ϣ��ʹ֮��Ӧ����ͼ��
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//������תʹ���ظ���ʽ
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//��������Ŵ����С������ΪGL_LINEAR
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//��������
	glTexImage1D(GL_TEXTURE_1D, 0, 3, stripeImageWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, stripeImage);

	//�����������������
	//ָ����ֵ�������ɲ���
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	//ָ�������������ɺ���,ϵ����sgenparamsָ��
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
 
//����
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
 
//������
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
 
//��������
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
	//�����������ɫ
	glClear(GL_COLOR_BUFFER_BIT);
	//�����ɫ
	glColor3f(1.0, 1.0, 1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Բ�����������
	glLoadIdentity();  
	//���ĸ��ط���
	gluLookAt(-2.0, -1.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
	glPushMatrix();
	glRotated(spinX, 0, 1, 0);
	glRotated(spinY, 1, 0, 0);
	glTranslated(0, 0, des);
	//ͷ
	drawBall(2, 0, 1, 0, SOLID);
	//����
	drawSkewed(5, 4.4, 4, 0, -0.75, 0, SOLID);
	//���
	drawHalfBall(1, 3.5, -2.1, 0, SOLID);
	drawHalfBall(1, -3.5, -2.1, 0, SOLID);
	//�첲
	drawSkewed(1, 3, 1, 3.5, -1.3, 0, SOLID);
	drawSkewed(1, 3, 1, -3.5, -1.3, 0, SOLID);
	//��
	drawBall(1, 3.5, -6.4, 0, SOLID);
	drawBall(1, -3.5, -6.4, 0, SOLID);
	//��
	drawSkewed(1.2, 3, 2, 1, -2.4, 0, SOLID);
	drawSkewed(1.2, 3, 2, -1, -2.4, 0, SOLID);
	//��
	drawSkewed(1.5, 1, 3, 0.9, -9.2, 0, SOLID);
	drawSkewed(1.5, 1, 3, -0.9, -9.2, 0, SOLID);
	glPopMatrix();
    glutSwapBuffers();
}
//������¼�
void mouseClick(int btn, int state, int x, int y) {
    moveX = x;
	moveY = y;
	GLfloat ambient[] = { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 0.1 };  
	 //���û�����
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);  
	//���ù�Դ
	glEnable(GL_LIGHT0);  
}
 
//�����¼�
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
// ����ƶ��¼� 
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
	//�����ӿڴ�С
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	//ͶӰ��ʾ
	glMatrixMode(GL_PROJECTION);
	//����ԭ������Ļ����
	glLoadIdentity();
	//����ģ���Ӿ�
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}
 
int main(int argc, char** argv) {
	//��ʼ��
	glutInit(&argc, argv);
	//������ʾģʽ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//��ʼ�����ڴ�С
	glutInitWindowSize(500, 500); 
	//�������ϽǴ���λ��
	glutInitWindowPosition(100, 100);
	//��������
	glutCreateWindow(argv[0]);
	//��ʼ��
	init();
	//��ʾ����
	glutDisplayFunc(display);
	//���ڴ�С�ı�ʱ����Ӧ
	glutReshapeFunc(reshape);
	//������¼�������������ɿ�ʱ����
    glutMouseFunc(mouseClick);
	//����ƶ��¼�����갴�²��ƶ�ʱ����
    glutMotionFunc(mouseMove);
	//�����¼�
	glutKeyboardFunc(keyPressed);
	//ѭ��
	glutMainLoop();
	return 0;
}