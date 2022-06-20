#include <windows.h>
#include <GL/glut.h>
#include<stdio.h>
#include <stdlib.h>
static int sunday = 0,year = 0, day = 0, month = 0, moonday = 0;
static GLuint texname[3];
char sunname[] = "sun.jpg";
char earthname[] = "ear.jpg";
char moonname[] = "moon.jpg";
GLuint texture_id1;//纹理id
GLuint texture_id2;
GLuint texture_id3;
GLUquadricObj* sun = gluNewQuadric();
GLUquadricObj* ear = gluNewQuadric();
GLUquadricObj* moon = gluNewQuadric();
int load_texture(char* file_name, int width, int height, int depth, GLenum colour_type, GLenum filter_type)
{
    GLubyte* raw_bitmap;
    FILE* file;
    int errnum;
    if ((errnum = fopen_s(&file, file_name, "rb")) != 0)
    {
        return 1;
    }
    raw_bitmap = (GLubyte*)malloc(width * height * depth * (sizeof(GLubyte)));
    if (raw_bitmap == NULL)
    {
        fclose(file);
        return 2;
    }
    fread(raw_bitmap, width * height * depth, 1, file);
    fclose(file);
 
    //  设置过滤类型
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type);
 
    //  设置纹理环境
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, colour_type,
        GL_UNSIGNED_BYTE, raw_bitmap);
    free(raw_bitmap);
    return 0;
}
void LoadTextures(char *filename,GLuint texture_id, int MaxNrOfTextures)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(MaxNrOfTextures, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    load_texture(filename, 512, 257, 3, GL_BGR_EXT, GL_NEAREST);
}
 
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_TEXTURE_2D);
    glClearDepth(1.0f);    // 设置深度缓存
    glDepthFunc(GL_LEQUAL);  // 选择深度测试方式
    glEnable(GL_DEPTH_TEST);  // 开启深度测试
    glShadeModel(GL_SMOOTH);  // 阴暗处理采用平滑方式
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // 最精细的透视计算
}
void display(void)
{
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// glutWireSphere中第一、二、三个参数分别表示半径，经度线数、纬度线数
    glRotatef((GLfloat)sunday, 0.0, 0.0, 1.0);
    LoadTextures(sunname, texture_id1, texname[0]);
    glBindTexture(GL_TEXTURE_2D, texname[0]);
    gluQuadricDrawStyle(sun, GLU_FILL);
    gluQuadricNormals(sun, GLU_SMOOTH);
    gluQuadricTexture(sun, GL_TRUE);
	gluSphere(sun,1.0, 20, 16);  /* draw sun */
  //绕Z轴旋转
	glRotatef((GLfloat)year, 0.0, 0.0, 1.0);
	glTranslatef(2.0, 0.0, 0.0);
	glRotatef((GLfloat)day, 0.0, 0.0, 1.0);
    LoadTextures(earthname, texture_id2, texname[1]);
    glBindTexture(GL_TEXTURE_2D, texname[1]);
    gluQuadricDrawStyle(ear, GLU_FILL);
    gluQuadricNormals(ear, GLU_SMOOTH);
    gluQuadricTexture(ear, GL_TRUE);
	gluSphere(ear,0.2, 10, 8);  /* draw smaller planet */
	glRotatef((GLfloat)month, 0.0, 0.0, 1.0); 
	glTranslatef(0.3, 0.0, 0.0);
	glRotatef((GLfloat)moonday, 0.0, 0.0, 1.0);
    LoadTextures(moonname, texture_id3, texname[2]);
    glBindTexture(GL_TEXTURE_2D, texname[2]);
    gluQuadricDrawStyle(moon, GLU_FILL);
    gluQuadricNormals(moon, GLU_SMOOTH);
    gluQuadricTexture(moon, GL_TRUE);
    gluSphere(moon,0.05, 10, 8);  /* draw moon */
    glPopMatrix();
	glutSwapBuffers();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//从z的正方向看向坐标原点
	gluLookAt(1.0, 1.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
void TimerFunc(int value)
{
	glutPostRedisplay();
    sunday = sunday + 1;
	day = day + 1;
	year = year + 1;
	month = month + 25;
	glutTimerFunc(100, TimerFunc, 1);
}
 
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(100,TimerFunc,1);
    glutMainLoop();
    return 0;
}