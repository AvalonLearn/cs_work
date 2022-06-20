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

#define MAX_EMBOSS (GLfloat)0.01f//突起的最大值
#define __ARB_ENABLE true//是否使用ARB方式
//#define EXT_INFO
#define MAX_EXTENSION_SPACE 10240//扩展空间
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

HGLRC hRC=NULL;//着色描述表
HDC hDC=NULL;//设备描述表
HWND hWnd=NULL;//窗口句柄
HINSTANCE hInstance;//应用程序实例

bool keys[256];//按键状态
bool active=true;//激活状态
bool fullscreen = FALSE;//全屏状态

char* window_title = "纹理映射";
//旋转用
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
GLfloat LightAmbient[]	= { 0.2f, 0.2f, 0.2f};					// 环境光
GLfloat LightDiffuse[]	= { 1.0f, 1.0f, 1.0f};					// 漫射光
GLfloat LightPosition[]	= { 0.0f, 0.0f, 2.0f};					// 光源位置
GLfloat Gray[]		= { 0.5f, 0.5f, 0.5f, 1.0f};
GLfloat data[]={
    // 前面
	0.0f, 0.0f,		-1.0f, -1.0f, +1.0f,
	1.0f, 0.0f,		+1.0f, -1.0f, +1.0f,
	1.0f, 1.0f,		+1.0f, +1.0f, +1.0f,
	0.0f, 1.0f,		-1.0f, +1.0f, +1.0f,
	// 背面
	1.0f, 0.0f,		-1.0f, -1.0f, -1.0f,
	1.0f, 1.0f,		-1.0f, +1.0f, -1.0f,
	0.0f, 1.0f,		+1.0f, +1.0f, -1.0f,
	0.0f, 0.0f,		+1.0f, -1.0f, -1.0f,
	// 上面
	0.0f, 1.0f,		-1.0f, +1.0f, -1.0f,
	0.0f, 0.0f,		-1.0f, +1.0f, +1.0f,
	1.0f, 0.0f,		+1.0f, +1.0f, +1.0f,
	1.0f, 1.0f,		+1.0f, +1.0f, -1.0f,
	// 下面
	1.0f, 1.0f,		-1.0f, -1.0f, -1.0f,
	0.0f, 1.0f,		+1.0f, -1.0f, -1.0f,
	0.0f, 0.0f,		+1.0f, -1.0f, +1.0f,
	1.0f, 0.0f,		-1.0f, -1.0f, +1.0f,
	// 右面
	1.0f, 0.0f,		+1.0f, -1.0f, -1.0f,
	1.0f, 1.0f,		+1.0f, +1.0f, -1.0f,
	0.0f, 1.0f,		+1.0f, +1.0f, +1.0f,
	0.0f, 0.0f,		+1.0f, -1.0f, +1.0f,
	// 左面
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
		if ((i==0) || ((i>1) && string[i-1]=='\n')) {				// 新的扩展名开始与这里
			other=&string[i];
			pos=0;							// 开始新的比较
			while (string[i]!='\n') {					// 比较整个扩展名
				if (string[i]==search[pos]) pos++;			// 下一个字符
				if ((pos>maxpos) && string[i+1]=='\n') return true;	// 如果整个扩展名相同则成功返回
				i++;
			}
		}
	}
	return false;								// 没找到
}

bool initMultitexture(void) {
	char *extensions;
	extensions=strdup((char *) glGetString(GL_EXTENSIONS));			// 返回扩展名字符串
	int len=strlen(extensions);
	for (int i=0; i<len; i++)							// 使用'\n'分割各个扩展名
		if (extensions[i]==' ') extensions[i]='\n';

#ifdef EXT_INFO
	MessageBox(hWnd,extensions,"supported GL extensions",MB_OK | MB_ICONINFORMATION);
#endif

	if (isInString(extensions,"GL_ARB_multitexture")				// 是否支持多重纹理扩展？
		&& __ARB_ENABLE							// 是否使用多重纹理扩展？
		&& isInString(extensions,"GL_EXT_texture_env_combine"))		// 是否支持纹理环境混合
	{
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&maxTexelUnits);
		glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

#ifdef EXT_INFO
		MessageBox(hWnd,"The GL_ARB_multitexture 扩展被使用.","支持多重纹理",MB_OK | MB_ICONINFORMATION);
#endif

		return true;
	}
	useMultitexture=false;							// 如果不支持多重纹理则返回false
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
    //指明接下来的两行代码将影响投影矩阵projection matrix
    //投影矩阵可以给场景增加透视
    glLoadIdentity();
    //重置投影矩阵
    //在此方法之后为我们的场景设置透视图
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    //设置基于窗口宽度和高度的45度角来观察
    //0.1f是近平面的距离，100.0f是远平面的距离(在场景中所能绘制深度的起点和终点)
    glMatrixMode(GL_MODELVIEW);
    //表明任何新的变化将会影响modelview matrix(模式观察矩阵)
    //存放了物体讯息
    glLoadIdentity();
    //重置模型观察矩阵
}

