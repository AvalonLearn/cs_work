#include <windows.h>
#include <GL/glut.h>
#include "GL/GLAUX.H"
#include <stdio.h>
#include <tchar.h>

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

using namespace std;

HGLRC hRC = NULL;    //着色描述表
HDC hDC = NULL;      //设备描述表
HWND hWnd = NULL;    //窗口句柄
HINSTANCE hInstance; //应用程序实例

bool keys[256];          //按键状态
bool active = true;      //激活状态
bool fullscreen = FALSE; //全屏状态

char *window_title = "猴头期末实验";
//旋转用
const float piover180 = 0.0174532925f;
float heading = 0.0f;
float xpos = 0.0f;
float ypos = 0.0f;
float zpos = 5.0f;
const int xlen = 10;
const int ylen = 10;
const float worldsize = 10.0f;
float meshtable[xlen][ylen][2];

GLfloat xrot; // X Rotation
GLfloat yrot; // Y Rotation
GLfloat walkbias = 0;
GLfloat walkbiasangle = 0;
GLfloat lookupdown = 0.0f;

BOOL light;
BOOL blend; //是否混合
BOOL bp;    // B键按下
BOOL cp;    // C键按下
BOOL lp;    // L键按下
BOOL sp;    //空格键是否按下
int part1;  //圆盘的起始角度
int part2;  //圆盘的结束角度

GLfloat LightAmbient[] = {0.8f, 0.8f, 0.8f, 1.0f};
//环境光参数
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
//漫反射光参数
GLfloat LightPosition[] = {5.0f, 0.0f, 0.0f, 1.0f};
//光源位置

GLuint filter; //纹理选择

typedef struct Texture
{
    GLuint id;
    int width;
    int height;
    int nrChannels;
    unsigned char *data;
    int loadTexture(char *Filepath);
    void freeTexture();
} Texture;
//纹理结构体

