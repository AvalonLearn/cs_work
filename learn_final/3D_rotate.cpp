//在窗口和全屏模式下切换，按ESC退出。

#include <Windows.h> //Windows的头文件
#include <gl/glut.h> //包含OpenGL实用库
#include <stdio.h>
#include <stdlib.h>

HGLRC hRC = NULL;    //窗口着色描述表句柄
HDC hDC = NULL;      // OpenGL渲染描述表句柄
HWND hWnd = NULL;    //保存窗口句柄
HINSTANCE hInstance; //保存程序实例

bool keys[256];         //保存键盘按键的数组
bool active = TRUE;     //窗口的活动标志，缺省为TRUE
bool fullscreen = TRUE; //全屏标志缺省，缺省设定成全屏模式

GLfloat rtri;  //用于三角形的角度
GLfloat rquad; //用于四边形的角度

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // WndProc的定义

//重置OpenGL窗口大小
GLvoid ResizeGLScene(GLsizei width, GLsizei height)
{
    if (height == 0) //防止被零除
    {
        height = 1; //将height设为1
    }

    glViewport(0, 0, width, height); //重置当前的视口
    glMatrixMode(GL_PROJECTION);     //旋转投影矩阵
    glLoadIdentity();                //重置投影矩阵

    //设置视口的大小
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW); //选择模型观察矩阵
    glLoadIdentity();           //重置模型观察矩阵
}

int InitGL(GLvoid) //对OpenGL进行设置
{
    glShadeModel(GL_SMOOTH);                           //启用阴影平滑
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);              //黑色背景
    glClearDepth(1.0f);                                //设置深度缓存
    glEnable(GL_DEPTH_TEST);                           //启用深度测试
    glDepthFunc(GL_LEQUAL);                            //所作深度测试的类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //告诉系统对透视进行修正

    return TRUE;
}

//进行绘制
int DrawGLScene(GLvoid)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity();                                   //重置当前的模型观察矩阵
    glTranslatef(-1.5f, 0.0f, -6.0f);                   //左移1.5单位，并移入屏幕6.0

    glRotatef(rtri, 0.0f, 1.0f, 0.0f); //绕Y轴旋转金字塔
    glBegin(GL_TRIANGLES);             //绘制金字塔各面

    glColor3f(1.0f, 0.0f, 0.0f);    //设置当前色为红色
    glVertex3f(0.0f, 1.0f, 0.0f);   //上顶点(前侧面)
    glColor3f(0.0f, 1.0f, 0.0f);    //设置当前色为绿色
    glVertex3f(-1.0f, -1.0f, 1.0f); //左下(前侧面)
    glColor3f(0.0f, 0.0f, 1.0f);    //设置当前色为蓝色
    glVertex3f(1.0f, -1.0f, 1.0f);  //右下(前侧面)

    glColor3f(1.0f, 0.0f, 0.0f);    //设置当前色为红色
    glVertex3f(0.0f, 1.0f, 0.0f);   //上顶点(右侧面)
    glColor3f(0.0f, 1.0f, 0.0f);    //设置当前色为绿色
    glVertex3f(1.0f, -1.0f, 1.0f);  //左下(右侧面)
    glColor3f(0.0f, 0.0f, 1.0f);    //设置当前色为蓝色
    glVertex3f(1.0f, -1.0f, -1.0f); //右下(右侧面)

    glColor3f(1.0f, 0.0f, 0.0f);     //设置当前色为红色
    glVertex3f(0.0f, 1.0f, 0.0f);    //上顶点(后侧面)
    glColor3f(0.0f, 1.0f, 0.0f);     //设置当前色为绿色
    glVertex3f(1.0f, -1.0f, -1.0f);  //左下(后侧面)
    glColor3f(0.0f, 0.0f, 1.0f);     //设置当前色为蓝色
    glVertex3f(-1.0f, -1.0f, -1.0f); //右下(后侧面)

    glColor3f(1.0f, 0.0f, 0.0f);     //设置当前色为红色
    glVertex3f(0.0f, 1.0f, 0.0f);    //上顶点(左侧面)
    glColor3f(0.0f, 1.0f, 0.0f);     //设置当前色为绿色
    glVertex3f(-1.0f, -1.0f, -1.0f); //左下(左侧面)
    glColor3f(0.0f, 0.0f, 1.0f);     //设置当前色为蓝色
    glVertex3f(-1.0f, -1.0f, 1.0f);  //右下(左侧面)

    glEnd(); //三角形绘制结束

    glLoadIdentity();                   //重置模型观察矩阵
    glTranslatef(1.5f, 0.0f, -7.0f);    //先右移再移入屏幕
    glRotatef(rquad, 1.0f, 1.0f, 1.0f); //绕XYZ轴旋转立方体

    glBegin(GL_QUADS);           //绘制正方形
    glColor3f(0.0f, 1.0f, 0.0f); //将当前色设置为绿色
    //顶面
    glVertex3f(1.0f, 1.0f, -1.0f);  //右上
    glVertex3f(-1.0f, 1.0f, -1.0f); //左上
    glVertex3f(-1.0f, 1.0f, 1.0f);  //左下
    glVertex3f(1.0f, 1.0f, 1.0f);   //右下

    //橙色
    glColor3f(1.0f, 0.5f, 0.0f);
    //底面
    glVertex3f(1.0f, -1.0f, -1.0f);  //右上
    glVertex3f(-1.0f, -1.0f, -1.0f); //左上
    glVertex3f(-1.0f, -1.0f, 1.0f);  //左下
    glVertex3f(1.0f, -1.0f, 1.0f);   //右下

    //红色
    glColor3f(1.0f, 0.0f, 0.0f);
    //前面
    glVertex3f(1.0f, 1.0f, 1.0f);   //右上
    glVertex3f(-1.0f, 1.0f, 1.0f);  //左上
    glVertex3f(-1.0f, -1.0f, 1.0f); //左下
    glVertex3f(1.0f, -1.0f, 1.0f);  //右下

    //黄色
    glColor3f(1.0f, 1.0f, 0.0f);
    //后面
    glVertex3f(1.0f, 1.0f, -1.0f);   //右上
    glVertex3f(-1.0f, 1.0f, -1.0f);  //左上
    glVertex3f(-1.0f, -1.0f, -1.0f); //左下
    glVertex3f(1.0f, -1.0f, -1.0f);  //右下

    //蓝色
    glColor3f(0.0f, 0.0f, 1.0f);
    //左面
    glVertex3f(-1.0f, 1.0f, 1.0f);   //右上
    glVertex3f(-1.0f, 1.0f, -1.0f);  //左上
    glVertex3f(-1.0f, -1.0f, -1.0f); //左下
    glVertex3f(-1.0f, -1.0f, 1.0f);  //右下

    //紫罗兰色
    glColor3f(1.0f, 0.0f, 1.0f);
    //右面
    glVertex3f(1.0f, 1.0f, 1.0f);   //右上
    glVertex3f(1.0f, 1.0f, -1.0f);  //左上
    glVertex3f(1.0f, -1.0f, -1.0f); //左下
    glVertex3f(1.0f, -1.0f, 1.0f);  //右下

    glEnd();

    //<控制旋转速度>
    rtri += 0.02f;   //增加三角形的旋转变量
    rquad -= 0.015f; //减少四边形的旋转变量

    return TRUE;
}