void LoadTexture(){
    //加载纹理
    if(texture[0].loadTexture("./Data/Base.bmp")){
        //设置纹理属性
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
        glPixelTransferf(GL_RED_SCALE,0.5f);					// 把颜色值变为原来的50%
        glPixelTransferf(GL_GREEN_SCALE,0.5f);				
	    glPixelTransferf(GL_BLUE_SCALE,0.5f);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);		//不使用重复贴图
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glGenTextures(1, &bump[0].id);
        glBindTexture(GL_TEXTURE_2D, bump[0].id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, bump[0].width, bump[0].height, 0, GL_RGB, GL_UNSIGNED_BYTE, bump[0].data);
    }
}

void doCube (void) {
	int i;
	glBegin(GL_QUADS);
		// 前面
		glNormal3f( 0.0f, 0.0f, +1.0f);
		for (i=0; i<4; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// 后面
		glNormal3f( 0.0f, 0.0f,-1.0f);
		for (i=4; i<8; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// 上面
		glNormal3f( 0.0f, 1.0f, 0.0f);
		for (i=8; i<12; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// 下面
		glNormal3f( 0.0f,-1.0f, 0.0f);
		for (i=12; i<16; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// 右面
		glNormal3f( 1.0f, 0.0f, 0.0f);
		for (i=16; i<20; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
		// 左面
		glNormal3f(-1.0f, 0.0f, 0.0f);
		for (i=20; i<24; i++) {
			glTexCoord2f(data[5*i],data[5*i+1]);
			glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
		}
	glEnd();
}

    int InitGL(GLvoid)
    {
        //初始化
        LoadTexture();
        glEnable(GL_TEXTURE_2D); //启用纹理映射
        glShadeModel(GL_SMOOTH); //启用平滑着色

        glClearColor(0.0f, 0.0f, 0.0f, 0.5f); //设置画布背景色

        //必须有的
        glClearDepth(1.0f);      //设置深度缓存
        glEnable(GL_DEPTH_TEST); //启用深度测试
        glDepthFunc(GL_LEQUAL);  //设置深度测试类型
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        //真正的精细的透视修正
        multitextureSupported = initMultitexture();
        initLights();
        return TRUE;
        }

int DrawGLScene(GLvoid){
    //所有的绘制图形的代码
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //清除颜色缓存和深度缓存,重置场景
    glLoadIdentity();
    //将当前点移动到屏幕中心
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
    //依次释放着色描述表,设备描述表和窗口句柄
    if(fullscreen){
        //回到桌面
        ChangeDisplaySettings(NULL,0);//回到原始桌面
        ShowCursor(TRUE);
    }
    if(hRC){
        //释放着色描述表
        if(!wglMakeCurrent(NULL,NULL)){
            //释放当前的绘图设备失败
            MessageBox(NULL,_T("释放DC或RC失败!"),_T("关闭错误"),MB_OK | MB_ICONINFORMATION);
        }
        if(!wglDeleteContext(hRC)){
            //释放着色描述表失败
            MessageBox(NULL,_T("释放RC失败!"),_T("关闭错误"),MB_OK | MB_ICONINFORMATION);
        }
        hRC=NULL;
    }
    if(hDC && !ReleaseDC(hWnd,hDC)){
        //释放设备描述表失败
        MessageBox(NULL,_T("释放DC失败!"),_T("关闭错误"),MB_OK | MB_ICONINFORMATION);
        hDC = NULL;
    }
    if(hWnd && !DestroyWindow(hWnd)){
        //释放窗口句柄失败
        MessageBox(NULL,_T("释放窗口句柄失败!"),_T("关闭错误"),MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;
    }
    if(!UnregisterClass(_T("OpenGL"),hInstance)){
        //释放类名失败
        MessageBox(NULL,_T("释放类名失败!"),_T("关闭错误"),MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;
    }
}

BOOL CreateGLWindow(char* title,int width,int height,int bits,bool fullscreenflag){
    GLuint PixelFormat;//像素格式描述符
    WNDCLASS wc;//窗口类结构体
    DWORD dwExStyle;//窗口扩展样式
    DWORD dwStyle;//窗口样式
    RECT WindowRect;//窗口矩形
    WindowRect.left=(long)0;//左边位置
    WindowRect.right=(long)width;//右边位置
    WindowRect.top=(long)0;//上边位置
    WindowRect.bottom=(long)height;//下边位置
    fullscreen=fullscreenflag;//是否全屏

    hInstance=GetModuleHandle(NULL);//获取当前程序的句柄
    wc.style=CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//窗口样式
    wc.lpfnWndProc=WndProc;//窗口回调函数
    wc.cbClsExtra=0;//窗口类额外存储空间
    wc.cbWndExtra=0;//窗口额外存储空间
    wc.hInstance=hInstance;//窗口实例句柄
    wc.hIcon=LoadIcon(NULL,IDI_WINLOGO);//窗口图标
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);//窗口鼠标光标
    //wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);//窗口背景色
    wc.hbrBackground = NULL;//窗口背景色
    wc.lpszMenuName=NULL;//窗口菜单名
    wc.lpszClassName=_T("OpenGL");//窗口类名
    if(!RegisterClass(&wc)){
        //注册窗口类失败
        MessageBox(NULL,_T("注册窗口类失败!"),_T("错误"),MB_OK | MB_ICONINFORMATION);
        return FALSE;
    }
    if(fullscreen){
        //全屏窗口
        DEVMODE dmScreenSettings;//屏幕设置结构体
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));//清空结构体
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);//结构体大小
        dmScreenSettings.dmPelsWidth=width;//窗口宽度
        dmScreenSettings.dmPelsHeight=height;//窗口高度
        dmScreenSettings.dmBitsPerPel=bits;//像素深度
        dmScreenSettings.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;//标志位
        if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL){
            //改变屏幕设置失败
            if(MessageBox(NULL,_T("改变屏幕设置失败，是否继续?"),_T("错误"),MB_YESNO | MB_ICONINFORMATION)==IDYES){
                fullscreen=FALSE;//不是全屏窗口
            }else{
                MessageBox(NULL,_T("程序退出!"),_T("错误"),MB_OK | MB_ICONINFORMATION);
                return FALSE;//退出程序
            }
        }
    }
    if(fullscreen){
        //全屏窗口
        dwExStyle=WS_EX_APPWINDOW;//窗口扩展样式
        dwStyle=WS_POPUP;//窗口样式
        ShowCursor(FALSE);//隐藏鼠标
    }else{
        //非全屏窗口
        dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;//窗口扩展样式
        dwStyle=WS_OVERLAPPEDWINDOW;//窗口样式
    }
    AdjustWindowRectEx(&WindowRect,dwStyle,FALSE,dwExStyle);//调整窗口位置
    if(!(hWnd=CreateWindowEx(dwExStyle,//扩展窗体风格
    _T("OpenGL"),//类名
    title,//窗口标题
    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
    0,0,//窗口位置
    WindowRect.right-WindowRect.left,
    WindowRect.bottom-WindowRect.top,
    NULL,//无父窗口
    NULL,//无菜单
    hInstance,//实例
    NULL))){
        //创建窗口失败
        KillGLWindow();//重置显示区
        MessageBox(NULL,_T("不能创建一个窗口设备描述表"),_T("错误"),MB_OK | MB_ICONINFORMATION);
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
        MessageBox(NULL, _T("不能创建一种相匹配的像素格式"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd))){
        KillGLWindow();
        MessageBox(NULL, _T("不能设置像素格式"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if(!SetPixelFormat(hDC,PixelFormat,&pfd)){
	    KillGLWindow();
	    MessageBox(NULL,_T("不能设置像素格式"),_T("错误"),MB_OK|MB_ICONEXCLAMATION);
	    return FALSE;
    }
    if (!(hRC = wglCreateContext(hDC))){
        KillGLWindow();
        MessageBox(NULL,_T("不能创建OpenGL渲染描述表"),_T("错误"),MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
    if(!wglMakeCurrent(hDC,hRC)){
        KillGLWindow();
        MessageBox(NULL,_T("不能激活当前的OpenGL渲染描述表"),_T("错误"),MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
    ShowWindow(hWnd,SW_SHOW);//显示窗口
    SetForegroundWindow(hWnd);//设置窗口为前景窗口
    SetFocus(hWnd);//设置窗口为焦点窗口
    ReSizeGLScene(width,height);//设置窗口大小
    if(!InitGL()){
        KillGLWindow();
        MessageBox(NULL,_T("不能初始化OpenGL"),_T("错误"),MB_OK|MB_ICONEXCLAMATION);
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
    return DefWindowProc(hWnd,uMsg,wParam,lParam);//向DefWindowProc传递所有未处理的消息
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
    MSG msg;
    BOOL done=FALSE;
    fullscreen = FALSE;
    // if(MessageBox(NULL,_T("你想在全屏模式下运行吗？"),_T("设置全屏模式"),MB_YESNO|MB_ICONQUESTION)==IDNO){
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
                //重建OpenGL窗口
                if(!CreateGLWindow(window_title,640,480,16,fullscreen)){
                    return 0;
                }
            }
        }
    }
    KillGLWindow();
    return (msg.wParam);
}