#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLAUX.H>
#include "GLext.h"
#include <GL/glut.h>
#include <string.h>
#include <tchar.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define MAX_EMBOSS (GLfloat)0.01f//ͻ������ֵ
#define __ARB_ENABLE true//�Ƿ�ʹ��ARB��ʽ
//#define EXT_INFO
#define MAX_EXTENSION_SPACE 10240//��չ�ռ�
#define MAX_EXTENSION_LENGTH 256
BOOL multitextureSupported = false;
BOOL useMultitexture = true;
GLint maxTexelUnits = 1;

PFNGMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB = NULL;
PFNGMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
PFNGMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB = NULL;
PFNGMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

HGLRC hRC=NULL;//��ɫ������
HDC hDC=NULL;//�豸������
HWND hWnd=NULL;//���ھ��
HINSTANCE hInstance;//Ӧ�ó���ʵ��

bool keys[256];//����״̬
bool active=true;//����״̬
bool fullscreen = FALSE;//ȫ��״̬

char* window_title = "����ӳ��";
//��ת��
GLfloat xrot;
GLfloat yrot;
GLfloat zrot;

typedef struct Texture
{
    GLuint id;
    int width;
    int height;
    int nrChannels;
    unsigned char *data;
    int loadTexture(char* Filepath);
    void freeTexture();
}Texture;

int Texture::loadTexture(char* Filepath)
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

void Texture::freeTexture(){
    stbi_image_free(data);
}

const int texture_length = 3;
Texture texture[texture_length];
GLuint filter = 1;
Texture bump[texture_length];
Texture invbump[texture_length];
GLuint glLogo;
GLuint multiLogo;
GLfloat LightAmbient[]	= { 0.2f, 0.2f, 0.2f};					// ������
GLfloat LightDiffuse[]	= { 1.0f, 1.0f, 1.0f};					// �����
GLfloat LightPosition[]	= { 0.0f, 0.0f, 2.0f};					// ��Դλ��
GLfloat Gray[]		= { 0.5f, 0.5f, 0.5f, 1.0f};
GLfloat data[]={
    // ǰ��
	0.0f, 0.0f,		-1.0f, -1.0f, +1.0f,
	1.0f, 0.0f,		+1.0f, -1.0f, +1.0f,
	1.0f, 1.0f,		+1.0f, +1.0f, +1.0f,
	0.0f, 1.0f,		-1.0f, +1.0f, +1.0f,
	// ����
	1.0f, 0.0f,		-1.0f, -1.0f, -1.0f,
	1.0f, 1.0f,		-1.0f, +1.0f, -1.0f,
	0.0f, 1.0f,		+1.0f, +1.0f, -1.0f,
	0.0f, 0.0f,		+1.0f, -1.0f, -1.0f,
	// ����
	0.0f, 1.0f,		-1.0f, +1.0f, -1.0f,
	0.0f, 0.0f,		-1.0f, +1.0f, +1.0f,
	1.0f, 0.0f,		+1.0f, +1.0f, +1.0f,
	1.0f, 1.0f,		+1.0f, +1.0f, -1.0f,
	// ����
	1.0f, 1.0f,		-1.0f, -1.0f, -1.0f,
	0.0f, 1.0f,		+1.0f, -1.0f, -1.0f,
	0.0f, 0.0f,		+1.0f, -1.0f, +1.0f,
	1.0f, 0.0f,		-1.0f, -1.0f, +1.0f,
	// ����
	1.0f, 0.0f,		+1.0f, -1.0f, -1.0f,
	1.0f, 1.0f,		+1.0f, +1.0f, -1.0f,
	0.0f, 1.0f,		+1.0f, +1.0f, +1.0f,
	0.0f, 0.0f,		+1.0f, -1.0f, +1.0f,
	// ����
	0.0f, 0.0f,		-1.0f, -1.0f, -1.0f,
	1.0f, 0.0f,		-1.0f, -1.0f, +1.0f,
	1.0f, 1.0f,		-1.0f, +1.0f, +1.0f,
	0.0f, 1.0f,		-1.0f, +1.0f, -1.0f
};

bool isInString(char *string, const char *search) {
	int pos=0;
	int maxpos=strlen(search)-1;
	int len=strlen(string);
	char *other;
	for (int i=0; i<len; i++) {
		if ((i==0) || ((i>1) && string[i-1]=='\n')) {				// �µ���չ����ʼ������
			other=&string[i];
			pos=0;							// ��ʼ�µıȽ�
			while (string[i]!='\n') {					// �Ƚ�������չ��
				if (string[i]==search[pos]) pos++;			// ��һ���ַ�
				if ((pos>maxpos) && string[i+1]=='\n') return true;	// ���������չ����ͬ��ɹ�����
				i++;
			}
		}
	}
	return false;								// û�ҵ�
}