GLvoid KillGLWindow(GLvoid) //正常销毁窗口
{
    if (fullscreen) //是否处于全屏模式
    {
        ChangeDisplaySettings(NULL, 0); //是的话，切换回桌面
        ShowCursor(TRUE);               //显示鼠标指针
    }

    if (hRC) //是否拥有OpenGL渲染描述表
    {
        if (!wglMakeCurrent(NULL, NULL)) //能否是否DC和RC描述表
        {
            MessageBox(NULL, "释放DC或RC失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(hRC)) //能否删除RC
        {
            MessageBox(NULL, "释放RC失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
        }

        hRC = NULL;
    }

    if (hDC && !ReleaseDC(hWnd, hDC)) //能否释放DC
    {
        MessageBox(NULL, "释放DC失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
        hDC = NULL;
    }

    if (hWnd && !DestroyWindow(hWnd)) //能否销毁窗口
    {
        MessageBox(NULL, "释放窗口句柄失败。", "关闭错误", MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;
    }

    if (!UnregisterClass("OpenGL", hInstance)) //能否注销窗口类
    {
        MessageBox(NULL, "不能注销窗口类。", "关闭错误", MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;
    }
}

BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
    GLuint PixelFormat;             //保存查找匹配的结果
    WNDCLASS wc;                    //窗口类结构
    DWORD dwExStyle;                //扩展窗口风格
    DWORD dwStyle;                  //窗口风格
    RECT WindowRect;                //取得矩形的左上角和右下角的坐标值
    WindowRect.left = (long)0;      //将Left设为0
    WindowRect.right = (long)width; //将Right设为要求的宽度
    WindowRect.top = (long)0;
    WindowRect.bottom = (long)height;

    fullscreen = fullscreenflag; //设置全局全屏标志

    hInstance = GetModuleHandle(NULL);             //取得窗口的实例
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //移动时重画，并为窗口取得DC
    wc.lpfnWndProc = WndProc;                      // WndProc处理消息
    wc.cbClsExtra = 0;                             //无额外窗口数据Specifies the number of extra bytes to allocate following the window-class structure.
    wc.cbWndExtra = 0;                             //无额外窗口数据Specifies the number of extra bytes to allocate following the window instance.
    wc.hInstance = hInstance;                      //设置实例
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);        //装入缺省图标
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      //装入鼠标指针
    wc.hbrBackground = NULL;                       // GL不需要背景
    wc.lpszMenuName = NULL;                        //不需要菜单
    wc.lpszClassName = "OpenGL";                   //设定类名字

    if (!RegisterClass(&wc)) //尝试注册窗口类
    {
        MessageBox(NULL, "注册窗口失败", "错误", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (fullscreen)
    {
        DEVMODE dmScreenSettings;                               //设备模式
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); //确保内存清空为零
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);     // Devmode结构的大小
        dmScreenSettings.dmPelsWidth = width;                   //所选屏幕宽度
        dmScreenSettings.dmPelsHeight = height;                 //所选屏幕高度
        dmScreenSettings.dmBitsPerPel = bits;                   //每像素所选的色彩浓度
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        //尝试设置显示模式并返回结果。注：CDS_FULLSCREEN移去了状态条。
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            //若模式失败，提供两个选项：退出或在窗口内运行。
            if (MessageBox(NULL, "全屏模式在当前显卡上设置失败!\n使用窗口模式?",
                           "提示", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
            {
                fullscreen = FALSE; //选择窗口模式
            }
            else
            {
                //弹出一个对话框，告诉用户程序结束
                MessageBox(NULL, "程序将被关闭", "错误", MB_OK | MB_ICONSTOP);
                return FALSE;
            }
        }
    }

    if (fullscreen)
    {
        dwExStyle = WS_EX_APPWINDOW; //扩展窗体风格
        dwStyle = WS_POPUP;          //窗体风格
        ShowCursor(FALSE);           //隐藏鼠标指针
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //扩展窗体风格
        dwStyle = WS_OVERLAPPEDWINDOW;                  //窗体风格
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); //调整窗口达到真正要求的大小

    //窗体自适应各种分辨率以居中方式显示
    LONG nX, nY;

    nX = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    nY = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    if (!(hWnd = CreateWindowEx(dwExStyle,                          //扩展窗体风格
                                "OpenGL",                           //类名字
                                title,                              //窗口标题
                                WS_CLIPSIBLINGS | WS_CLIPCHILDREN | //必须的窗体风格属性
                                    dwStyle,                        //选择的窗体属性
                                nX, nY,                             //窗口位置
                                WindowRect.right - WindowRect.left, //计算调整好的窗口宽度
                                WindowRect.bottom - WindowRect.top, //计算调整好的窗体高度
                                NULL,                               //无父窗口
                                NULL,                               //无菜单
                                hInstance,                          //实例
                                NULL)))                             //不向WM_CREATE传递任何东西
    {
        KillGLWindow(); //重置显示区
        MessageBox(NULL, "不能创建一个窗口设备描述表", "错误", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    static PIXELFORMATDESCRIPTOR pfd = // pdf告诉窗口希望的东西，即窗口使用的像素格式
        {
            sizeof(PIXELFORMATDESCRIPTOR), //上述格式描述符的大小
            1,                             //版本号
            PFD_DRAW_TO_WINDOW |           //格式支持窗口
                PFD_SUPPORT_OPENGL |       //格式支持OpenGL
                PFD_DOUBLEBUFFER,          //支持双缓冲
            PFD_TYPE_RGBA,                 //申请RGBA格式
            bits,                          //选定色彩深度
            0, 0, 0, 0, 0, 0,              //忽略的色彩位
            0,                             //无Alpha缓存
            0,                             //忽略Shift Bit
            0,                             //无累加缓存
            0, 0, 0, 0,                    //忽略聚集位
            16,                            // 16位Z-缓存(深度缓存)
            0,                             //无蒙板缓存
            0,                             //无辅助缓存
            PFD_MAIN_PLANE,                //主绘图层
            0,                             // Reserved
            0, 0, 0                        //忽略层遮罩
        };

    if (!(hDC = GetDC(hWnd))) //是否取得设备描述表
    {
        KillGLWindow(); //重置显示区
        MessageBox(NULL, "不能创建一种相匹配的像素格式", "错误", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // Windows是否找到相应的像素格式
    {
        KillGLWindow(); //重置显示区
        MessageBox(NULL, "不能设置像素格式", "错误", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!SetPixelFormat(hDC, PixelFormat, &pfd)) //是否能够设置像素格式
    {
        KillGLWindow(); //重置显示区
        MessageBox(NULL, "不能设置像素格式", "错误", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!(hRC = wglCreateContext(hDC))) //能否取得着色描述表
    {
        KillGLWindow(); //重置显示区
        MessageBox(NULL, "不能创建OpenGL渲染描述表", "错误", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!wglMakeCurrent(hDC, hRC)) //尝试激活着色描述表
    {
        KillGLWindow();
        MessageBox(NULL, "不能激活当前的OpenGL渲染描述表", "错误", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOW);    //显示窗口
    SetForegroundWindow(hWnd);    //提供优先级别
    SetFocus(hWnd);               //设置键盘的焦点至此窗口
    ResizeGLScene(width, height); //设置透视GL屏幕

    if (!InitGL()) //初始化新建的GL窗口
    {
        KillGLWindow();
        MessageBox(NULL, "Initialization", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    return TRUE;
}

//处理所有的窗口消息
LRESULT CALLBACK WndProc(HWND hWnd,     //窗口的句柄
                         UINT uMsg,     //窗口的消息
                         WPARAM wParam, //附加的消息内容
                         LPARAM lParam) //附加的消息内容
{
    switch (uMsg) //检查检查Windows消息
    {
    case WM_ACTIVATE: //监视窗口激活消息
    {
        if (!HIWORD(wParam)) //检查最小化状态
        {
            active = TRUE; //程序处于激活状态
        }
        else
        {
            active = FALSE; //程序不再激活
        }

        return 0; //返回消息循环
    }

    case WM_SYSCOMMAND: //系统中断命令
    {
        switch (wParam) //检查系统调用
        {
        case SC_SCREENSAVE:   //屏保运行
        case SC_MONITORPOWER: //显示器进入节电模式
            return 0;         //阻止发生
        }
        break; //退出
    }
    case WM_CLOSE: //收到Close消息
    {
        PostQuitMessage(0); //发出退出消息
        return 0;
    }
    case WM_KEYDOWN: //有键按下
    {
        keys[wParam] = TRUE;
        return 0;
    }
    case WM_KEYUP: //有键放开
    {
        keys[wParam] = FALSE;
        return 0;
    }
    case WM_SIZE: //调整OpenGL窗口大小
    {
        ResizeGLScene(LOWORD(lParam), HIWORD(lParam)); // LOWORD=width,HIWORD=height
        return 0;
    }
    }

    //向DefWindowProc传递所有未处理的消息
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Windows程序的入口，调用窗口创建例程，处理窗口消息，并监视人机交互。
int WINAPI WinMain(HINSTANCE hInstance,     //当前窗口实例
                   HINSTANCE hPrevInstance, //前一个窗口实例
                   LPSTR lpCmdLine,         //命令行参数
                   int nCmdShow)            //窗口显示状态
{
    MSG msg;           // Windows消息结构
    BOOL done = FALSE; //用来退出循环的BOOL变量
    //提示用户选择运行模式
    if (MessageBox(NULL, "你想在全屏模式下运行么？", "设置全屏模式", MB_YESNO | MB_ICONQUESTION) == IDNO)
    {
        fullscreen = FALSE; //设置为窗口模式
    }

    //创建OpenGL窗口
    if (!CreateGLWindow("OpenGL 3D 旋转物体", 640, 480, 16, fullscreen))
    {
        return 0; //失败退出
    }

    while (!done)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //是否有消息在等待
        {
            if (msg.message == WM_QUIT) //收到退出消息
            {
                done = TRUE;
            }
            else //处理窗口消息
            {
                TranslateMessage(&msg); //翻译消息
                DispatchMessage(&msg);  //发送消息
            }
        }
        else //如果没有消息
        {
            //绘制场景。监视ESC键和来自DrawGLScene()的退出消息
            if (active)
            {
                if (keys[VK_ESCAPE])
                {
                    done = TRUE;
                }
                else //没有退出，刷新屏幕
                {
                    DrawGLScene();    //绘制场景
                    SwapBuffers(hDC); //交换缓存(双缓存)
                }
            }
            if (keys[VK_F1])
            {
                keys[VK_F1] = FALSE;
                KillGLWindow();
                fullscreen = !fullscreen; //切换全屏/窗口模式
                //重建OpenGL窗口
                if (!CreateGLWindow("3D旋转实例", 640, 480, 16, fullscreen))
                {
                    return 0;
                }
            }
        }
    }

    //关闭程序
    KillGLWindow();      //销毁窗口
    return (msg.wParam); //退出程序
}