#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

static int year = 0, day = 0;
static int longitude = 200, latitude=200;

void init() 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   //glClearDepth(1.0);
   glShadeModel (GL_SMOOTH);    //多边形内部点的颜色通过对多边形顶点颜色插值产生，相当于gouraud明暗处理
   //glShadeModel (GL_FLAT);   //多边形内部所有点使用相同的颜色

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


   GLfloat mat_diffuse[] = { 0.96, 0.23, 0.11, 1.0 };  //漫反射系数，对应RGBA四个分量，A分量在混合开(glBlend)启后有效 

   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; //镜面反射系数,对应RGBA四个分量，A分量在混合开启后有效   
   GLfloat mat_shininess[] = { 50.0 };  
	  
 
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);//设置环境光系数和漫反射光系数     
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);//set material 镜面光反射  
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);//设置材料反射指数  

   glEnable(GL_LIGHTING);  
   glEnable(GL_LIGHT0);  
  
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );
    
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };//最后一个参数为0，表示光源在无穷远处  
   //GLfloat light_position[] = { 1.0, -1, 0.0, 1.0 };//局部光源  
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);//光源位置  


   glColor3f (1.0, 1.0, 1.0); //启用光源后，不在使用颜色函数对物体着色
   
   glMatrixMode(GL_MODELVIEW);  //使用模型视图矩阵栈保存后面的几何变换      
   glPushMatrix();              //保存世界坐标系到观察坐标系的变换矩阵
   
  //glutSolidSphere中第一、二、三个参数分别表示半径，经度线数、纬度线数
   //glutSolidSphere(1.0,longitude,latitude); 
   glutSolidTeapot(1.0);

   /*设定第二个球体的公转角度，注意公转相当于从先平移，再旋转，
   在该例中第二个球体的实际的变换顺序为 T1*T2*T3*球体的坐标，T3*球体的坐标被首先运算，
   所以在OpenGL中与变换矩阵相乘的顺序是遵循后调用者先乘。 ;   */
  
   glRotatef ((GLfloat) year, 0.0, 1.0, 0.0);  // 旋转变换T1
   glTranslatef (2.0, 0.0, 0.0);               //  平移变换T2
   glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);    // 旋转变换T3
    
   glColor3f(0.0,0.0,1.0);
   glutSolidSphere(0.2, longitude,latitude); /*绘制出第二个球体*/
   glPopMatrix();                //恢复世界坐标系到观察坐标系的变换矩阵
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);  //使视区大小保持与窗口的显示区域大小一致
   glMatrixMode(GL_MODELVIEW);                    //使用模型视图矩阵栈
   glLoadIdentity();
   
/* gluLookAt 函数设定了观察视点，和观察方向
函数原型：gluLookAt(GLdoble eyex,GLdouble eyey,GLdouble eyez,GLdouble centerx,GLdouble centery,GLdouble centerz,GLdouble upx,GLdouble upy,GLdouble upz);
它选取 eyex,eyey, eyez　做为原点（观察坐标系的坐标（用世界坐标系来表示的）），
然后centerx, centery,centerz　指定了观察方向（ｚ轴的反方向），　
upx,upy,upz　指定了ｙ轴的正方向的近似方向（它不一定和ｚ轴方向正交，但可以通过相关的运算，找到正交的Ｙ轴的正方向，从而也可以找到ｘ轴的正方向．主要就是通过向量的叉乘来计算）．
使用了gluLookAt函数，实际上是建立了从世界坐标系到观察坐标系的转换的矩阵，后面的投影变换将在观察坐标系下进行
*/
   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 

   glMatrixMode (GL_PROJECTION);              //使用投影矩阵栈，准备设定投影矩阵
   glLoadIdentity ();                            //初始化投影矩阵为单位矩阵

// gluPerspective函数定义视景体（裁剪棱台），四个参数的含义见图，
// 该函数实际上会根据四个参数计算用以投影变换的透视投影矩阵
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.5, 30.0);  
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 'd':
         day = (day + 10) % 360;   //增加自转的角度，逆时针旋转
         glutPostRedisplay();
         break;
      case 'D':
         day = (day - 10) % 360;   //减少自转的角度，顺时针旋转
         glutPostRedisplay();
         break;
      case 'y':
         year = (year + 5) % 360;  //增加公转的角度，逆时针旋转
         glutPostRedisplay();
         break;
      case 'Y':
         year = (year - 5) % 360;  //减少公转的角度，顺时针旋转
         glutPostRedisplay();
         break;
	  case 'l':
		 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //使用线框模型显示物体
		 glDisable(GL_LIGHTING);
		 longitude = 20; 
	     latitude=16;
		 glutPostRedisplay();
		 break;
	  case 'f':
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //使用多边形填充模型型显示物体
		 glEnable(GL_LIGHTING);
		 longitude = 200; 
	     latitude=200;
		 glutPostRedisplay();
		 break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);

   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}