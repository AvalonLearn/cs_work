//�ڴ��ں�ȫ��ģʽ���л�����ESC�˳���

#include <Windows.h> //Windows��ͷ�ļ�
#include <gl/glut.h> //����OpenGLʵ�ÿ�
#include <stdio.h>
#include <stdlib.h>

HGLRC hRC = NULL;    //������ɫ��������
HDC hDC = NULL;      // OpenGL��Ⱦ��������
HWND hWnd = NULL;    //���洰�ھ��
HINSTANCE hInstance; //�������ʵ��

bool keys[256];         //������̰���������
bool active = TRUE;     //���ڵĻ��־��ȱʡΪTRUE
bool fullscreen = TRUE; //ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

GLfloat rtri;  //���������εĽǶ�
GLfloat rquad; //�����ı��εĽǶ�

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // WndProc�Ķ���

//����OpenGL���ڴ�С
GLvoid ResizeGLScene(GLsizei width, GLsizei height)
{
    if (height == 0) //��ֹ�����
    {
        height = 1; //��height��Ϊ1
    }

    glViewport(0, 0, width, height); //���õ�ǰ���ӿ�
    glMatrixMode(GL_PROJECTION);     //��תͶӰ����
    glLoadIdentity();                //����ͶӰ����

    //�����ӿڵĴ�С
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW); //ѡ��ģ�͹۲����
    glLoadIdentity();           //����ģ�͹۲����
}

int InitGL(GLvoid) //��OpenGL��������
{
    glShadeModel(GL_SMOOTH);                           //������Ӱƽ��
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);              //��ɫ����
    glClearDepth(1.0f);                                //������Ȼ���
    glEnable(GL_DEPTH_TEST);                           //������Ȳ���
    glDepthFunc(GL_LEQUAL);                            //������Ȳ��Ե�����
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //����ϵͳ��͸�ӽ�������

    return TRUE;
}