int Texture::loadTexture(char *Filepath)
{
    data = stbi_load(Filepath, &width, &height, &nrChannels, 0);
    if (data)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void Texture::freeTexture()
{
    stbi_image_free(data);
}
//纹理相关定义
const int length = 11;
Texture texture[length];
GLuint monkeymodel;
GLuint shadow;

typedef struct sPoint{
    GLfloat x, y, z;
} sPoint;//3D顶点
typedef struct sPlaneEq{
    GLfloat a, b, c, d;
} sPlaneEq;//平面方程
typedef struct sPlane{
    unsigned int indexp[3];// 3个整形索引指定了模型中三角形的三个顶点坐标
    sPoint p[3];			
	sPoint normals[3];			// 第二个变量指定了三角形面的法线
	unsigned int neigh[3]; 		// 与本三角形三个边相邻的面的索引
	sPlaneEq PlaneEq;			// 平面方程描述了三角所在的平面
	bool visible;			// 最后一个变量指定这个三角形是否投出阴影?
}sPlane;
typedef struct glObject{
    GLuint nPlanes, nPoints;
    sPoint points[100];
    sPlane planes[200];
} glObject;

void calculatePlane(sPlane& plane){
    sPoint v1 = plane.p[0];
    sPoint v2 = plane.p[1];
    sPoint v3 = plane.p[2];

    plane.PlaneEq.a=v1.y*(v2.z-v3.z) + v2.y*(v3.z-v1.z) + v3.y*(v1.z-v2.z);
	plane.PlaneEq.b = v1.z*(v2.x-v3.x) + v2.z*(v3.x-v1.x) + v3.z*(v1.x-v2.x);
	plane.PlaneEq.c = v1.x*(v2.y-v3.y) + v2.x*(v3.y-v1.y) + v3.x*(v1.y-v2.y);
	plane.PlaneEq.d = -( v1.x*( v2.y*v3.z - v3.y*v2.z ) +
				v2.x*(v3.y*v1.z - v1.y*v3.z) +
				v3.x*(v1.y*v2.z - v2.y*v1.z) );
}

inline void SetConnectivity(vector<sPlane>& planes){
    unsigned int p1i, p2i, p1j, p2j;
    unsigned int P1i, P2i, P1j, P2j;
    unsigned int i, j, ki, kj;
    for (i = 0; i < planes.size();i++){
        for (j = 0; j < 3;j++){
            planes[i].neigh[j] = 0;
        }
    }
        for (i = 0; i < planes.size(); i++)
        {
            for (j = i + 1; j < planes.size(); j++)
            {
                for (ki = 0; ki < 3; ki++)
                {
                    if (!planes[i].neigh[ki])
                    {
                        for (kj = 0; kj < 3; kj++)
                        {
                            p1i = ki;
                            p1j = kj;
                            p2i = (ki + 1) % 3;
                            p2j = (kj + 1) % 3;

                            p1i = planes[i].indexp[p1i];
                            p2i = planes[i].indexp[p2i];
                            p1j = planes[j].indexp[p1j];
                            p2j = planes[j].indexp[p2j];

                            //如果面A的边P1i->P1j和面B的边P2i->P2j为同一条边，则又下面的公式的P1i=P1j，并且P2i=P2j
                            P1i = ((p1i + p2i) - abs(float(p1i) - p2i)) / 2;
                            P2i = ((p1i + p2i) + abs(float(p1i) - p2i)) / 2;
                            P1j = ((p1j + p2j) - abs(float(p1j) - p2j)) / 2;
                            P2j = ((p1j + p2j) + abs(float(p1j) - p2j)) / 2;

                            if ((P1i == P1j) && (P2i == P2j))
                            {
                                planes[i].neigh[ki] = j + 1;
                                planes[j].neigh[kj] = i + 1;
                            }
                        }
                    }
                }
            }
        }
}

void doShadowPass(vector<sPlane>& planes,float *lp){
    unsigned int	i, j, k, jj;
	sPoint p1, p2;
    sPoint v1, v2;
    //对模型中的每一个面
	for (i=0; i<planes.size();i++)
	{	
		//如果面在灯光的前面
		if (planes[i].visible)
		{
			//对于被灯光照射的面的每一个相邻的面
			for (j=0;j<3;j++)
			{
				k = planes[i].neigh[j];
				//如果面不存在，或不被灯光照射，那么这个边是边界
				if ((!k) || (planes[k-1].visible))
				{
					// 获得面的两个顶点
					p1 = planes[i].p[j];
					jj = (j+1)%3;
					p2 = planes[i].p[jj];

					//计算边的顶点到灯光的方向，并放大100倍
					v1.x = (p1.x - lp[0])*100;
					v1.y = (p1.y - lp[1])*100;
					v1.z = (p1.z - lp[2])*100;

					v2.x = (p2.x - lp[0])*100;
					v2.y = (p2.y - lp[1])*100;
					v2.z = (p2.z - lp[2])*100;
					
					//绘制构成阴影体边界的面
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(p1.x,p1.y,p1.z);
						glVertex3f(p1.x + v1.x,p1.y + v1.y,p1.z + v1.z);
						glVertex3f(p2.x,p2.y,p2.z);
						glVertex3f(p2.x + v2.x,p2.y + v2.y,p2.z + v2.z);
					glEnd();
				}
			}
		}
	}
}

void castShadow(vector<sPlane>& planes, GLfloat* lightPosition){
    // 设置哪些面在灯光的前面
	for ( int i = 0; i < planes.size(); i++ )
	{
        sPlaneEq planeEq = planes[i].PlaneEq;

        GLfloat side = planeEq.a*lightPosition[0]+
			planeEq.b*lightPosition[1]+
			planeEq.c*lightPosition[2]+
			planeEq.d;

		if ( side > 0 )
			planes[i].visible = true;
		else
			planes[i].visible = false;
	}

    glDisable( GL_LIGHTING );					// 关闭灯光
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );		// 关闭颜色缓存的写入	
	glDepthFunc( GL_LEQUAL );					// 设置深度比较函数
	glDepthMask( GL_FALSE );					// 关闭深度缓存的写入	
	glEnable( GL_STENCIL_TEST );				// 使用蒙板缓存
	glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFFL );	

    // 如果是逆时针（即面向视点）的多边形，通过了蒙板和深度测试，则把蒙板的值增加1
	glFrontFace( GL_CCW );
	glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
	doShadowPass( planes, lightPosition );
	// 如果是顺时针（即背向视点）的多边形，通过了蒙板和深度测试，则把蒙板的值减少1
	glFrontFace( GL_CW );
	glStencilOp( GL_KEEP, GL_KEEP, GL_DECR );
	doShadowPass( planes, lightPosition );

    glFrontFace( GL_CCW );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );	

	// 把阴影绘制上颜色
	glColor4f( 0.0f, 0.0f, 0.0f, 0.4f );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFFL );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	glPushMatrix();
	glLoadIdentity();
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f(-0.1f, 0.1f,-0.10f);
		glVertex3f(-0.1f,-0.1f,-0.10f);
		glVertex3f( 0.1f, 0.1f,-0.10f);
		glVertex3f( 0.1f,-0.1f,-0.10f);
	glEnd();
	glPopMatrix();
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
    if (height == 0)
    {
        height = 1;
    }
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    //指明接下来的两行代码将影响投影矩阵projection matrix
    //投影矩阵可以给场景增加透视
    glLoadIdentity();
    //重置投影矩阵
    //在此方法之后为我们的场景设置透视图
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    //设置基于窗口宽度和高度的45度角来观察
    // 0.1f是近平面的距离，100.0f是远平面的距离(在场景中所能绘制深度的起点和终点)
    glMatrixMode(GL_MODELVIEW);
    //表明任何新的变化将会影响modelview matrix(模式观察矩阵)
    //存放了物体讯息
    glLoadIdentity();
    //重置模型观察矩阵
}

