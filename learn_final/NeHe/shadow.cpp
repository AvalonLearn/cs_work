#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define PI  3.14159265359

GLdouble    fovy      = 60.0;
GLdouble    nearPlane = 10.0;
GLdouble    farPlane  = 100.0;

GLfloat     angle = 0.0;
GLfloat     torusAngle = 0.0;

GLfloat     lightPos[] = { 25.0, 25.0, 25.0, 1.0 };
GLfloat     lookat[] = { 0.0, 0.0, 0.0 };
GLfloat     up[] = { 0.0, 0.0, 1.0 };

GLboolean showShadow = GL_FALSE;

/*初始化光源与阴影图*/
void init( void )
{
    GLfloat  white[] = { 1.0, 1.0, 1.0, 1.0 };

    //生成深度纹理(阴影图，只关心深度，对图像颜色信息并不关心，所以最后数据参数可以是NULL)
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 256, 256, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );//GL_DEPTH_COMPONENT:深度纹理格式，用于将深度值记录到这张图中

    glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
    glLightfv( GL_LIGHT0, GL_SPECULAR, white );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, white );

    //设置阴影图相关过滤方式
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    /*深度纹理特有*/
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );//比较规则
    glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );//阴影图亮度
    /*分为普通对比模式（GL_NONE）和引用到贴图对比模式（GL_COMPARE_REF_TO_TEXTURE）,后者使用的深度纹理贴图是线性过滤的，而前者是直接填充。*/
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE );

    //自动生成纹理坐标
    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );

    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

    glCullFace( GL_BACK );//剔除背面

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHT0 );
    glEnable( GL_LIGHTING );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );
    glEnable( GL_TEXTURE_GEN_R );
    glEnable( GL_TEXTURE_GEN_Q );
    glEnable( GL_COLOR_MATERIAL );//激活颜色材料模式
    glEnable( GL_CULL_FACE );//激活表面剔除
}

void reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( fovy, (GLdouble) width/height, nearPlane, farPlane );
    glMatrixMode( GL_MODELVIEW );
}

void idle( void )
{
    angle += PI / 10000;
    torusAngle += .1;
    glutPostRedisplay();
}


void keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
        case 27:  /* Escape */
            exit( 0 );
            break;
          //开启与关闭纹理贴图
        case 't': {
            static GLboolean textureOn = GL_TRUE;
            textureOn = !textureOn;
            if ( textureOn )
                glEnable( GL_TEXTURE_2D );
            else
                glDisable( GL_TEXTURE_2D );
        }
            break;
        //深度纹理贴图的两种模式对比
        case 'm': {
            static GLboolean compareMode = GL_TRUE;
            compareMode = !compareMode;
            printf( "Compare mode %s\n", compareMode ? "On" : "Off" );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                            compareMode ? GL_COMPARE_R_TO_TEXTURE : GL_NONE );
        }
            break;
         //比较规则切换
        case 'f': {
            static GLboolean funcMode = GL_TRUE;
            funcMode = !funcMode;
            printf( "Operator %s\n", funcMode ? "GL_LEQUAL" : "GL_GEQUAL" );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,funcMode ? GL_LEQUAL : GL_GEQUAL );//当前片段z与阴影图z比较规则(<=, >=)
        }
            break;
        //是否显示阴影图
        case 's':
            showShadow = !showShadow;
            break;
         //运动与停止
        case 'p': {
            static GLboolean  animate = GL_TRUE;
            animate = !animate;
            glutIdleFunc( animate ? idle : NULL );
        }
            break;
    }

    glutPostRedisplay();
}


void transposeMatrix( GLfloat m[16] )
{
    GLfloat  tmp;
#define Swap( a, b )    tmp = a; a = b; b = tmp
    Swap( m[1],  m[4]  );
    Swap( m[2],  m[8]  );
    Swap( m[3],  m[12] );
    Swap( m[6],  m[9]  );
    Swap( m[7],  m[13] );
    Swap( m[11], m[14] );
#undef Swap
}

