#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

GLsizei winWidth = 400, winHeight = 300;

typedef struct point{
    int x;
    int y;
    point(int x,int y){
        this->x=x;
        this->y=y;
    }
}point;

vector<point> vertice;
//多边形的边为vertice[i]和vertice[i+1]

typedef struct XET{
    double x;
    double dx;
    double yMax;
    XET* next;
}AET,NET;

void setPixel(int x,int y){
    glBegin(GL_POINTS);
    glColor3f(0.0,1.0,1.0);
    glVertex2i(x,y);
    glEnd();
    glFlush();
}

void PolyScan(){
    int pointNum=vertice.size();
    if(pointNum<3){
        printf("多边形的顶点数不能小于3\n");
        return;
    }
    //计算yMax和yMin
    int yMax=vertice[0].y;
    int yMin=vertice[0].y;
    for(int i=1;i<pointNum;i++){
        if(vertice[i].y>yMax)
            yMax=vertice[i].y;
        if(vertice[i].y<yMin)
            yMin=vertice[i].y;
    }
    int lineNum=yMax-yMin;

    //printf("yMax=%d,yMin=%d,lineNum=%d\n",yMax,yMin,lineNum);

    NET* net=new NET[lineNum];
    for(int i=0;i<lineNum;i++){
        // net[i].x=0.0;
        // net[i].dx=0.0;
        // net[i].yMax=0.0;
        net[i].next=NULL;
    }

    for(int i=0;i<pointNum;i++){
        int nextI=(i+1)%pointNum;
        //printf("%d %d\n",vertice[i].y,vertice[nextI].y);
        if(vertice[i].y<vertice[nextI].y){
            NET* p=&net[vertice[i].y-yMin];
            while(p->next!=NULL&&p->next->x<vertice[i].x){
                p=p->next;
            }
            NET* q=new NET;
            // printf("vertice[%d].x=%d,vertice[%d].y=%d\n",i,vertice[i].x,i,vertice[i].y);
            // printf("vertice[%d].x=%d,vertice[%d].y=%d\n",nextI,vertice[nextI].x,nextI,vertice[nextI].y);
            q->x=vertice[i].x;
            q->dx=(double)(vertice[nextI].x-vertice[i].x)/(double)(vertice[nextI].y-vertice[i].y);
            //dx=1/k
            // TODO: 后期考虑优化
            q->yMax=vertice[nextI].y;
            q->next=p->next;
            p->next=q;
            //p=&net[vertice[i].y-yMin];
            // while(p){
            //     printf("p->x=%f,p->dx=%f,p->yMax=%f\n",p->x,p->dx,p->yMax);
            //     p=p->next;
            // }
        }
        else if(vertice[nextI].y<vertice[i].y){
            NET* p=&net[vertice[nextI].y-yMin];
            while(p->next!=NULL&&p->next->x<vertice[nextI].x){
                p=p->next;
            }
            // printf("vertice[%d].x=%d,vertice[%d].y=%d\n",i,vertice[i].x,i,vertice[i].y);
            // printf("vertice[%d].x=%d,vertice[%d].y=%d\n",nextI,vertice[nextI].x,nextI,vertice[nextI].y);
            NET* q=new NET;
            q->x=vertice[nextI].x;
            q->dx=(double)(vertice[i].x-vertice[nextI].x)/(double)(vertice[i].y-vertice[nextI].y);
            //dx=1/k
            // TODO: 后期考虑优化
            q->yMax=vertice[i].y;
            q->next=p->next;
            p->next=q;
        }
    }
    //建立新的边表NewEdgeTable

    // for(int i=0;i<lineNum;i++){
    //     NET* p=net[i].next;
    //     if(p==NULL){
    //         printf("空\n");
    //     }else{
    //         while(p){
    //             printf("p->x=%f,p->dx=%f,p->yMax=%f ",p->x,p->dx,p->yMax);
    //             p=p->next;
    //         }
    //         printf("\n");
    //     }
    // }

    AET* aet=new AET;
    aet->next=NULL;
    for(int i=0;i<lineNum;i++){
        AET* pre=aet;
        AET* p_renew=aet->next;
        while(p_renew){
            if((int)p_renew->yMax==i+yMin){
                //删除已经扫描到yMax的边
                AET* q=p_renew;
                pre->next=p_renew->next;
                //printf("删除边：x=%f,dx=%f,yMax=%f\n",p_renew->x,p_renew->dx,p_renew->yMax);
                delete q;
                p_renew=pre->next;
            }else{
                //更新新的x的值
                p_renew->x+=p_renew->dx;
                p_renew=p_renew->next;
                pre=pre->next;
            }
        }

        if(net[i].next!=NULL){
            //如果当前扫描线有新的边，那么便加入新边
            AET* netP=net[i].next;
            while(netP){
                AET* p_update=aet;
                while(p_update->next!=NULL&&p_update->next->x<netP->x){
                    //printf("已有边：x=%f,dx=%f,yMax=%f\n",p_update->next->x,p_update->next->dx,p_update->next->yMax);
                    p_update=p_update->next;
                }
                AET* q=new AET;
                q->x=netP->x;
                q->dx=netP->dx;
                q->yMax=netP->yMax;
                q->next=p_update->next;
                p_update->next=q;
                netP=netP->next;
                //printf("新增边：x=%f,dx=%f,yMax=%f\n",q->x,q->dx,q->yMax);
            }
        }

        //排序
        AET* pre_sort=aet;
        AET* p_sort=aet->next;
        while(p_sort){
            AET* min=p_sort;
            AET* pre_min=pre_sort;
            AET* pre_q=pre_sort->next;
            AET* q=p_sort->next;
            while(q){
                if(min->x>q->x){
                    pre_min=pre_q;
                    min=q;
                }
                pre_q=pre_q->next;
                q=q->next;
            }
            pre_min->next=min->next;
            min->next=pre_sort->next;
            pre_sort->next=min;
            pre_sort=pre_sort->next;
            p_sort=p_sort->next;
        }

        //printf("第%d条扫描线的边：",i+yMin);
        // AET* p=aet->next;
        // while(p){
        //     //printf("(%lf,%lf,%lf)->",p->x,p->dx,p->yMax);
        //     p=p->next;
        // }
        // printf("\n");
        
        AET* point=aet->next;
        AET* next=point->next;
        while(point){
            if(next==NULL){break;}
            //printf("开始点(%lf,%d),结束点(%lf,%d)\n",point->x,i+yMin,next->x,i+yMin);
            int s,e;
            s=point->x;
            e=next->x;
            for(int j=s;j<=e;j++){
                setPixel(j,i+yMin);
                //printf("(%d,%d)",j,i+yMin);
            }
            if(point->next->next!=NULL){
                point=point->next->next;
                next=point->next;
            }else{
                break;
            }
        }
    }
    glFlush();
    delete []net;
    AET* p=aet;
    while(p){
        AET* q=p;
        p=p->next;
        delete q;
    }
    //printf("Polygon Scan\n");
}