void LoadTexture()
{
    //加载纹理
    if (texture[0].loadTexture("./Data/mud.bmp"))
    {
        //设置纹理属性
        //创建Nearest滤波贴图
        glGenTextures(1, &texture[0].id);
        glBindTexture(GL_TEXTURE_2D, texture[0].id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[0].width, texture[0].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[0].data);
    }
    if (texture[1].loadTexture("./Data/grass.bmp"))
    {
        //设置纹理属性
        //创建Linear滤波贴图
        glGenTextures(1, &texture[1].id);
        glBindTexture(GL_TEXTURE_2D, texture[1].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[1].width, texture[1].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[1].data);
    }
    if (texture[2].loadTexture("./Data/sky.bmp"))
    {
        //设置纹理属性
        //创建MipMapped滤波贴图
        glGenTextures(1, &texture[2].id);
        glBindTexture(GL_TEXTURE_2D, texture[2].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[2].width, texture[2].height, GL_RGB, GL_UNSIGNED_BYTE, texture[2].data);
    }
    if (texture[3].loadTexture("./Data/BG.bmp"))
    {
        //设置纹理属性
        //创建MipMapped滤波背景贴图
        glGenTextures(1, &texture[3].id);
        glBindTexture(GL_TEXTURE_2D, texture[3].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[3].width, texture[3].height, GL_RGB, GL_UNSIGNED_BYTE, texture[3].data);
    }
    if (texture[4].loadTexture("./Data/water.bmp"))
    {
        //设置纹理属性
        //创建MipMapped滤波水的贴图
        glGenTextures(1, &texture[4].id);
        glBindTexture(GL_TEXTURE_2D, texture[4].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[4].width, texture[4].height, GL_RGB, GL_UNSIGNED_BYTE, texture[4].data);
    }
    if (texture[5].loadTexture("./Data/sand.bmp"))
    {
        //设置纹理属性
        //创建MipMapped滤波沙子贴图
        glGenTextures(1, &texture[5].id);
        glBindTexture(GL_TEXTURE_2D, texture[5].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[5].width, texture[5].height, GL_RGB, GL_UNSIGNED_BYTE, texture[5].data);
    }
    if (texture[6].loadTexture("./Data/rocks.bmp"))
    {
        //设置纹理属性
        //试图创建MipMapped滤波岩石贴图
        glGenTextures(1, &texture[6].id);
        glBindTexture(GL_TEXTURE_2D, texture[6].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[6].width, texture[6].height, GL_RGB, GL_UNSIGNED_BYTE, texture[6].data);
    }
    if (texture[7].loadTexture("./Data/snow.bmp"))
    {
        //设置纹理属性
        //创建MipMapped滤波雪地贴图
        glGenTextures(1, &texture[7].id);
        glBindTexture(GL_TEXTURE_2D, texture[7].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[7].width, texture[7].height, GL_RGB, GL_UNSIGNED_BYTE, texture[7].data);
    }
    if (texture[8].loadTexture("./Data/moon.jpg"))
    {
        //设置纹理属性
        //创建MipMapped滤波月球贴图
        glGenTextures(1, &texture[8].id);
        glBindTexture(GL_TEXTURE_2D, texture[8].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[8].width, texture[8].height, GL_RGB, GL_UNSIGNED_BYTE, texture[8].data);
    }
    if (texture[9].loadTexture("./Data/star.jpg"))
    {
        //设置纹理属性
        //创建MipMapped滤波星星贴图
        glGenTextures(1, &texture[9].id);
        glBindTexture(GL_TEXTURE_2D, texture[9].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[9].width, texture[9].height, GL_RGB, GL_UNSIGNED_BYTE, texture[9].data);
    }
    // texture[10]空着,这样就是白色的了
}