/*绘制场景上对象*/
void drawObjects( GLboolean shadowRender )
{
    GLboolean textureOn = glIsEnabled( GL_TEXTURE_2D );

    if ( shadowRender )
        glDisable( GL_TEXTURE_2D );

    //绘制矩形
    if ( !shadowRender ) {
        glNormal3f( 0, 0, 1 );
        glColor3f( 1, 1, 1 );
        glRectf( -20.0, -20.0, 20.0, 20.0 );
    }
    //红色圆环
    glPushMatrix();
    glTranslatef( 11, 11, 11 );
    glRotatef( 54.73, -5, 5, 0 );
    glRotatef( torusAngle, 1, 0, 0 );
    glColor3f( 1, 0, 0 );
    glutSolidTorus( 1, 4, 8, 36 );
    glPopMatrix();
    //蓝色立方体
    glPushMatrix();
    glTranslatef( 2, 2, 2 );
    glColor3f( 0, 0, 1 );
    glutSolidCube( 4 );
    glPopMatrix();
    //线框圆表光源
    glPushMatrix();
    glTranslatef( lightPos[0], lightPos[1], lightPos[2] );
    glColor3f( 1, 1, 1 );
    glutWireSphere( 0.5, 6, 6 );
    glPopMatrix();

    if ( shadowRender && textureOn )
        glEnable( GL_TEXTURE_2D );
}

/*生成阴影图*/
void generateShadowMap( void )
{
    GLint    viewport[4];
    GLfloat  lightPos[4];

    glGetLightfv( GL_LIGHT0, GL_POSITION, lightPos );//获得光源的位置
    glGetIntegerv( GL_VIEWPORT, viewport );//获得视口信息

    glViewport( 0, 0, 256, 256 );//设置视口与阴影图大小匹配

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluPerspective( 80.0, 1.0, 10.0, 1000.0 );
    glMatrixMode( GL_MODELVIEW );

    glPushMatrix();
    glLoadIdentity();
    gluLookAt( lightPos[0], lightPos[1], lightPos[2],
              lookat[0], lookat[1], lookat[2],
              up[0], up[1], up[2] );//把摄像机放到光源处

    drawObjects( GL_TRUE );//绘制场景

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 256, 256, 0 );//摄像机移到光源处渲染的阴影图

    glViewport( viewport[0], viewport[1], viewport[2], viewport[3] );//恢复当前视口

    //绘制阴影图
    if ( showShadow ) {
        GLfloat depthImage[256][256];
        glReadPixels( 0, 0, 256, 256,GL_DEPTH_COMPONENT, GL_FLOAT, depthImage );
        glWindowPos2f( viewport[2]/2, 0 );//设置阴影图绘制位置
        glDrawPixels( 256, 256, GL_LUMINANCE,
                     GL_FLOAT, depthImage );
        glutSwapBuffers();
    }
}

//自动生成阴影图的纹理坐标
void generateTextureMatrix( void )
{
    GLfloat  tmpMatrix[16];

    glPushMatrix();
    glLoadIdentity();
    glTranslatef( 0.5, 0.5, 0.0 );
    glScalef( 0.5, 0.5, 1.0 );
    gluPerspective( 60.0, 1.0, 1.0, 1000.0 );
    gluLookAt( lightPos[0], lightPos[1], lightPos[2],
              lookat[0], lookat[1], lookat[2],
              up[0], up[1], up[2] );
    glGetFloatv( GL_MODELVIEW_MATRIX, tmpMatrix );
    glPopMatrix();

    transposeMatrix( tmpMatrix );

    glTexGenfv( GL_S, GL_OBJECT_PLANE, &tmpMatrix[0] );
    glTexGenfv( GL_T, GL_OBJECT_PLANE, &tmpMatrix[4] );
    glTexGenfv( GL_R, GL_OBJECT_PLANE, &tmpMatrix[8] );
    glTexGenfv( GL_Q, GL_OBJECT_PLANE, &tmpMatrix[12] );
}

//渲染
void display( void )
{
    GLfloat  radius = 30;

    generateShadowMap();
    generateTextureMatrix();

    if ( showShadow )
        return;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();
    gluLookAt( radius*cos(angle), radius*sin(angle), 30,
              lookat[0], lookat[1], lookat[2],
              up[0], up[1], up[2] );
    drawObjects( GL_FALSE );
    glPopMatrix();

    glutSwapBuffers();
}

int main( int argc, char** argv )
{
     glWindowPos2f   = (PFNGLWINDOWPOS2FARBPROC)glutGetProcAddress("glWindowPos2f");

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( 512, 512 );
    glutInitWindowPosition( 100, 100 );
    glutCreateWindow( argv[0] );

    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );

    glutMainLoop();

    return 0;
}