bool initMultitexture(void) {
	char *extensions;
	extensions=strdup((char *) glGetString(GL_EXTENSIONS));			// ������չ���ַ���
	int len=strlen(extensions);
	for (int i=0; i<len; i++)							// ʹ��'\n'�ָ������չ��
		if (extensions[i]==' ') extensions[i]='\n';

#ifdef EXT_INFO
	MessageBox(hWnd,extensions,"supported GL extensions",MB_OK | MB_ICONINFORMATION);
#endif

	if (isInString(extensions,"GL_ARB_multitexture")				// �Ƿ�֧�ֶ���������չ��
		&& __ARB_ENABLE							// �Ƿ�ʹ�ö���������չ��
		&& isInString(extensions,"GL_EXT_texture_env_combine"))		// �Ƿ�֧�����������
	{
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&maxTexelUnits);
		glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

#ifdef EXT_INFO
		MessageBox(hWnd,"The GL_ARB_multitexture ��չ��ʹ��.","֧�ֶ�������",MB_OK | MB_ICONINFORMATION);
#endif

		return true;
	}
	useMultitexture=false;							// �����֧�ֶ��������򷵻�false
	return false;
}

void initLights(void) {
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);				
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GLvoid ReSizeGLScene(GLsizei width, GLsizei height){
    if(height==0){
        height = 1;
    }
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    //ָ�������������д��뽫Ӱ��ͶӰ����projection matrix
    //ͶӰ������Ը���������͸��
    glLoadIdentity();
    //����ͶӰ����
    //�ڴ˷���֮��Ϊ���ǵĳ�������͸��ͼ
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    //���û��ڴ��ڿ�Ⱥ͸߶ȵ�45�Ƚ����۲�
    //0.1f�ǽ�ƽ��ľ��룬100.0f��Զƽ��ľ���(�ڳ��������ܻ�����ȵ������յ�)
    glMatrixMode(GL_MODELVIEW);
    //�����κ��µı仯����Ӱ��modelview matrix(ģʽ�۲����)
    //���������ѶϢ
    glLoadIdentity();
    //����ģ�͹۲����
}

void LoadTexture(){
    //��������
    if(texture[0].loadTexture("./Data/Base.bmp")){
        //������������
        glGenTextures(1, &texture[0].id);
        glBindTexture(GL_TEXTURE_2D, texture[0].id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture[0].width, texture[0].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[0].data);
    }
    if(texture[1].loadTexture("./Data/Base.bmp")){
        glGenTextures(1, &texture[1].id);
        glBindTexture(GL_TEXTURE_2D, texture[1].id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture[1].width, texture[1].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[1].data);
    }
    if(texture[2].loadTexture("./Data/Base.bmp")){
        glGenTextures(1, &texture[2].id);
        glBindTexture(GL_TEXTURE_2D, texture[2].id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture[2].width, texture[2].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[2].data);
    }
    if(bump[0].loadTexture("./Data/Base.bmp")){
        glPixelTransferf(GL_RED_SCALE,0.5f);					// ����ɫֵ��Ϊԭ����50%
        glPixelTransferf(GL_GREEN_SCALE,0.5f);				
	    glPixelTransferf(GL_BLUE_SCALE,0.5f);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);		//��ʹ���ظ���ͼ
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glGenTextures(1, &bump[0].id);
        glBindTexture(GL_TEXTURE_2D, bump[0].id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, bump[0].width, bump[0].height, 0, GL_RGB, GL_UNSIGNED_BYTE, bump[0].data);
    }
}

