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

HGLRC hRC = NULL;    //��ɫ������
HDC hDC = NULL;      //�豸������
HWND hWnd = NULL;    //���ھ��
HINSTANCE hInstance; //Ӧ�ó���ʵ��

bool keys[256];          //����״̬
bool active = true;      //����״̬
bool fullscreen = FALSE; //ȫ��״̬

char *window_title = "��ͷ��ĩʵ��";
//��ת��
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
BOOL blend; //�Ƿ���
BOOL bp;    // B������
BOOL cp;    // C������
BOOL lp;    // L������
BOOL sp;    //�ո���Ƿ���
int part1;  //Բ�̵���ʼ�Ƕ�
int part2;  //Բ�̵Ľ����Ƕ�
int p1 = 0; //����
int p2 = 1;

GLfloat LightAmbient[] = {0.8f, 0.8f, 0.8f, 1.0f};
//���������
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
//����������
GLfloat LightPosition[] = {5.0f, 0.0f, 0.0f, 1.0f};
//��Դλ��

GLuint filter; //���������

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
    //ָ�������������д��뽫Ӱ��ͶӰ����projection matrix
    //ͶӰ������Ը���������͸��
    glLoadIdentity();
    //����ͶӰ����
    //�ڴ˷���֮��Ϊ���ǵĳ�������͸��ͼ
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    //���û��ڴ��ڿ�Ⱥ͸߶ȵ�45�Ƚ����۲�
    // 0.1f�ǽ�ƽ��ľ��룬100.0f��Զƽ��ľ���(�ڳ��������ܻ�����ȵ������յ�)
    glMatrixMode(GL_MODELVIEW);
    //�����κ��µı仯����Ӱ��modelview matrix(ģʽ�۲����)
    //���������ѶϢ
    glLoadIdentity();
    //����ģ�͹۲����
}

void LoadTexture()
{
    //��������
    if (texture[0].loadTexture("./Data/mud.bmp"))
    {
        //������������
        //��ͼ����Nearest�˲���ͼ
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
        //������������
        //��ͼ����Linear�˲���ͼ
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
        //������������
        //��ͼ����MipMapped�˲���ͼ
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
        //������������
        //��ͼ����MipMapped�˲���ͼ
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
        //������������
        //��ͼ����MipMapped�˲���ͼ
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
        //������������
        //��ͼ����MipMapped�˲���ͼ
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
        //������������
        //��ͼ����MipMapped�˲���ͼ
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
        //������������
        //��ͼ����MipMapped�˲���ͼ
        glGenTextures(1, &texture[7].id);
        glBindTexture(GL_TEXTURE_2D, texture[7].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[7].width, texture[7].height, GL_RGB, GL_UNSIGNED_BYTE, texture[7].data);
        // stbi_image_free(texture[0].data);
    }
    if(texture[8].loadTexture("./Data/moon.jpg")){
        //������������
        //��ͼ����MipMapped�˲���ͼ
        glGenTextures(1, &texture[8].id);
        glBindTexture(GL_TEXTURE_2D, texture[8].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[8].width, texture[8].height, GL_RGB, GL_UNSIGNED_BYTE, texture[8].data);
        // stbi_image_free(texture[0].data);
    }
    if(texture[9].loadTexture("./Data/star.jpg")){
        //������������
        //��ͼ����MipMapped�˲���ͼ
        glGenTextures(1, &texture[9].id);
        glBindTexture(GL_TEXTURE_2D, texture[9].id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[9].width, texture[9].height, GL_RGB, GL_UNSIGNED_BYTE, texture[9].data);
        // stbi_image_free(texture[0].data);
    }
    //texture[10]����,�������ǰ�ɫ����
}