void CreateWorld()
{
    //创建一个空的场景

    //底层
    glBindTexture(GL_TEXTURE_2D, texture[1].id);
    glBegin(GL_QUADS);
    //设置纹理坐标
    for (int i = 0; i < xlen - 1; i++)
    {
        for (int j = 0; j < ylen - 1; j++)
        {
            glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(meshtable[i][j][0], -worldsize, meshtable[i][j][1]);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(meshtable[i + 1][j][0], -worldsize, meshtable[i + 1][j][1]);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(meshtable[i + 1][j + 1][0], -worldsize, meshtable[i + 1][j + 1][1]);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(meshtable[i][j + 1][0], -worldsize, meshtable[i][j + 1][1]);
        }
    }
    glEnd();

    //天空
    glBindTexture(GL_TEXTURE_2D, texture[2].id);
    glBegin(GL_QUADS);
    for (int i = 0; i < xlen - 1; i++)
    {
        for (int j = 0; j < ylen - 1; j++)
        {
            glNormal3f(0.0, -1.0, 0.0);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(meshtable[i][j][0], worldsize, -meshtable[i][j][1]);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(meshtable[i + 1][j][0], worldsize, -meshtable[i + 1][j][1]);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(meshtable[i + 1][j + 1][0], worldsize, -meshtable[i + 1][j + 1][1]);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(meshtable[i][j + 1][0], worldsize, -meshtable[i][j + 1][1]);
        }
    }
    glEnd();

    //后面
    glBindTexture(GL_TEXTURE_2D, texture[3].id);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(worldsize, worldsize, -worldsize);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-worldsize, worldsize, -worldsize);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-worldsize, -worldsize, -worldsize);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(worldsize, -worldsize, -worldsize);
    glEnd();

    //前面
    glBindTexture(GL_TEXTURE_2D, texture[4].id);
    glBegin(GL_QUADS);
    for (int i = 0; i < xlen - 1; i++)
    {
        for (int j = 0; j < ylen - 1; j++)
        {
            glNormal3f(0.0, 0.0, -1.0);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(meshtable[i][j][0], meshtable[i][j][1], worldsize);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(meshtable[i + 1][j][0], meshtable[i + 1][j][1], worldsize);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(meshtable[i + 1][j + 1][0], meshtable[i + 1][j + 1][1], worldsize);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(meshtable[i][j + 1][0], meshtable[i][j + 1][1], worldsize);
        }
    }
    glEnd();

    //左面
    glBindTexture(GL_TEXTURE_2D, texture[5].id);
    glBegin(GL_QUADS);
    for (int i = 0; i < xlen - 1; i++)
    {
        for (int j = 0; j < ylen - 1; j++)
        {
            glNormal3f(1.0, 0.0, 0.0);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-worldsize, meshtable[i][j][1], meshtable[i][j][0]);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(-worldsize, meshtable[i + 1][j][1], meshtable[i + 1][j][0]);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(-worldsize, meshtable[i + 1][j + 1][1], meshtable[i + 1][j + 1][0]);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-worldsize, meshtable[i][j + 1][1], meshtable[i][j + 1][0]);
        }
    }
    glEnd();

    //右面
    glBindTexture(GL_TEXTURE_2D, texture[6].id);
    glBegin(GL_QUADS);
    for (int i = 0; i < xlen - 1; i++)
    {
        for (int j = 0; j < ylen - 1; j++)
        {
            glNormal3f(-1.0, 0.0, 0.0);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(worldsize, meshtable[i][j][1], -meshtable[i][j][0]);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(worldsize, meshtable[i + 1][j][1], -meshtable[i + 1][j][0]);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(worldsize, meshtable[i + 1][j + 1][1], -meshtable[i + 1][j + 1][0]);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(worldsize, meshtable[i][j + 1][1], -meshtable[i][j + 1][0]);
        }
    }
    glEnd();
}

//这里的类用于读取monkey.obj文件，专门针对monkey.obj文件的格式进行了设计
class ObjLoader
{
public:
    struct vertex
    {
        float x;
        float y;
        float z;
    };
    ObjLoader(string filename); //读取函数
    void Draw();                //绘制函数
private:
    vector<vector<GLfloat>> v; //存放顶点(x,y,z)坐标
    vector<vector<GLint>> f;   //存放面的顶点索引
    vector<sPlane> plane;//模型表面
};
ObjLoader::ObjLoader(string filename)
{
    ifstream file(filename);
    string line;
    while (getline(file, line))
    {
        if (line.substr(0, 1) == "v")
        {
            vector<GLfloat> Point;
            GLfloat x, y, z;
            istringstream s(line.substr(2));
            s >> x;
            s >> y;
            s >> z;
            Point.push_back(x);
            Point.push_back(y);
            Point.push_back(z);
            v.push_back(Point);
        }
        else if (line.substr(0, 1) == "f")
        {
            vector<GLint> vIndexSets;
            GLint u, v, w;
            istringstream vtns(line.substr(2));
            vtns >> u;
            vtns >> v;
            vtns >> w;
            vIndexSets.push_back(u - 1);
            vIndexSets.push_back(v - 1);
            vIndexSets.push_back(w - 1);
            f.push_back(vIndexSets);
        }
    }
    file.close();
}