void myMouse(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN){
        setPixel(x,winHeight-y);
        point p(x,winHeight-y);
        vertice.push_back(p);
        printf("顶点:(%d,%d)\n",x,winHeight-y);
    }
    if(button==GLUT_RIGHT_BUTTON&&state==GLUT_DOWN){
        glBegin(GL_LINES);
        for(int i=0;i<vertice.size()-1;i++){
            glVertex2i(vertice[i].x,vertice[i].y);
            glVertex2i(vertice[i+1].x,vertice[i+1].y);
        }
        glVertex2i(vertice[vertice.size()-1].x,vertice[vertice.size()-1].y);
        glVertex2i(vertice[0].x,vertice[0].y);
        glEnd();
        glFlush();
        PolyScan();
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0,0.4,0.2);
    glPointSize(2);
    glFlush();
}

void winReshapeFcn(int newWidth,int newHeight){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,(GLdouble)newWidth,0,(GLdouble)newHeight);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argv,char** argc){
    glutInit(&argv,argc);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(500,200);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("poly_scan");

    // glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// gluOrtho2D(0, winWidth, 0, winHeight);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glutDisplayFunc(display);
    glutMouseFunc(myMouse);
    glutReshapeFunc(winReshapeFcn);

    glutMainLoop();
    return 0;
}
