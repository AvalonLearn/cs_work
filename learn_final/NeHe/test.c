#include<stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int x, y, z;
    char type[5];
    sscanf("vt 10 10 10", "%s", type);
    printf("%s\n", type);
    strlen(type);
    char*token;
    token=strtok("vt 10 10 10", " ");
    sscanf("vt 10 10 10", "%s %f %f %f", type, &x, &y, &z);
    printf("%f %f %f\n", x, y, z);
    for (int i = 0;i<strlen(token);i++){
        printf("%c\n", token[i]);
    }
        return 0;
}

GLuint elephant;
float elephantrot;
void loadObj(char *fname)
{
    FILE *fp;
    int read;
    GLfloat x, y, z;
    char ch;
    elephant = glGenLists(1);
    fp = fopen(fname, "r");
    if (!fp)
    {
        printf("can't open file %s\n", fname);
        exit(1);
    }
//    glPointSize(2.0);
    glNewList(elephant, GL_COMPILE);
    {
        LoadTexture();
        glBindTexture(GL_TEXTURE_2D, texture[filter].id);
        glPushMatrix();
        glBegin(GL_POINTS);
        while (!(feof(fp)))
        {
            char line[255];
            fgets(line, 255, fp);
            //read = fscanf(fp, "%c %f %f %f", &ch, &x, &y, &z);
            char type[5];
            sscanf(line, "%s", &type, &x, &y, &z);
            if (read == 4 && ch == 'v')
            {
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
    glPopMatrix();
    glEndList();
    fclose(fp);
}


#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
typedef struct point{
    float x;
    float y;
    float z;
    point(float x,float y,float z){
        this->x=x;
        this->y=y;
        this->z=z;
    }
}point,normal;
typedef struct texcoord
{
    float u;
    float v;
    texcoord(float u,float v){
        this->u=u;
        this->v=v;
    }
}texcoord;
typedef struct ipoint{
    int x;
    int y;
    int z;
    ipoint(int x,int y,int z){
        this->x=x;
        this->y=y;
        this->z=z;
    }
}ipoint;


// ��ȡoff�ļ������������մ��ݸ�������ɫ���� ����λ�� / ������������ / ���㷨��
void readObj(
    std::string filepath, 
    std::vector<point>& points, 
    std::vector<texcoord>& texcoords,
    std::vector<normal>& normals
)
{
    // ��������
    std::vector<point> vectexPosition;
    std::vector<texcoord> vertexTexcoord;
    std::vector<normal> vectexNormal;

    // ��Ƭ������Ϣ
    std::vector<ipoint> positionIndex;
    std::vector<ipoint> texcoordIndex;
    std::vector<ipoint> normalIndex;

    // ���ļ���
    std::ifstream fin(filepath);
    std::string line;
    if (!fin.is_open())
    {
        std::cout << "�ļ� " << filepath << " ��ʧ��" << std::endl;
        exit(-1);
    }

    // ���ж�ȡ
    while (std::getline(fin, line))
    {
        std::istringstream sin(line);   // ��һ�е�������Ϊ string stream �������Ҷ�ȡ
        std::string type;
        GLfloat x, y, z;
        int v0, vt0, vn0;   // ��Ƭ�� 1 ������ġ�λ�ã��������꣬���ߡ�����
        int v1, vt1, vn1;   // 2
        int v2, vt2, vn2;   // 3
        char slash;

        // ��ȡobj�ļ�
        sin >> type;
        if (type == "v") {
            sin >> x >> y >> z;
            vectexPosition.push_back(point(x, y, z));
        }
        if (type == "vt") {
            sin >> x >> y;
            vertexTexcoord.push_back(texcoord(x, y));
        }
        if (type == "vn") {
            sin >> x >> y >> z;
            vectexNormal.push_back(normal(x, y, z));
        }
        if (type == "f") {
            sin >> v0 >> slash >> vt0 >> slash >> vn0;
            sin >> v1 >> slash >> vt1 >> slash >> vn1;
            sin >> v2 >> slash >> vt2 >> slash >> vn2;
            positionIndex.push_back(ipoint(v0 - 1, v1 - 1, v2 - 1));
            texcoordIndex.push_back(ipoint(vt0 - 1, vt1 - 1, vt2 - 1));
            normalIndex.push_back(ipoint(vn0 - 1, vn1 - 1, vn2 - 1));
        }
    }

    // ������Ƭ��Ϣ�������մ��붥����ɫ���Ķ�������
    for (int i = 0; i < positionIndex.size(); i++)
    {
        // ����λ��
        points.push_back(vectexPosition[positionIndex[i].x]);
        points.push_back(vectexPosition[positionIndex[i].y]);
        points.push_back(vectexPosition[positionIndex[i].z]);

        // ������������
        texcoords.push_back(vertexTexcoord[texcoordIndex[i].x]);
        texcoords.push_back(vertexTexcoord[texcoordIndex[i].y]);
        texcoords.push_back(vertexTexcoord[texcoordIndex[i].z]);

        // ���㷨��
        normals.push_back(vectexNormal[normalIndex[i].x]);
        normals.push_back(vectexNormal[normalIndex[i].y]);
        normals.push_back(vectexNormal[normalIndex[i].z]);
    }
}



GLuint elephant;
float elephantrot;
void loadObj(char *fname)
{
    FILE *fp;
    int read;
    GLfloat x, y, z;
    char ch;
    elephant = glGenLists(1);
    std::vector<point> points;
    std::vector<texcoord> texcoords;
    std::vector<normal> normals;
    readObj(fname, points, texcoords, normals);
    glNewList(elephant, GL_COMPILE);
    {
        LoadTexture();
        glBindTexture(GL_TEXTURE_2D, texture[filter].id);
        glBegin(GL_POINTS);
        for (int i = 0; i < points.size(); i++)
        {
            points
        }
        glEnd();
    }
    glEndList();
}

GLint u, v, w;
            stringstream vtns(line.substr(2));
            string str_tmp;
            for (int i = 0;i<3;i++)
            {
                getline(vtns, str_tmp, ' ');
                sscanf(str_tmp.c_str(), "%d/%d/%d", &u, &v, &w);
                printf("u=%d v=%d w=%d\n", u, v, w);
                vector<GLint> vIndexSets;
                vIndexSets.push_back(u - 1);
                vIndexSets.push_back(v - 1);
                vIndexSets.push_back(w - 1);
                printf("vIndexSet.size()=%d\n", vIndexSets.size());
                f.push_back(vIndexSets);
            }