//���л���
int DrawGLScene(GLvoid)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����Ļ����Ȼ���
    glLoadIdentity();                                   //���õ�ǰ��ģ�͹۲����
    glTranslatef(-1.5f, 0.0f, -6.0f);                   //����1.5��λ����������Ļ6.0

    glRotatef(rtri, 0.0f, 1.0f, 0.0f); //��Y����ת������
    glBegin(GL_TRIANGLES);             //���ƽ���������

    glColor3f(1.0f, 0.0f, 0.0f);    //���õ�ǰɫΪ��ɫ
    glVertex3f(0.0f, 1.0f, 0.0f);   //�϶���(ǰ����)
    glColor3f(0.0f, 1.0f, 0.0f);    //���õ�ǰɫΪ��ɫ
    glVertex3f(-1.0f, -1.0f, 1.0f); //����(ǰ����)
    glColor3f(0.0f, 0.0f, 1.0f);    //���õ�ǰɫΪ��ɫ
    glVertex3f(1.0f, -1.0f, 1.0f);  //����(ǰ����)

    glColor3f(1.0f, 0.0f, 0.0f);    //���õ�ǰɫΪ��ɫ
    glVertex3f(0.0f, 1.0f, 0.0f);   //�϶���(�Ҳ���)
    glColor3f(0.0f, 1.0f, 0.0f);    //���õ�ǰɫΪ��ɫ
    glVertex3f(1.0f, -1.0f, 1.0f);  //����(�Ҳ���)
    glColor3f(0.0f, 0.0f, 1.0f);    //���õ�ǰɫΪ��ɫ
    glVertex3f(1.0f, -1.0f, -1.0f); //����(�Ҳ���)

    glColor3f(1.0f, 0.0f, 0.0f);     //���õ�ǰɫΪ��ɫ
    glVertex3f(0.0f, 1.0f, 0.0f);    //�϶���(�����)
    glColor3f(0.0f, 1.0f, 0.0f);     //���õ�ǰɫΪ��ɫ
    glVertex3f(1.0f, -1.0f, -1.0f);  //����(�����)
    glColor3f(0.0f, 0.0f, 1.0f);     //���õ�ǰɫΪ��ɫ
    glVertex3f(-1.0f, -1.0f, -1.0f); //����(�����)

    glColor3f(1.0f, 0.0f, 0.0f);     //���õ�ǰɫΪ��ɫ
    glVertex3f(0.0f, 1.0f, 0.0f);    //�϶���(�����)
    glColor3f(0.0f, 1.0f, 0.0f);     //���õ�ǰɫΪ��ɫ
    glVertex3f(-1.0f, -1.0f, -1.0f); //����(�����)
    glColor3f(0.0f, 0.0f, 1.0f);     //���õ�ǰɫΪ��ɫ
    glVertex3f(-1.0f, -1.0f, 1.0f);  //����(�����)

    glEnd(); //�����λ��ƽ���

    glLoadIdentity();                   //����ģ�͹۲����
    glTranslatef(1.5f, 0.0f, -7.0f);    //��������������Ļ
    glRotatef(rquad, 1.0f, 1.0f, 1.0f); //��XYZ����ת������

    glBegin(GL_QUADS);           //����������
    glColor3f(0.0f, 1.0f, 0.0f); //����ǰɫ����Ϊ��ɫ
    //����
    glVertex3f(1.0f, 1.0f, -1.0f);  //����
    glVertex3f(-1.0f, 1.0f, -1.0f); //����
    glVertex3f(-1.0f, 1.0f, 1.0f);  //����
    glVertex3f(1.0f, 1.0f, 1.0f);   //����

    //��ɫ
    glColor3f(1.0f, 0.5f, 0.0f);
    //����
    glVertex3f(1.0f, -1.0f, -1.0f);  //����
    glVertex3f(-1.0f, -1.0f, -1.0f); //����
    glVertex3f(-1.0f, -1.0f, 1.0f);  //����
    glVertex3f(1.0f, -1.0f, 1.0f);   //����

    //��ɫ
    glColor3f(1.0f, 0.0f, 0.0f);
    //ǰ��
    glVertex3f(1.0f, 1.0f, 1.0f);   //����
    glVertex3f(-1.0f, 1.0f, 1.0f);  //����
    glVertex3f(-1.0f, -1.0f, 1.0f); //����
    glVertex3f(1.0f, -1.0f, 1.0f);  //����

    //��ɫ
    glColor3f(1.0f, 1.0f, 0.0f);
    //����
    glVertex3f(1.0f, 1.0f, -1.0f);   //����
    glVertex3f(-1.0f, 1.0f, -1.0f);  //����
    glVertex3f(-1.0f, -1.0f, -1.0f); //����
    glVertex3f(1.0f, -1.0f, -1.0f);  //����

    //��ɫ
    glColor3f(0.0f, 0.0f, 1.0f);
    //����
    glVertex3f(-1.0f, 1.0f, 1.0f);   //����
    glVertex3f(-1.0f, 1.0f, -1.0f);  //����
    glVertex3f(-1.0f, -1.0f, -1.0f); //����
    glVertex3f(-1.0f, -1.0f, 1.0f);  //����

    //������ɫ
    glColor3f(1.0f, 0.0f, 1.0f);
    //����
    glVertex3f(1.0f, 1.0f, 1.0f);   //����
    glVertex3f(1.0f, 1.0f, -1.0f);  //����
    glVertex3f(1.0f, -1.0f, -1.0f); //����
    glVertex3f(1.0f, -1.0f, 1.0f);  //����

    glEnd();

    //<������ת�ٶ�>
    rtri += 0.02f;   //���������ε���ת����
    rquad -= 0.015f; //�����ı��ε���ת����

    return TRUE;
}

