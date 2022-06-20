#include <windows.h>
#include <GL/glut.h>
#include "GL/GLAUX.H"
#include <stdio.h>
#include <tchar.h>
#include "GLext.h"

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
int p1 = 0; //增量
int p2 = 1;

GLfloat LightAmbient[] = {0.8f, 0.8f, 0.8f, 1.0f};
//环境光参数
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
//漫反射光参数
GLfloat LightPosition[] = {5.0f, 0.0f, 0.0f, 1.0f};
//光源位置

GLuint filter; //纹理过滤器

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

const int length = 11;
Texture texture[length];
GLuint monkeymodel;
GLuint world;

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
        //试图创建Nearest滤波贴图
        glGenTextures(1, &texture[0].id);
        glBindTexture(GL_TEXTURE_2D, texture[0].id);
        // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[0].width, texture[0].height, GL_RGB, GL_UNSIGNED_BYTE, texture[0].data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[0].width, texture[0].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[0].data);
        // stbi_image_free(texture[0].data);
    }
    if (texture[1].loadTexture("./Data/grass.bmp"))
    {
        //设置纹理属性
        //试图创建Linear滤波贴图
        glGenTextures(1, &texture[1].id);
        glBindTexture(GL_TEXTURE_2D, texture[1].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[1].width, texture[1].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[1].data);
        // stbi_image_free(texture[0].data);
    }
    if (texture[2].loadTexture("./Data/sky.bmp"))
    {
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[2].id);
        glBindTexture(GL_TEXTURE_2D, texture[2].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[2].width, texture[2].height, GL_RGB, GL_UNSIGNED_BYTE, texture[2].data);
        // stbi_image_free(texture[0].data);
    }
    if (texture[3].loadTexture("./Data/BG.bmp"))
    {
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[3].id);
        glBindTexture(GL_TEXTURE_2D, texture[3].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[3].width, texture[3].height, GL_RGB, GL_UNSIGNED_BYTE, texture[3].data);
        // stbi_image_free(texture[0].data);
    }
    if (texture[4].loadTexture("./Data/water.bmp"))
    {
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[4].id);
        glBindTexture(GL_TEXTURE_2D, texture[4].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[4].width, texture[4].height, GL_RGB, GL_UNSIGNED_BYTE, texture[4].data);
        // stbi_image_free(texture[0].data);
    }
    if (texture[5].loadTexture("./Data/sand.bmp"))
    {
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[5].id);
        glBindTexture(GL_TEXTURE_2D, texture[5].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[5].width, texture[5].height, GL_RGB, GL_UNSIGNED_BYTE, texture[5].data);
        // stbi_image_free(texture[0].data);
    }
    if (texture[6].loadTexture("./Data/rocks.bmp"))
    {
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[6].id);
        glBindTexture(GL_TEXTURE_2D, texture[6].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[6].width, texture[6].height, GL_RGB, GL_UNSIGNED_BYTE, texture[6].data);
        // stbi_image_free(texture[0].data);
    }
    if (texture[7].loadTexture("./Data/snow.bmp"))
    {
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[7].id);
        glBindTexture(GL_TEXTURE_2D, texture[7].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[7].width, texture[7].height, GL_RGB, GL_UNSIGNED_BYTE, texture[7].data);
        // stbi_image_free(texture[0].data);
    }
    if(texture[8].loadTexture("./Data/moon.jpg")){
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[8].id);
        glBindTexture(GL_TEXTURE_2D, texture[8].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[8].width, texture[8].height, GL_RGB, GL_UNSIGNED_BYTE, texture[8].data);
        // stbi_image_free(texture[0].data);
    }
    if(texture[9].loadTexture("./Data/star.jpg")){
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[9].id);
        glBindTexture(GL_TEXTURE_2D, texture[9].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[9].width, texture[9].height, GL_RGB, GL_UNSIGNED_BYTE, texture[9].data);
        // stbi_image_free(texture[0].data);
    }
    //texture[10]空着,这样就是白色的了
}