void doCube (void) {
	int i;
	glBegin(GL_QUADS);
		// ǰ��
		glNormal3f( 0.0f, 0.0f, +1.0f);
		for (i=0; i<4; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// ����
		glNormal3f( 0.0f, 0.0f,-1.0f);
		for (i=4; i<8; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// ����
		glNormal3f( 0.0f, 1.0f, 0.0f);
		for (i=8; i<12; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// ����
		glNormal3f( 0.0f,-1.0f, 0.0f);
		for (i=12; i<16; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// ����
		glNormal3f( 1.0f, 0.0f, 0.0f);
		for (i=16; i<20; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// ����
		glNormal3f(-1.0f, 0.0f, 0.0f);
		for (i=20; i<24; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
	glEnd();
}

    int InitGL(GLvoid)
    {
        //��ʼ��
        LoadTexture();
        glEnable(GL_TEXTURE_2D); //��������ӳ��
        glShadeModel(GL_SMOOTH); //����ƽ����ɫ

        glClearColor(0.0f, 0.0f, 0.0f, 0.5f); //���û�������ɫ

        //�����е�
        glClearDepth(1.0f);      //������Ȼ���
        glEnable(GL_DEPTH_TEST); //������Ȳ���
        glDepthFunc(GL_LEQUAL);  //������Ȳ�������
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        //�����ľ�ϸ��͸������
        multitextureSupported = initMultitexture();
        initLights();
        return TRUE;
        }

int DrawGLScene(GLvoid){
    //���еĻ���ͼ�εĴ���
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //�����ɫ�������Ȼ���,���ó���
    glLoadIdentity();
    //����ǰ���ƶ�����Ļ����
    glTranslatef(0.0f, 0.0f, -5.0f);
    glBindTexture(GL_TEXTURE_2D, bump[0].id);
    doCube();
    xrot += 0.01f;
    yrot += 0.02f;
    zrot += 0.03f;
    return TRUE;
}

GLvoid KillGLWindow(GLvoid){
    texture[0].freeTexture();
    //�����ͷ���ɫ������,�豸������ʹ��ھ��
    if(fullscreen){
        //�ص�����
        ChangeDisplaySettings(NULL,0);//�ص�ԭʼ����
        ShowCursor(TRUE);
    }
    if(hRC){
        //�ͷ���ɫ������
        if(!wglMakeCurrent(NULL,NULL)){
            //�ͷŵ�ǰ�Ļ�ͼ�豸ʧ��
            MessageBox(NULL,_T("�ͷ�DC��RCʧ��!"),_T("�رմ���"),MB_OK | MB_ICONINFORMATION);
        }
        if(!wglDeleteContext(hRC)){
            //�ͷ���ɫ������ʧ��
            MessageBox(NULL,_T("�ͷ�RCʧ��!"),_T("�رմ���"),MB_OK | MB_ICONINFORMATION);
        }
        hRC=NULL;
    }
    if(hDC && !ReleaseDC(hWnd,hDC)){
        //�ͷ��豸������ʧ��
        MessageBox(NULL,_T("�ͷ�DCʧ��!"),_T("�رմ���"),MB_OK | MB_ICONINFORMATION);
        hDC = NULL;
    }
    if(hWnd && !DestroyWindow(hWnd)){
        //�ͷŴ��ھ��ʧ��
        MessageBox(NULL,_T("�ͷŴ��ھ��ʧ��!"),_T("�رմ���"),MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;
    }
    if(!UnregisterClass(_T("OpenGL"),hInstance)){
        //�ͷ�����ʧ��
        MessageBox(NULL,_T("�ͷ�����ʧ��!"),_T("�رմ���"),MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;
    }
}

BOOL CreateGLWindow(char* title,int width,int height,int bits,bool fullscreenflag){
    GLuint PixelFormat;//���ظ�ʽ������
    WNDCLASS wc;//������ṹ��
    DWORD dwExStyle;//������չ��ʽ
    DWORD dwStyle;//������ʽ
    RECT WindowRect;//���ھ���
    WindowRect.left=(long)0;//���λ��
    WindowRect.right=(long)width;//�ұ�λ��
    WindowRect.top=(long)0;//�ϱ�λ��
    WindowRect.bottom=(long)height;//�±�λ��
    fullscreen=fullscreenflag;//�Ƿ�ȫ��

    hInstance=GetModuleHandle(NULL);//��ȡ��ǰ����ľ��
    wc.style=CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//������ʽ
    wc.lpfnWndProc=WndProc;//���ڻص�����
    wc.cbClsExtra=0;//���������洢�ռ�
    wc.cbWndExtra=0;//���ڶ���洢�ռ�
    wc.hInstance=hInstance;//����ʵ�����
    wc.hIcon=LoadIcon(NULL,IDI_WINLOGO);//����ͼ��
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);//���������
    //wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);//���ڱ���ɫ
    wc.hbrBackground = NULL;//���ڱ���ɫ
    wc.lpszMenuName=NULL;//���ڲ˵���
    wc.lpszClassName=_T("OpenGL");//��������
    if(!RegisterClass(&wc)){
        //ע�ᴰ����ʧ��
        MessageBox(NULL,_T("ע�ᴰ����ʧ��!"),_T("����"),MB_OK | MB_ICONINFORMATION);
        return FALSE;
    }
    if(fullscreen){
        //ȫ������
        DEVMODE dmScreenSettings;//��Ļ���ýṹ��
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));//��սṹ��
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);//�ṹ���С
        dmScreenSettings.dmPelsWidth=width;//���ڿ��
        dmScreenSettings.dmPelsHeight=height;//���ڸ߶�
        dmScreenSettings.dmBitsPerPel=bits;//�������
        dmScreenSettings.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;//��־λ
        if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL){
            //�ı���Ļ����ʧ��
            if(MessageBox(NULL,_T("�ı���Ļ����ʧ�ܣ��Ƿ����?"),_T("����"),MB_YESNO | MB_ICONINFORMATION)==IDYES){
                fullscreen=FALSE;//����ȫ������
            }else{
                MessageBox(NULL,_T("�����˳�!"),_T("����"),MB_OK | MB_ICONINFORMATION);
                return FALSE;//�˳�����
            }
        }
    }
    if(fullscreen){
        //ȫ������
        dwExStyle=WS_EX_APPWINDOW;//������չ��ʽ
        dwStyle=WS_POPUP;//������ʽ
        ShowCursor(FALSE);//�������
    }else{
        //��ȫ������
        dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;//������չ��ʽ
        dwStyle=WS_OVERLAPPEDWINDOW;//������ʽ
    }
    AdjustWindowRectEx(&WindowRect,dwStyle,FALSE,dwExStyle);//��������λ��
    if(!(hWnd=CreateWindowEx(dwExStyle,//��չ������
    _T("OpenGL"),//����
    title,//���ڱ���
    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
    0,0,//����λ��
    WindowRect.right-WindowRect.left,
    WindowRect.bottom-WindowRect.top,
    NULL,//�޸�����
    NULL,//�޲˵�
    hInstance,//ʵ��
    NULL))){
        //��������ʧ��
        KillGLWindow();//������ʾ��
        MessageBox(NULL,_T("���ܴ���һ�������豸������"),_T("����"),MB_OK | MB_ICONINFORMATION);
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
    if(!(hDC=GetDC(hWnd))){
        KillGLWindow();
        MessageBox(NULL, _T("���ܴ���һ����ƥ������ظ�ʽ"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd))){
        KillGLWindow();
        MessageBox(NULL, _T("�����������ظ�ʽ"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if(!SetPixelFormat(hDC,PixelFormat,&pfd)){
	    KillGLWindow();
	    MessageBox(NULL,_T("�����������ظ�ʽ"),_T("����"),MB_OK|MB_ICONEXCLAMATION);
	    return FALSE;
    }
    if (!(hRC = wglCreateContext(hDC))){
        KillGLWindow();
        MessageBox(NULL,_T("���ܴ���OpenGL��Ⱦ������"),_T("����"),MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
    if(!wglMakeCurrent(hDC,hRC)){
        KillGLWindow();
        MessageBox(NULL,_T("���ܼ��ǰ��OpenGL��Ⱦ������"),_T("����"),MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
    ShowWindow(hWnd,SW_SHOW);//��ʾ����
    SetForegroundWindow(hWnd);//���ô���Ϊǰ������
    SetFocus(hWnd);//���ô���Ϊ���㴰��
    ReSizeGLScene(width,height);//���ô��ڴ�С
    if(!InitGL()){
        KillGLWindow();
        MessageBox(NULL,_T("���ܳ�ʼ��OpenGL"),_T("����"),MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
    switch(uMsg){
        case WM_ACTIVATE:{
            if(LOWORD(wParam)==WA_INACTIVE){
                active=FALSE;
            }else{
                active=TRUE;
            }
            return 0;
        }
        case WM_SYSCOMMAND:{
            switch(wParam){
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    return 0;
            }
            break;
        }
        case WM_CLOSE:{
            PostQuitMessage(0);
            return 0;
        }
        case WM_KEYDOWN:{
            keys[wParam]=TRUE;
            return 0;
        }
        case WM_KEYUP:{
            keys[wParam]=FALSE;
            return 0;
        }
        case WM_SIZE:{
            ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
            return 0;
        }
    }
    return DefWindowProc(hWnd,uMsg,wParam,lParam);//��DefWindowProc��������δ�������Ϣ
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
    MSG msg;
    BOOL done=FALSE;
    fullscreen = FALSE;
    // if(MessageBox(NULL,_T("������ȫ��ģʽ��������"),_T("����ȫ��ģʽ"),MB_YESNO|MB_ICONQUESTION)==IDNO){
    //     fullscreen=FALSE;
    // }
    if(!CreateGLWindow(window_title,640,480,16,fullscreen)){
        return 0;
    }
    while(!done){
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            if(msg.message==WM_QUIT){
                done=TRUE;
            }else{
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }else{
            if(active){
                if(keys[VK_ESCAPE]){
                    done=TRUE;
                }else{
                    DrawGLScene();
                    SwapBuffers(hDC);
                }
            }
            if(keys[VK_F1]){
                keys[VK_F1]=FALSE;
                KillGLWindow();
                fullscreen=!fullscreen;
                //�ؽ�OpenGL����
                if(!CreateGLWindow(window_title,640,480,16,fullscreen)){
                    return 0;
                }
            }
        }
    }
    KillGLWindow();
    return (msg.wParam);
}