void ObjLoader::Draw()
{
    int index = 0;
    monkeymodel = glGenLists(1);
    glNewList(monkeymodel, GL_COMPILE);
    glBegin(GL_TRIANGLES); //开始绘制
    for (int i = 0; i < f.size(); i++)
    {
        GLfloat VN[3]; //法线
        //三个顶点
        vertex a, b, c, normal;
        sPlane aPlane;

        if ((f[i]).size() != 3)
        {
            cout << "ERRER::THE SIZE OF f IS " << f[i].size() << endl;
            for (int j = 0; j < (f[i]).size(); j++)
            {
                cout << (f[i])[j] << " ";
            }
        }
        else
        {
            GLint firstVertexIndex = (f[i])[0]; //取出顶点索引
            GLint secondVertexIndex = (f[i])[1];
            GLint thirdVertexIndex = (f[i])[2];

            aPlane.indexp[0] = firstVertexIndex;
            aPlane.indexp[1] = secondVertexIndex;
            aPlane.indexp[2] = thirdVertexIndex;

            a.x = (v[firstVertexIndex])[0]; //第一个顶点
            a.y = (v[firstVertexIndex])[1];
            a.z = (v[firstVertexIndex])[2];

            b.x = (v[secondVertexIndex])[0]; //第二个顶点
            b.y = (v[secondVertexIndex])[1];
            b.z = (v[secondVertexIndex])[2];

            c.x = (v[thirdVertexIndex])[0]; //第三个顶点
            c.y = (v[thirdVertexIndex])[1];
            c.z = (v[thirdVertexIndex])[2];

            aPlane.p[0].x = a.x;
            aPlane.p[0].y = a.y;
            aPlane.p[0].z = a.z;

            aPlane.p[1].x = b.x;
            aPlane.p[1].y = b.y;
            aPlane.p[1].z = b.z;

            aPlane.p[2].x = c.x;
            aPlane.p[2].y = c.y;
            aPlane.p[2].z = c.z;

            GLfloat vec1[3], vec2[3], vec3[3]; //计算法向量
            //(x2-x1,y2-y1,z2-z1)
            vec1[0] = a.x - b.x;
            vec1[1] = a.y - b.y;
            vec1[2] = a.z - b.z;

            //(x3-x2,y3-y2,z3-z2)
            vec2[0] = a.x - c.x;
            vec2[1] = a.y - c.y;
            vec2[2] = a.z - c.z;

            //叉乘得到的结果
            vec3[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
            vec3[1] = vec2[0] * vec1[2] - vec2[2] * vec1[0];
            vec3[2] = vec2[1] * vec1[0] - vec2[0] * vec1[1];

            //计算法向量的模
            GLfloat D = sqrt(pow(vec3[0], 2) + pow(vec3[1], 2) + pow(vec3[2], 2));

            //进行法向量的归一化处理
            VN[0] = vec3[0] / D;
            VN[1] = vec3[1] / D;
            VN[2] = vec3[2] / D;

            aPlane.normals->x = VN[0];
            aPlane.normals->y = VN[1];
            aPlane.normals->z = VN[2];

            calculatePlane(aPlane);
            plane.push_back(aPlane);

            glNormal3f(VN[0], VN[1], VN[2]); //绘制法向量
            glTexCoord2f(a.x, b.y);
            glVertex3f(a.x, a.y, a.z); //绘制三角面
            glTexCoord2f(b.x, b.y);
            glVertex3f(b.x, b.y, b.z);
            glTexCoord2f(c.x, c.y);
            glVertex3f(c.x, c.y, c.z);
        }
    }
    glEnd();
    glEndList();
    shadow=glGenLists(1);
    glNewList(shadow,GL_COMPILE);
    SetConnectivity(plane);
    castShadow(plane, LightPosition);
    glEndList();
}

string filename = "./Data/monkey.obj";
ObjLoader monkey = ObjLoader(filename);

int InitGL(GLvoid)
{
    //初始化
    LoadTexture();                                    //加载纹理
    glEnable(GL_TEXTURE_2D);                          //启用纹理映射
    glShadeModel(GL_SMOOTH);                          //启用平滑着色
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);   //设置环境光参数
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);   //设置漫反射光参数
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition); //设置光源位置
    glEnable(GL_LIGHT1);                              //启用环境光

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f); //设置画布背景色

    //必须有的
    glClearDepth(1.0f);      //设置深度缓存
    glEnable(GL_DEPTH_TEST); //启用深度测试
    glDepthFunc(GL_LEQUAL);  //设置深度测试类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //真正的精细的透视修正

    monkey.Draw();
    glPolygonMode(GL_BACK, GL_FILL);  //后表面完全填充
    glPolygonMode(GL_FRONT, GL_FILL); //前表面完全填充
    //初始化网格信息（该网格用于绘制周围环境的表面）
    for (int x = 0; x < xlen; x++)
    {
        for (int y = 0; y < ylen; y++)
        {
            meshtable[x][y][0] = ((float(x) / (xlen - 1)) * 2 * worldsize - worldsize);
            meshtable[x][y][1] = ((float(y) / (ylen - 1)) * 2 * worldsize - worldsize);
        }
    }
    return TRUE;
}