GLvoid KillGLWindow(GLvoid) //�������ٴ���
{
    if (fullscreen) //�Ƿ���ȫ��ģʽ
    {
        ChangeDisplaySettings(NULL, 0); //�ǵĻ����л�������
        ShowCursor(TRUE);               //��ʾ���ָ��
    }

    if (hRC) //�Ƿ�ӵ��OpenGL��Ⱦ������
    {
        if (!wglMakeCurrent(NULL, NULL)) //�ܷ��Ƿ�DC��RC������
        {
            MessageBox(NULL, "�ͷ�DC��RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(hRC)) //�ܷ�ɾ��RC
        {
            MessageBox(NULL, "�ͷ�RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
        }

        hRC = NULL;
    }

    if (hDC && !ReleaseDC(hWnd, hDC)) //�ܷ��ͷ�DC
    {
        MessageBox(NULL, "�ͷ�DCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
        hDC = NULL;
    }

    if (hWnd && !DestroyWindow(hWnd)) //�ܷ����ٴ���
    {
        MessageBox(NULL, "�ͷŴ��ھ��ʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;
    }

    if (!UnregisterClass("OpenGL", hInstance)) //�ܷ�ע��������
    {
        MessageBox(NULL, "����ע�������ࡣ", "�رմ���", MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;
    }
}

BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
    GLuint PixelFormat;             //�������ƥ��Ľ��
    WNDCLASS wc;                    //������ṹ
    DWORD dwExStyle;                //��չ���ڷ��
    DWORD dwStyle;                  //���ڷ��
    RECT WindowRect;                //ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
    WindowRect.left = (long)0;      //��Left��Ϊ0
    WindowRect.right = (long)width; //��Right��ΪҪ��Ŀ��
    WindowRect.top = (long)0;
    WindowRect.bottom = (long)height;

    fullscreen = fullscreenflag; //����ȫ��ȫ����־

    hInstance = GetModuleHandle(NULL);             //ȡ�ô��ڵ�ʵ��
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //�ƶ�ʱ�ػ�����Ϊ����ȡ��DC
    wc.lpfnWndProc = WndProc;                      // WndProc������Ϣ
    wc.cbClsExtra = 0;                             //�޶��ⴰ������Specifies the number of extra bytes to allocate following the window-class structure.
    wc.cbWndExtra = 0;                             //�޶��ⴰ������Specifies the number of extra bytes to allocate following the window instance.
    wc.hInstance = hInstance;                      //����ʵ��
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);        //װ��ȱʡͼ��
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      //װ�����ָ��
    wc.hbrBackground = NULL;                       // GL����Ҫ����
    wc.lpszMenuName = NULL;                        //����Ҫ�˵�
    wc.lpszClassName = "OpenGL";                   //�趨������

    if (!RegisterClass(&wc)) //����ע�ᴰ����
    {
        MessageBox(NULL, "ע�ᴰ��ʧ��", "����", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (fullscreen)
    {
        DEVMODE dmScreenSettings;                               //�豸ģʽ
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); //ȷ���ڴ����Ϊ��
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);     // Devmode�ṹ�Ĵ�С
        dmScreenSettings.dmPelsWidth = width;                   //��ѡ��Ļ���
        dmScreenSettings.dmPelsHeight = height;                 //��ѡ��Ļ�߶�
        dmScreenSettings.dmBitsPerPel = bits;                   //ÿ������ѡ��ɫ��Ũ��
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        //����������ʾģʽ�����ؽ����ע��CDS_FULLSCREEN��ȥ��״̬����
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            //��ģʽʧ�ܣ��ṩ����ѡ��˳����ڴ��������С�
            if (MessageBox(NULL, "ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ��!\nʹ�ô���ģʽ?",
                           "��ʾ", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
            {
                fullscreen = FALSE; //ѡ�񴰿�ģʽ
            }
            else
            {
                //����һ���Ի��򣬸����û��������
                MessageBox(NULL, "���򽫱��ر�", "����", MB_OK | MB_ICONSTOP);
                return FALSE;
            }
        }
    }

    if (fullscreen)
    {
        dwExStyle = WS_EX_APPWINDOW; //��չ������
        dwStyle = WS_POPUP;          //������
        ShowCursor(FALSE);           //�������ָ��
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //��չ������
        dwStyle = WS_OVERLAPPEDWINDOW;                  //������
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); //�������ڴﵽ����Ҫ��Ĵ�С

    //��������Ӧ���ֱַ����Ծ��з�ʽ��ʾ
    LONG nX, nY;

    nX = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    nY = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    if (!(hWnd = CreateWindowEx(dwExStyle,                          //��չ������
                                "OpenGL",                           //������
                                title,                              //���ڱ���
                                WS_CLIPSIBLINGS | WS_CLIPCHILDREN | //����Ĵ���������
                                    dwStyle,                        //ѡ��Ĵ�������
                                nX, nY,                             //����λ��
                                WindowRect.right - WindowRect.left, //��������õĴ��ڿ��
                                WindowRect.bottom - WindowRect.top, //��������õĴ���߶�
                                NULL,                               //�޸�����
                                NULL,                               //�޲˵�
                                hInstance,                          //ʵ��
                                NULL)))                             //����WM_CREATE�����κζ���
    {
        KillGLWindow(); //������ʾ��
        MessageBox(NULL, "���ܴ���һ�������豸������", "����", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    static PIXELFORMATDESCRIPTOR pfd = // pdf���ߴ���ϣ���Ķ�����������ʹ�õ����ظ�ʽ
        {
            sizeof(PIXELFORMATDESCRIPTOR), //������ʽ�������Ĵ�С
            1,                             //�汾��
            PFD_DRAW_TO_WINDOW |           //��ʽ֧�ִ���
                PFD_SUPPORT_OPENGL |       //��ʽ֧��OpenGL
                PFD_DOUBLEBUFFER,          //֧��˫����
            PFD_TYPE_RGBA,                 //����RGBA��ʽ
            bits,                          //ѡ��ɫ�����
            0, 0, 0, 0, 0, 0,              //���Ե�ɫ��λ
            0,                             //��Alpha����
            0,                             //����Shift Bit
            0,                             //���ۼӻ���
            0, 0, 0, 0,                    //���Ծۼ�λ
            16,                            // 16λZ-����(��Ȼ���)
            0,                             //���ɰ建��
            0,                             //�޸�������
            PFD_MAIN_PLANE,                //����ͼ��
            0,                             // Reserved
            0, 0, 0                        //���Բ�����
        };

    if (!(hDC = GetDC(hWnd))) //�Ƿ�ȡ���豸������
    {
        KillGLWindow(); //������ʾ��
        MessageBox(NULL, "���ܴ���һ����ƥ������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // Windows�Ƿ��ҵ���Ӧ�����ظ�ʽ
    {
        KillGLWindow(); //������ʾ��
        MessageBox(NULL, "�����������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!SetPixelFormat(hDC, PixelFormat, &pfd)) //�Ƿ��ܹ��������ظ�ʽ
    {
        KillGLWindow(); //������ʾ��
        MessageBox(NULL, "�����������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!(hRC = wglCreateContext(hDC))) //�ܷ�ȡ����ɫ������
    {
        KillGLWindow(); //������ʾ��
        MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!wglMakeCurrent(hDC, hRC)) //���Լ�����ɫ������
    {
        KillGLWindow();
        MessageBox(NULL, "���ܼ��ǰ��OpenGL��Ⱦ������", "����", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOW);    //��ʾ����
    SetForegroundWindow(hWnd);    //�ṩ���ȼ���
    SetFocus(hWnd);               //���ü��̵Ľ������˴���
    ResizeGLScene(width, height); //����͸��GL��Ļ

    if (!InitGL()) //��ʼ���½���GL����
    {
        KillGLWindow();
        MessageBox(NULL, "Initialization", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    return TRUE;
}

//�������еĴ�����Ϣ
LRESULT CALLBACK WndProc(HWND hWnd,     //���ڵľ��
                         UINT uMsg,     //���ڵ���Ϣ
                         WPARAM wParam, //���ӵ���Ϣ����
                         LPARAM lParam) //���ӵ���Ϣ����
{
    switch (uMsg) //�����Windows��Ϣ
    {
    case WM_ACTIVATE: //���Ӵ��ڼ�����Ϣ
    {
        if (!HIWORD(wParam)) //�����С��״̬
        {
            active = TRUE; //�����ڼ���״̬
        }
        else
        {
            active = FALSE; //�����ټ���
        }

        return 0; //������Ϣѭ��
    }

    case WM_SYSCOMMAND: //ϵͳ�ж�����
    {
        switch (wParam) //���ϵͳ����
        {
        case SC_SCREENSAVE:   //��������
        case SC_MONITORPOWER: //��ʾ������ڵ�ģʽ
            return 0;         //��ֹ����
        }
        break; //�˳�
    }
    case WM_CLOSE: //�յ�Close��Ϣ
    {
        PostQuitMessage(0); //�����˳���Ϣ
        return 0;
    }
    case WM_KEYDOWN: //�м�����
    {
        keys[wParam] = TRUE;
        return 0;
    }
    case WM_KEYUP: //�м��ſ�
    {
        keys[wParam] = FALSE;
        return 0;
    }
    case WM_SIZE: //����OpenGL���ڴ�С
    {
        ResizeGLScene(LOWORD(lParam), HIWORD(lParam)); // LOWORD=width,HIWORD=height
        return 0;
    }
    }

    //��DefWindowProc��������δ�������Ϣ
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Windows�������ڣ����ô��ڴ������̣���������Ϣ���������˻�������
int WINAPI WinMain(HINSTANCE hInstance,     //��ǰ����ʵ��
                   HINSTANCE hPrevInstance, //ǰһ������ʵ��
                   LPSTR lpCmdLine,         //�����в���
                   int nCmdShow)            //������ʾ״̬
{
    MSG msg;           // Windows��Ϣ�ṹ
    BOOL done = FALSE; //�����˳�ѭ����BOOL����
    //��ʾ�û�ѡ������ģʽ
    if (MessageBox(NULL, "������ȫ��ģʽ������ô��", "����ȫ��ģʽ", MB_YESNO | MB_ICONQUESTION) == IDNO)
    {
        fullscreen = FALSE; //����Ϊ����ģʽ
    }

    //����OpenGL����
    if (!CreateGLWindow("OpenGL 3D ��ת����", 640, 480, 16, fullscreen))
    {
        return 0; //ʧ���˳�
    }

    while (!done)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //�Ƿ�����Ϣ�ڵȴ�
        {
            if (msg.message == WM_QUIT) //�յ��˳���Ϣ
            {
                done = TRUE;
            }
            else //��������Ϣ
            {
                TranslateMessage(&msg); //������Ϣ
                DispatchMessage(&msg);  //������Ϣ
            }
        }
        else //���û����Ϣ
        {
            //���Ƴ���������ESC��������DrawGLScene()���˳���Ϣ
            if (active)
            {
                if (keys[VK_ESCAPE])
                {
                    done = TRUE;
                }
                else //û���˳���ˢ����Ļ
                {
                    DrawGLScene();    //���Ƴ���
                    SwapBuffers(hDC); //��������(˫����)
                }
            }
            if (keys[VK_F1])
            {
                keys[VK_F1] = FALSE;
                KillGLWindow();
                fullscreen = !fullscreen; //�л�ȫ��/����ģʽ
                //�ؽ�OpenGL����
                if (!CreateGLWindow("3D��תʵ��", 640, 480, 16, fullscreen))
                {
                    return 0;
                }
            }
        }
    }

    //�رճ���
    KillGLWindow();      //���ٴ���
    return (msg.wParam); //�˳�����
}