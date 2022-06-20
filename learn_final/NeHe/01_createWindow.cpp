#include <windows.h>
#include <GL/glut.h>
#include <tchar.h>

HGLRC hRC=NULL;//��ɫ������
HDC hDC=NULL;//�豸������
HWND hWnd=NULL;//���ھ��
HINSTANCE hInstance;//Ӧ�ó���ʵ��

bool keys[256];//����״̬
bool active=true;//����״̬
bool fullscreen = FALSE;//ȫ��״̬

char* window_title = "�����κ�������";

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

int InitGL(GLvoid){
    //��ʼ��
    glShadeModel(GL_SMOOTH);//������Ӱƽ��
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//���û�������ɫ

    //�����е�
    glClearDepth(1.0f);//������Ȼ���
    glEnable(GL_DEPTH_TEST);//������Ȳ���
    glDepthFunc(GL_LEQUAL);//������Ȳ�������
    return TRUE;
}

int DrawGLScene(GLvoid){
    //���еĻ���ͼ�εĴ���
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //�����ɫ�������Ȼ���,���ó���
    glLoadIdentity();
    //����ǰ���ƶ�����Ļ����
    return TRUE;
}

GLvoid KillGLWindow(GLvoid){
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