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

/*��ʼ����Դ����Ӱͼ*/
void init( void )
{
    GLfloat  white[] = { 1.0, 1.0, 1.0, 1.0 };

    //�����������(��Ӱͼ��ֻ������ȣ���ͼ����ɫ��Ϣ�������ģ�����������ݲ���������NULL)
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 256, 256, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );//GL_DEPTH_COMPONENT:��������ʽ�����ڽ����ֵ��¼������ͼ��

    glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
    glLightfv( GL_LIGHT0, GL_SPECULAR, white );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, white );

    //������Ӱͼ��ع��˷�ʽ
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    /*�����������*/
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );//�ȽϹ���
    glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );//��Ӱͼ����
    /*��Ϊ��ͨ�Ա�ģʽ��GL_NONE�������õ���ͼ�Ա�ģʽ��GL_COMPARE_REF_TO_TEXTURE��,����ʹ�õ����������ͼ�����Թ��˵ģ���ǰ����ֱ����䡣*/
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE );

    //�Զ�������������
    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );

    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

    glCullFace( GL_BACK );//�޳�����

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHT0 );
    glEnable( GL_LIGHTING );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );
    glEnable( GL_TEXTURE_GEN_R );
    glEnable( GL_TEXTURE_GEN_Q );
    glEnable( GL_COLOR_MATERIAL );//������ɫ����ģʽ
    glEnable( GL_CULL_FACE );//��������޳�
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
          //������ر�������ͼ
        case 't': {
            static GLboolean textureOn = GL_TRUE;
            textureOn = !textureOn;
            if ( textureOn )
                glEnable( GL_TEXTURE_2D );
            else
                glDisable( GL_TEXTURE_2D );
        }
            break;
        //���������ͼ������ģʽ�Ա�
        case 'm': {
            static GLboolean compareMode = GL_TRUE;
            compareMode = !compareMode;
            printf( "Compare mode %s\n", compareMode ? "On" : "Off" );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                            compareMode ? GL_COMPARE_R_TO_TEXTURE : GL_NONE );
        }
            break;
         //�ȽϹ����л�
        case 'f': {
            static GLboolean funcMode = GL_TRUE;
            funcMode = !funcMode;
            printf( "Operator %s\n", funcMode ? "GL_LEQUAL" : "GL_GEQUAL" );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,funcMode ? GL_LEQUAL : GL_GEQUAL );//��ǰƬ��z����Ӱͼz�ȽϹ���(<=, >=)
        }
            break;
        //�Ƿ���ʾ��Ӱͼ
        case 's':
            showShadow = !showShadow;
            break;
         //�˶���ֹͣ
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

/*���Ƴ����϶���*/
void drawObjects( GLboolean shadowRender )
{
    GLboolean textureOn = glIsEnabled( GL_TEXTURE_2D );

    if ( shadowRender )
        glDisable( GL_TEXTURE_2D );

    //���ƾ���
    if ( !shadowRender ) {
        glNormal3f( 0, 0, 1 );
        glColor3f( 1, 1, 1 );
        glRectf( -20.0, -20.0, 20.0, 20.0 );
    }
    //��ɫԲ��
    glPushMatrix();
    glTranslatef( 11, 11, 11 );
    glRotatef( 54.73, -5, 5, 0 );
    glRotatef( torusAngle, 1, 0, 0 );
    glColor3f( 1, 0, 0 );
    glutSolidTorus( 1, 4, 8, 36 );
    glPopMatrix();
    //��ɫ������
    glPushMatrix();
    glTranslatef( 2, 2, 2 );
    glColor3f( 0, 0, 1 );
    glutSolidCube( 4 );
    glPopMatrix();
    //�߿�Բ���Դ
    glPushMatrix();
    glTranslatef( lightPos[0], lightPos[1], lightPos[2] );
    glColor3f( 1, 1, 1 );
    glutWireSphere( 0.5, 6, 6 );
    glPopMatrix();

    if ( shadowRender && textureOn )
        glEnable( GL_TEXTURE_2D );
}

/*������Ӱͼ*/
void generateShadowMap( void )
{
    GLint    viewport[4];
    GLfloat  lightPos[4];

    glGetLightfv( GL_LIGHT0, GL_POSITION, lightPos );//��ù�Դ��λ��
    glGetIntegerv( GL_VIEWPORT, viewport );//����ӿ���Ϣ

    glViewport( 0, 0, 256, 256 );//�����ӿ�����Ӱͼ��Сƥ��

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
              up[0], up[1], up[2] );//��������ŵ���Դ��

    drawObjects( GL_TRUE );//���Ƴ���

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 256, 256, 0 );//������Ƶ���Դ����Ⱦ����Ӱͼ

    glViewport( viewport[0], viewport[1], viewport[2], viewport[3] );//�ָ���ǰ�ӿ�

    //������Ӱͼ
    if ( showShadow ) {
        GLfloat depthImage[256][256];
        glReadPixels( 0, 0, 256, 256,GL_DEPTH_COMPONENT, GL_FLOAT, depthImage );
        glWindowPos2f( viewport[2]/2, 0 );//������Ӱͼ����λ��
        glDrawPixels( 256, 256, GL_LUMINANCE,
                     GL_FLOAT, depthImage );
        glutSwapBuffers();
    }
}

//�Զ�������Ӱͼ����������
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

//��Ⱦ
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