void CreateWorld()
{
    //����һ���յĳ���
    // world=glGenLists(1);
    // glNewList(world,GL_COMPILE);

    //�ײ�
    glBindTexture(GL_TEXTURE_2D, texture[1].id);
    glBegin(GL_QUADS);
    //������������
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

    //���
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

    //����
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

    //ǰ��
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

    //����
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

    //����
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
    ObjLoader(string filename); //��ȡ����
    void Draw();                //���ƺ���
private:
    vector<vector<GLfloat>> v;  //��Ŷ���(x,y,z)����
    vector<vector<GLint>> f;    //�����Ķ��㡢��������������
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
    glBegin(GL_TRIANGLES); //��ʼ����
    for (int i = 0; i < f.size(); i++)
    {
        GLfloat VN[3]; //����
        //��������
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
            GLint firstVertexIndex = (f[i])[0]; //ȡ����������
            GLint secondVertexIndex = (f[i])[1];
            GLint thirdVertexIndex = (f[i])[2];

            a.x = (v[firstVertexIndex])[0]; //��һ������
            a.y = (v[firstVertexIndex])[1];
            a.z = (v[firstVertexIndex])[2];

            b.x = (v[secondVertexIndex])[0]; //�ڶ�������
            b.y = (v[secondVertexIndex])[1];
            b.z = (v[secondVertexIndex])[2];

            c.x = (v[thirdVertexIndex])[0]; //����������
            c.y = (v[thirdVertexIndex])[1];
            c.z = (v[thirdVertexIndex])[2];

            GLfloat vec1[3], vec2[3], vec3[3]; //���㷨����
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

            glNormal3f(VN[0], VN[1], VN[2]); //���Ʒ�����
            glTexCoord2f(a.x, b.y);
            glVertex3f(a.x, a.y, a.z); //����������
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
    //��ʼ��
    LoadTexture();
    glEnable(GL_TEXTURE_2D);                          //��������ӳ��
    glShadeModel(GL_SMOOTH);                          //����ƽ����ɫ
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);   //���û��������
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);   //��������������
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition); //���ù�Դλ��
    glEnable(GL_LIGHT1);                              //���û�����

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f); //���û�������ɫ

    //�����е�
    glClearDepth(1.0f);      //������Ȼ���
    glEnable(GL_DEPTH_TEST); //������Ȳ���
    glDepthFunc(GL_LEQUAL);  //������Ȳ�������
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //�����ľ�ϸ��͸������

    monkey.Draw();
    glPolygonMode(GL_BACK, GL_FILL);  //�������ȫ���
    glPolygonMode(GL_FRONT, GL_FILL); //ǰ������ȫ���
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
    //���еĻ���ͼ�εĴ���
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //�����ɫ�������Ȼ���,���ó���
    glLoadIdentity();
    //����ǰ���ƶ�����Ļ����
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

    //�����ͷ���ɫ������,�豸������ʹ��ھ��
    if (fullscreen)
    {
        //�ص�����
        ChangeDisplaySettings(NULL, 0); //�ص�ԭʼ����
        ShowCursor(TRUE);
    }
    if (hRC)
    {
        //�ͷ���ɫ������
        if (!wglMakeCurrent(NULL, NULL))
        {
            //�ͷŵ�ǰ�Ļ�ͼ�豸ʧ��
            MessageBox(NULL, _T("�ͷ�DC��RCʧ��!"), _T("�رմ���"), MB_OK | MB_ICONINFORMATION);
        }
        if (!wglDeleteContext(hRC))
        {
            //�ͷ���ɫ������ʧ��
            MessageBox(NULL, _T("�ͷ�RCʧ��!"), _T("�رմ���"), MB_OK | MB_ICONINFORMATION);
        }
        hRC = NULL;
    }
    if (hDC && !ReleaseDC(hWnd, hDC))
    {
        //�ͷ��豸������ʧ��
        MessageBox(NULL, _T("�ͷ�DCʧ��!"), _T("�رմ���"), MB_OK | MB_ICONINFORMATION);
        hDC = NULL;
    }
    if (hWnd && !DestroyWindow(hWnd))
    {
        //�ͷŴ��ھ��ʧ��
        MessageBox(NULL, _T("�ͷŴ��ھ��ʧ��!"), _T("�رմ���"), MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;
    }
    if (!UnregisterClass(_T("OpenGL"), hInstance))
    {
        //�ͷ�����ʧ��
        MessageBox(NULL, _T("�ͷ�����ʧ��!"), _T("�رմ���"), MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;
    }
}

BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
    GLuint PixelFormat;               //���ظ�ʽ������
    WNDCLASS wc;                      //������ṹ��
    DWORD dwExStyle;                  //������չ��ʽ
    DWORD dwStyle;                    //������ʽ
    RECT WindowRect;                  //���ھ���
    WindowRect.left = (long)0;        //���λ��
    WindowRect.right = (long)width;   //�ұ�λ��
    WindowRect.top = (long)0;         //�ϱ�λ��
    WindowRect.bottom = (long)height; //�±�λ��
    fullscreen = fullscreenflag;      //�Ƿ�ȫ��

    hInstance = GetModuleHandle(NULL);             //��ȡ��ǰ����ľ��
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //������ʽ
    wc.lpfnWndProc = WndProc;                      //���ڻص�����
    wc.cbClsExtra = 0;                             //���������洢�ռ�
    wc.cbWndExtra = 0;                             //���ڶ���洢�ռ�
    wc.hInstance = hInstance;                      //����ʵ�����
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);        //����ͼ��
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      //���������
    // wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);//���ڱ���ɫ
    wc.hbrBackground = NULL;         //���ڱ���ɫ
    wc.lpszMenuName = NULL;          //���ڲ˵���
    wc.lpszClassName = _T("OpenGL"); //��������
    if (!RegisterClass(&wc))
    {
        //ע�ᴰ����ʧ��
        MessageBox(NULL, _T("ע�ᴰ����ʧ��!"), _T("����"), MB_OK | MB_ICONINFORMATION);
        return FALSE;
    }
    if (fullscreen)
    {
        //ȫ������
        DEVMODE dmScreenSettings;                                                 //��Ļ���ýṹ��
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));                   //��սṹ��
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);                       //�ṹ���С
        dmScreenSettings.dmPelsWidth = width;                                     //���ڿ��
        dmScreenSettings.dmPelsHeight = height;                                   //���ڸ߶�
        dmScreenSettings.dmBitsPerPel = bits;                                     //�������
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; //��־λ
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            //�ı���Ļ����ʧ��
            if (MessageBox(NULL, _T("�ı���Ļ����ʧ�ܣ��Ƿ����?"), _T("����"), MB_YESNO | MB_ICONINFORMATION) == IDYES)
            {
                fullscreen = FALSE; //����ȫ������
            }
            else
            {
                MessageBox(NULL, _T("�����˳�!"), _T("����"), MB_OK | MB_ICONINFORMATION);
                return FALSE; //�˳�����
            }
        }
    }
    if (fullscreen)
    {
        //ȫ������
        dwExStyle = WS_EX_APPWINDOW; //������չ��ʽ
        dwStyle = WS_POPUP;          //������ʽ
        ShowCursor(FALSE);           //�������
    }
    else
    {
        //��ȫ������
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //������չ��ʽ
        dwStyle = WS_OVERLAPPEDWINDOW;                  //������ʽ
    }
    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); //��������λ��
    if (!(hWnd = CreateWindowEx(dwExStyle,                      //��չ������
                                _T("OpenGL"),                   //����
                                title,                          //���ڱ���
                                WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                                0, 0, //����λ��
                                WindowRect.right - WindowRect.left,
                                WindowRect.bottom - WindowRect.top,
                                NULL,      //�޸�����
                                NULL,      //�޲˵�
                                hInstance, //ʵ��
                                NULL)))
    {
        //��������ʧ��
        KillGLWindow(); //������ʾ��
        MessageBox(NULL, _T("���ܴ���һ�������豸������"), _T("����"), MB_OK | MB_ICONINFORMATION);
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
        MessageBox(NULL, _T("���ܴ���һ����ƥ������ظ�ʽ"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
    {
        KillGLWindow();
        MessageBox(NULL, _T("�����������ظ�ʽ"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!SetPixelFormat(hDC, PixelFormat, &pfd))
    {
        KillGLWindow();
        MessageBox(NULL, _T("�����������ظ�ʽ"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!(hRC = wglCreateContext(hDC)))
    {
        KillGLWindow();
        MessageBox(NULL, _T("���ܴ���OpenGL��Ⱦ������"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (!wglMakeCurrent(hDC, hRC))
    {
        KillGLWindow();
        MessageBox(NULL, _T("���ܼ��ǰ��OpenGL��Ⱦ������"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    ShowWindow(hWnd, SW_SHOW);    //��ʾ����
    SetForegroundWindow(hWnd);    //���ô���Ϊǰ������
    SetFocus(hWnd);               //���ô���Ϊ���㴰��
    ReSizeGLScene(width, height); //���ô��ڴ�С
    if (!InitGL())
    {
        KillGLWindow();
        MessageBox(NULL, _T("���ܳ�ʼ��OpenGL"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
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
    return DefWindowProc(hWnd, uMsg, wParam, lParam); //��DefWindowProc��������δ�������Ϣ
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    BOOL done = FALSE;
    fullscreen = FALSE;
    // if(MessageBox(NULL,_T("������ȫ��ģʽ��������"),_T("����ȫ��ģʽ"),MB_YESNO|MB_ICONQUESTION)==IDNO){
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
                //�ؽ�OpenGL����
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