void CreateWorld()
{
    //创建一个空的场景
    // world=glGenLists(1);
    // glNewList(world,GL_COMPILE);

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
    // glEndList();
}



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
    vector<vector<GLfloat>> v;  //存放顶点(x,y,z)坐标
    vector<vector<GLint>> f;    //存放面的顶点、纹理、法向量索引
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

            a.x = (v[firstVertexIndex])[0]; //第一个顶点
            a.y = (v[firstVertexIndex])[1];
            a.z = (v[firstVertexIndex])[2];

            b.x = (v[secondVertexIndex])[0]; //第二个顶点
            b.y = (v[secondVertexIndex])[1];
            b.z = (v[secondVertexIndex])[2];

            c.x = (v[thirdVertexIndex])[0]; //第三个顶点
            c.y = (v[thirdVertexIndex])[1];
            c.z = (v[thirdVertexIndex])[2];

            GLfloat vec1[3], vec2[3], vec3[3]; //计算法向量
            //(x2-x1,y2-y1,z2-z1)
            vec1[0] = a.x - b.x;
            vec1[1] = a.y - b.y;
            vec1[2] = a.z - b.z;

            //(x3-x2,y3-y2,z3-z2)
            vec2[0] = a.x - c.x;
            vec2[1] = a.y - c.y;
            vec2[2] = a.z - c.z;

            //(x3-x1,y3-y1,z3-z1)
            vec3[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
            vec3[1] = vec2[0] * vec1[2] - vec2[2] * vec1[0];
            vec3[2] = vec2[1] * vec1[0] - vec2[0] * vec1[1];

            GLfloat D = sqrt(pow(vec3[0], 2) + pow(vec3[1], 2) + pow(vec3[2], 2));

            VN[0] = vec3[0] / D;
            VN[1] = vec3[1] / D;
            VN[2] = vec3[2] / D;

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
}

string filename = "./Data/monkey.obj";
ObjLoader monkey = ObjLoader(filename);

int InitGL(GLvoid)
{
    //初始化
    LoadTexture();
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
    for (int x = 0; x < xlen; x++)
    {
        for (int y = 0; y < ylen; y++)
        {
            meshtable[x][y][0] = ((float(x) / (xlen - 1)) * 2 * worldsize - worldsize);
            meshtable[x][y][1] = ((float(y) / (ylen - 1)) * 2 * worldsize - worldsize);
            // printf("%f %f\n",meshtable[x][y][0],meshtable[x][y][1]);
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
    GLfloat xtrans = -xpos;
    GLfloat ztrans = -zpos;
    GLfloat ytrans = -walkbias - 0.25f;
    GLfloat sceneroty = 360.0f - yrot;
    lookupdown = 360 - xrot;

    glRotatef(lookupdown, 1.0f, 0, 0);
    glRotatef(sceneroty, 0, 1.0f, 0);
    glTranslatef(xtrans, ytrans, ztrans);
    glTranslatef(0.0f, -ypos, 0.0f);

    CreateWorld();
    // glCallList(world);
    glBindTexture(GL_TEXTURE_2D, texture[filter].id);
    glCallList(monkeymodel);

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
    fullscreen = FALSE;
    // if(MessageBox(NULL,_T("你想在全屏模式下运行吗？"),_T("设置全屏模式"),MB_YESNO|MB_ICONQUESTION)==IDNO){
    //     fullscreen=FALSE;
    // }
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
                    if (keys[' '] && !sp)
				{
					sp=TRUE;
                    LightAmbient[0] = (float)rand() / RAND_MAX;
                    LightAmbient[1] = (float)rand() / RAND_MAX;
                    LightAmbient[2] = (float)rand() / RAND_MAX;
                    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
                    glEnable(GL_LIGHT1);  
				}
				if (!keys[' '])
				{
					sp=FALSE;
				}
                    if (keys['L'] && !lp)
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
                    if (keys['C']&&!cp)
                    {
                        cp = TRUE;
                        filter++;
                        if(filter>length-1){
                            filter=0;
                        }
                        printf("now filter=%d\n",filter);
                    }
                    if(!keys['C']){
                        cp = FALSE;
                    }
                    if (keys['F'])
                    {
                        yrot -= 0.05;
                        if (yrot < 0.0)
                        {
                            yrot += 360.0;
                        }
                    }
                    if (keys[VK_PRIOR])
                    {
                        ypos += 0.01;
                    }
                    if (keys[VK_NEXT])
                    {
                        ypos -= 0.01;
                    }
                    if (keys[VK_UP])
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
                    if (keys[VK_DOWN])
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
                    if (keys[VK_LEFT])
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
                    if (keys[VK_RIGHT])
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