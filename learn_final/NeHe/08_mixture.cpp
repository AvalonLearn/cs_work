#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <tchar.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

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
GLfloat z = -5.0f;

BOOL light;
BOOL blend;//是否混合
BOOL bp;//B键按下
BOOL lp;//L键按下
BOOL fp;//F键按下

GLfloat LightAmbient[]={0.8f,0.8f,0.8f,1.0f};
//环境光参数
GLfloat LightDiffuse[]={1.0f,1.0f,1.0f,1.0f};
//漫反射光参数
GLfloat LightPosition[]={0.0f,0.0f,-1.0f,1.0f};
//光源位置

GLuint filter;//纹理过滤器
const int length = 4;//纹理个数

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

Texture texture[length];

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
    if(texture[0].loadTexture("./Data/container.jpg")){
        //设置纹理属性
        //试图创建Nearest滤波贴图
        glGenTextures(1, &texture[0].id);
        glBindTexture(GL_TEXTURE_2D, texture[0].id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[0].width, texture[0].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[0].data);
        //stbi_image_free(texture[0].data);
    }
    if(texture[1].loadTexture("./Data/container.jpg")){
        //设置纹理属性
        //试图创建Linear滤波贴图
        glGenTextures(1, &texture[1].id);
        glBindTexture(GL_TEXTURE_2D, texture[1].id);
        glColor4f(0.0f,1.0f,1.0f,0.5f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[1].width, texture[1].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[1].data);
        //stbi_image_free(texture[0].data);
    }
    if(texture[2].loadTexture("./Data/container.jpg")){
        //设置纹理属性
        //试图创建MipMapped滤波贴图
        glGenTextures(1, &texture[2].id);
        glBindTexture(GL_TEXTURE_2D, texture[2].id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D,3,texture[2].width,texture[2].height,GL_RGB,GL_UNSIGNED_BYTE,texture[2].data);
        //stbi_image_free(texture[0].data);
    }
    if(texture[3].loadTexture("./Data/glass.jpg")){
        //设置纹理属性
        glGenTextures(1, &texture[3].id);
        glBindTexture(GL_TEXTURE_2D, texture[3].id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[3].width, texture[3].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[3].data);
    }
}

int InitGL(GLvoid){
    //初始化
    LoadTexture();
    glEnable(GL_TEXTURE_2D);//启用纹理映射
    glShadeModel(GL_SMOOTH);//启用平滑着色
    glLightfv(GL_LIGHT1,GL_AMBIENT,LightAmbient);//设置环境光参数
    glLightfv(GL_LIGHT1,GL_DIFFUSE,LightDiffuse);//设置漫反射光参数
    glLightfv(GL_LIGHT1,GL_POSITION,LightPosition);//设置光源位置
    glEnable(GL_LIGHT1);//启用环境光

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);//设置画布背景色

    //必须有的
    glClearDepth(1.0f);//设置深度缓存
    glEnable(GL_DEPTH_TEST);//启用深度测试
    glDepthFunc(GL_LEQUAL);//设置深度测试类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //真正的精细的透视修正
    return TRUE;
}

int DrawGLScene(GLvoid){
    //所有的绘制图形的代码
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //清除颜色缓存和深度缓存,重置场景
    glLoadIdentity();
    //将当前点移动到屏幕中心
    glTranslatef(0.0f, 0.0f, z);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,texture[filter].id);
    //绑定纹理
    glBegin(GL_QUADS);
    //前面
    glTexCoord2f(0.0f, 0.0f);glVertex3f(-1.0f, -1.0f, 1.0f);
    //0.0是纹理的左侧,0.5f是纹理的中间,1.0f是纹理的右侧
    //第一个参数是x坐标,第二个参数是y坐标
    glNormal3f(0.0f, 0.0f, 1.0f);//法向量,用于标明外侧
    glTexCoord2f(1.0,0.0f);glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0,1.0f);glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f,1.0f);glVertex3f(-1.0f, 1.0f, 1.0f);
    //后面
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);glVertex3f(1.0f, -1.0f, -1.0f);
    //上面
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);glVertex3f(1.0f, 1.0f, -1.0f);
    //下面
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);glVertex3f(-1.0f, -1.0f, 1.0f);
    //左面
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);glVertex3f(-1.0f, 1.0f, -1.0f);
    //右面
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);glVertex3f(1.0f, -1.0f, 1.0f);
    glEnd();

    // glBindTexture(GL_TEXTURE_2D,0);
    // glColor3f(0.0f, 1.0f, 1.0f);
    // glLoadIdentity();
    // glTranslatef(0.0f, 1.0f, z);
    // glutSolidTeapot(1);
    return TRUE;
}

GLvoid KillGLWindow(GLvoid){
    for (int i = length - 1; i >= 0; i--)
    {
        texture[i].freeTexture();
    }
    
        
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
                    if(keys['L']&&!lp){
                        lp = TRUE;
                        light = !light;
                        if(light){
                            glEnable(GL_LIGHTING);
                        }else{
                            glDisable(GL_LIGHTING);
                        }
                    }
                    if(!keys['L']){
                        lp = FALSE;
                    }
                    if(keys['F']&&!fp){
                        fp = TRUE;
                        filter += 1;
                        if(filter>length-1){
                            filter = 0;
                        }
                        printf("now filter=%d\n",filter);
                    }
                    if(!keys['F']){
                        fp = FALSE;
                    }
                    if(keys[VK_PRIOR]){
                        z-=0.01;
                    }
                    if(keys[VK_NEXT]){
                        z+=0.01;
                    }
                    if(keys[VK_UP]){
                        xrot -= 0.05;
                    }
                    if(keys[VK_DOWN]){
                        xrot += 0.05;
                    }
                    if(keys[VK_LEFT]){
                        yrot -= 0.05;
                    }
                    if(keys[VK_RIGHT]){
                        yrot += 0.05;
                    }
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
            if(keys['B']&&!bp){
                bp = TRUE;
                blend = !blend;
                if(blend){
                    glEnable(GL_BLEND);
                }else{
                    glDisable(GL_BLEND);
                }
            }
            if(!keys['B']){
                bp = FALSE;
            }
        }
    }
    KillGLWindow();
    return (msg.wParam);
}