int DrawGLScene(GLvoid)
{
    //所有的绘制图形的代码
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //清除颜色缓存和深度缓存,重置场景
    glLoadIdentity();
    //将当前点移动到屏幕中心
    GLfloat xtrans = -xpos;             //场景实际的x轴偏移量
    GLfloat ztrans = -zpos;             //场景实际的z轴偏移量
    GLfloat ytrans = -walkbias - 0.25f; //用于人物移动时镜头上下的移动
    GLfloat sceneroty = 360.0f - yrot;  //场景实际左右旋转的角度
    lookupdown = 360 - xrot;            //场景实际上下旋转的角度

    //相关变换矩阵
    glRotatef(lookupdown, 1.0f, 0, 0);
    glRotatef(sceneroty, 0, 1.0f, 0);
    glTranslatef(xtrans, ytrans, ztrans);
    //实际场景的上下移动
    glTranslatef(0.0f, -ypos, 0.0f);

    CreateWorld();
    glBindTexture(GL_TEXTURE_2D, texture[filter].id); //设置猴头的纹理
    glCallList(monkeymodel);
    //monkey.Draw();

    return TRUE;
}

GLvoid KillGLWindow(GLvoid)
{
    for (int i = length - 1; i >= 0; i--)
    {
        texture[i].freeTexture();
    }

    //依次释放着色描述表,设备描述表和窗口句柄
    if (fullscreen)
    {
        //回到桌面
        ChangeDisplaySettings(NULL, 0); //回到原始桌面
        ShowCursor(TRUE);
    }
    if (hRC)
    {
        //释放着色描述表
        if (!wglMakeCurrent(NULL, NULL))
        {
            //释放当前的绘图设备失败
            MessageBox(NULL, _T("释放DC或RC失败!"), _T("关闭错误"), MB_OK | MB_ICONINFORMATION);
        }
        if (!wglDeleteContext(hRC))
        {
            //释放着色描述表失败
            MessageBox(NULL, _T("释放RC失败!"), _T("关闭错误"), MB_OK | MB_ICONINFORMATION);
        }
        hRC = NULL;
    }
    if (hDC && !ReleaseDC(hWnd, hDC))
    {
        //释放设备描述表失败
        MessageBox(NULL, _T("释放DC失败!"), _T("关闭错误"), MB_OK | MB_ICONINFORMATION);
        hDC = NULL;
    }
    if (hWnd && !DestroyWindow(hWnd))
    {
        //释放窗口句柄失败
        MessageBox(NULL, _T("释放窗口句柄失败!"), _T("关闭错误"), MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;
    }
    if (!UnregisterClass(_T("OpenGL"), hInstance))
    {
        //释放类名失败
        MessageBox(NULL, _T("释放类名失败!"), _T("关闭错误"), MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;
    }
}

BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
    GLuint PixelFormat;               //像素格式描述符
    WNDCLASS wc;                      //窗口类结构体
    DWORD dwExStyle;                  //窗口扩展样式
    DWORD dwStyle;                    //窗口样式
    RECT WindowRect;                  //窗口矩形
    WindowRect.left = (long)0;        //左边位置
    WindowRect.right = (long)width;   //右边位置
    WindowRect.top = (long)0;         //上边位置
    WindowRect.bottom = (long)height; //下边位置
    fullscreen = fullscreenflag;      //是否全屏

    hInstance = GetModuleHandle(NULL);             //获取当前程序的句柄
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //窗口样式
    wc.lpfnWndProc = WndProc;                      //窗口回调函数
    wc.cbClsExtra = 0;                             //窗口类额外存储空间
    wc.cbWndExtra = 0;                             //窗口额外存储空间
    wc.hInstance = hInstance;                      //窗口实例句柄
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);        //窗口图标
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      //窗口鼠标光标
    // wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);//窗口背景色
    wc.hbrBackground = NULL;         //窗口背景色
    wc.lpszMenuName = NULL;          //窗口菜单名
    wc.lpszClassName = _T("OpenGL"); //窗口类名
    if (!RegisterClass(&wc))
    {
        //注册窗口类失败
        MessageBox(NULL, _T("注册窗口类失败!"), _T("错误"), MB_OK | MB_ICONINFORMATION);
        return FALSE;
    }
    if (fullscreen)
    {
        //全屏窗口
        DEVMODE dmScreenSettings;                                                 //屏幕设置结构体
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));                   //清空结构体
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);                       //结构体大小
        dmScreenSettings.dmPelsWidth = width;                                     //窗口宽度
        dmScreenSettings.dmPelsHeight = height;                                   //窗口高度
        dmScreenSettings.dmBitsPerPel = bits;                                     //像素深度
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; //标志位
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            //改变屏幕设置失败
            if (MessageBox(NULL, _T("改变屏幕设置失败，是否继续?"), _T("错误"), MB_YESNO | MB_ICONINFORMATION) == IDYES)
            {
                fullscreen = FALSE; //不是全屏窗口
            }
            else
            {
                MessageBox(NULL, _T("程序退出!"), _T("错误"), MB_OK | MB_ICONINFORMATION);
                return FALSE; //退出程序
            }
        }
    }
    if (fullscreen)
    {
        //全屏窗口
        dwExStyle = WS_EX_APPWINDOW; //窗口扩展样式
        dwStyle = WS_POPUP;          //窗口样式
        ShowCursor(FALSE);           //隐藏鼠标
    }
    else
    {
        //非全屏窗口
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //窗口扩展样式
        dwStyle = WS_OVERLAPPEDWINDOW;                  //窗口样式
    }
    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); //调整窗口位置
    if (!(hWnd = CreateWindowEx(dwExStyle,                      //扩展窗体风格
                                _T("OpenGL"),                   //类名
                                title,                          //窗口标题
                                WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                                0, 0, //窗口位置
                                WindowRect.right - WindowRect.left,
                                WindowRect.bottom - WindowRect.top,
                                NULL,      //无父窗口
                                NULL,      //无菜单
                                hInstance, //实例
                                NULL)))
    {
        //创建窗口失败
        KillGLWindow(); //重置显示区
        MessageBox(NULL, _T("不能创建一个窗口设备描述表"), _T("错误"), MB_OK | MB_ICONINFORMATION);
        return FALSE;
    }
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW |
            PFD_SUPPORT_OPENGL |
            PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        bits,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        16,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0};
    if (!(hDC = GetDC(hWnd)))
    {
        KillGLWindow();
        MessageBox(NULL, _T("不能创建一种相匹配的像素格式"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
    {
        KillGLWindow();
        MessageBox(NULL, _T("不能设置像素格式"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!SetPixelFormat(hDC, PixelFormat, &pfd))
    {
        KillGLWindow();
        MessageBox(NULL, _T("不能设置像素格式"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!(hRC = wglCreateContext(hDC)))
    {
        KillGLWindow();
        MessageBox(NULL, _T("不能创建OpenGL渲染描述表"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!wglMakeCurrent(hDC, hRC))
    {
        KillGLWindow();
        MessageBox(NULL, _T("不能激活当前的OpenGL渲染描述表"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    ShowWindow(hWnd, SW_SHOW);    //显示窗口
    SetForegroundWindow(hWnd);    //设置窗口为前景窗口
    SetFocus(hWnd);               //设置窗口为焦点窗口
    ReSizeGLScene(width, height); //设置窗口大小
    if (!InitGL())
    {
        KillGLWindow();
        MessageBox(NULL, _T("不能初始化OpenGL"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            active = FALSE;
        }
        else
        {
            active = TRUE;
        }
        return 0;
    }
    case WM_SYSCOMMAND:
    {
        switch (wParam)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            return 0;
        }
        break;
    }
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_KEYDOWN:
    {
        keys[wParam] = TRUE;
        return 0;
    }
    case WM_KEYUP:
    {
        keys[wParam] = FALSE;
        return 0;
    }
    case WM_SIZE:
    {
        ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam); //向DefWindowProc传递所有未处理的消息
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    BOOL done = FALSE;
    fullscreen = TRUE;
    if (MessageBox(NULL, _T("你想在全屏模式下运行吗？"), _T("设置全屏模式"), MB_YESNO | MB_ICONQUESTION) == IDNO)
    {
        fullscreen = FALSE;
    }

    if (!CreateGLWindow(window_title, 640, 480, 16, fullscreen))
    {
        return 0;
    }
    while (!done)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                done = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if (active)
            {
                if (keys[VK_ESCAPE])
                {
                    done = TRUE;
                }
                else
                {
                    DrawGLScene();
                    SwapBuffers(hDC);
                    if (keys[' '] && !sp)//空格键按下
                    {
                        sp = TRUE;
                        LightAmbient[0] = (float)rand() / RAND_MAX;
                        LightAmbient[1] = (float)rand() / RAND_MAX;
                        LightAmbient[2] = (float)rand() / RAND_MAX;
                        //设置随机的光线
                        glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
                        glEnable(GL_LIGHT1);
                    }
                    if (!keys[' '])
                    {
                        sp = FALSE;
                    }
                    if (keys['L'] && !lp)//同上
                    {
                        lp = TRUE;
                        light = !light;
                        if (light)
                        {
                            glEnable(GL_LIGHTING);
                        }
                        else
                        {
                            glDisable(GL_LIGHTING);
                        }
                    }
                    if (!keys['L'])
                    {
                        lp = FALSE;
                    }
                    if (keys['C'] && !cp)
                    {
                        cp = TRUE;
                        filter++;
                        if (filter > length - 1)
                        {
                            filter = 0;
                        }
                        printf("now filter=%d\n", filter);
                    }
                    if (!keys['C'])
                    {
                        cp = FALSE;
                    }
                    if (keys['F'])//不使用D键来做向右转是因为我电脑D键不灵敏，可以改成D就行
                    {
                        yrot -= 0.05;
                        if (yrot < 0.0)
                        {
                            yrot += 360.0;
                        }
                    }
                    if (keys[VK_PRIOR])//PageUp键
                    {
                        ypos += 0.01;
                    }
                    if (keys[VK_NEXT])//PageDown键
                    {
                        ypos -= 0.01;
                    }
                    if (keys[VK_UP])//方向键上箭头
                    {
                        xpos -= (float)sin(yrot * piover180) * 0.01f;
                        zpos -= (float)cos(yrot * piover180) * 0.01f;
                        if (walkbiasangle >= 359.0)
                        {
                            walkbiasangle = 0.0;
                        }
                        else
                        {
                            walkbiasangle += 1;
                        }
                        walkbias = (float)sin(walkbiasangle * piover180) / 20.0f;
                    }
                    if (keys[VK_DOWN])//方向键下箭头
                    {
                        xpos += (float)sin(yrot * piover180) * 0.01f;
                        zpos += (float)cos(yrot * piover180) * 0.01f;
                        if (walkbiasangle <= 1.0)
                        {
                            walkbiasangle = 359.0;
                        }
                        else
                        {
                            walkbiasangle -= 1;
                        }
                        walkbias = (float)sin(walkbiasangle * piover180) / 20.0f;
                    }
                    if (keys[VK_LEFT])//方向键左箭头
                    {
                        xpos -= 0.01f;
                        if (walkbiasangle >= 359.0)
                        {
                            walkbiasangle = 0.0;
                        }
                        else
                        {
                            walkbiasangle += 1;
                        }
                        walkbias = (float)sin(walkbiasangle * piover180) / 20.0f;
                    }
                    if (keys[VK_RIGHT])//方向键右箭头
                    {
                        xpos += 0.01f;
                        if (walkbiasangle <= 1.0)
                        {
                            walkbiasangle = 359.0;
                        }
                        else
                        {
                            walkbiasangle -= 1;
                        }
                        walkbias = (float)sin(walkbiasangle * piover180) / 20.0f;
                    }
                    if (keys['A'])
                    {
                        yrot += 0.05;
                        if (yrot > 360.0)
                        {
                            yrot -= 360.0;
                        }
                    }
                    if (keys['W'])
                    {
                        xrot += 0.05;
                        if (xrot > 360.0)
                        {
                            xrot -= 360.0;
                        }
                    }
                    if (keys['S'])
                    {
                        xrot -= 0.05;
                        if (xrot < 0.0)
                        {
                            xrot += 360.0;
                        }
                    }
                    if (keys['B'] && !bp)
                    {
                        bp = TRUE;
                        blend = !blend;
                        if (blend)
                        {
                            glEnable(GL_BLEND);
                        }
                        else
                        {
                            glDisable(GL_BLEND);
                        }
                    }
                    if (!keys['B'])
                    {
                        bp = FALSE;
                    }
                }
            }
            if (keys[VK_F1])
            {
                keys[VK_F1] = FALSE;
                KillGLWindow();
                fullscreen = !fullscreen;
                //重建OpenGL窗口
                if (!CreateGLWindow(window_title, 640, 480, 16, fullscreen))
                {
                    return 0;
                }
            }
        }
    }
    KillGLWindow();
    return (msg.wParam);
}