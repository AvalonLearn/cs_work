#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>

GLsizei winWidth=500,winHeight=500;
char sixel;
float theta=0;
float x=0,y=0,z=0;

void init(){
    glClearColor(1.0,1.0,1.0,0.0);
}

void displayWirePolyhedra(float x,float y,float z,float theta){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0,0.0,1.0);
    gluLookAt(5.0,5.0,5.0,0.0,0.0,0.0,0.0,1.0,0.0);
    glScalef(1.0,1.0,1.0);
    glTranslatef(1.0,2.0,0.0);
    glutSolidTeapot(1.5);
    glScalef(1.0,1.0,1.0);
    glTranslatef(-1.0,-5.0,0.0);
    glRotatef(theta,x,y,z);
    glutWireTeapot(2.0);
    glFlush();
}

void display(){
    displayWirePolyhedra(x,y,z,theta);
}

void winReshapeFcn(GLint newWidth,GLint newHeight){
    glViewport(0,0,newWidth,newHeight);
    glMatrixMode(GL_PROJECTION);
    glFrustum(-1.0,1.0,-1.0,1.0,2.0,20.0);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(400,150);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("Polyhedra");
    
    init();

    printf("请选择绕哪一个轴旋转x,y,z\n");
    scanf("%c",&sixel);
    getchar();
    if(sixel=='x'){
        x=1;
        y=0;
        z=0;
        printf("请输入旋转的角度\n");
        scanf("%f",&theta);
    }else if(sixel=='y'){
        x=0;
        y=1;
        z=0;
        printf("请输入旋转的角度\n");
        scanf("%f",&theta);
    }else if(sixel=='z'){
        x=0;
        y=0;
        z=1;
        printf("请输入旋转的角度\n");
        scanf("%f",&theta);
    }

    glutDisplayFunc(display);
    glutReshapeFunc(winReshapeFcn);
    glutMainLoop();
    return 0;
}