#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GLUT/glut.h>

#define G 9.8
#define E 0.70
#define dt 0.1

int a;
bool an = true;
/*構造体の定義*/
struct ball
{
    int id;
    double r; /* radius */
    double m, I; /* mass and inertial */
    double x, y, tht;/*座標*/
    double dx, dy, dtht;/*速度*/
    double ddx, ddy, ddtht;/*加速度*/
    double f_x, f_y;/*加わる力*/
};

struct ball ball1, foot;

struct leg
{
    int id;
    double joint_x, joint_y, leg_tht;
    double leg_len, leg_wid;
};

struct leg leg1;

/*初期値の設定*/

static GLfloat ang = -60.0;
static GLdouble i = 0.0;
static GLdouble tai_z = 10;
static GLdouble t = 0;

static GLdouble d_ball = 0.0;

static GLdouble dist_x = 0.0;
static GLdouble dist_y = 0.0;

/*関数プロトタイプ*/

void ball(double r);
void leg(void);
void display(void);
void simu(void);
void init(void);
void idle(void);
void reshape(int x, int y);
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);


int main(int argc, char *argv[])
{

    ball1.x = 0.0;
    ball1.y = -45.0;

    ball1.dx = 0.0;
    ball1.dy = 0.0;

    ball1.ddx = 0.0;
    ball1.ddy = -G;

    ball1.m = 10;
    foot.m = 50;

    ball1.f_x = 10;
    ball1.f_y = -ball1.m * G;

    tai_z = 100;

    leg1.joint_x = -15.0;
    leg1.joint_y = 0.0;

    leg1.leg_len = 47.5;
    leg1.leg_wid = 5.0;

    ball1.r = 5;
    foot.r = 3.536;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (400, 400);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    init();
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutDisplayFunc(display);
    //glutIdleFunc(bound);
    glutIdleFunc(simu);
    glutMainLoop();

    return 0;
}

void ball(double r)
{
    glBegin(GL_POLYGON);
    for(i = 0.0; i <= 2.0; i += 0.05)
        glVertex2d( r*sin(M_PI*i), r*cos(M_PI*i) );
    glEnd();
}

void leg(void)
{
    glBegin(GL_POLYGON);
    glVertex2d(2.5, 2.5);
    glVertex2d(-2.5, 2.5);
    glVertex2d(-2.5, -45.0);
    glVertex2d(2.5, -45.0);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2d(-2.5, -45.0);
    glVertex2d(-2.5, -50.0);
    glVertex2d(7.5, -50.0);
    glVertex2d(7.5, -45.0);
    glEnd();

    glPushMatrix();
    glTranslatef(5.0, -47.5, 0.0);
    ball(foot.r);
    glPopMatrix();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.5, 1.0, 0.0);


    glPushMatrix();
    glTranslatef(ball1.x, ball1.y, 0.0);//移動
    //glRotatef(ang, 0.0, 0.0, 1.0);//回転
    ball(ball1.r);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(leg1.joint_x,leg1.joint_y,0.0);
    glRotatef(ang, 0.0, 0.0, 1);
    leg();
    glPopMatrix();

    glutSwapBuffers();
}

void simu(void)
{
    if(an){
        ang = ang + 2.5;
        if ( ang > 360.0 )
            ang = ang - 360.0;
        //glutPostRedisplay();//再描画
    }

    /*跳ね返り*/

    if (ball1.x >= 45){
        ball1.x = 45;
        ball1.dx = -ball1.dx;
    }else if(ball1.x <= -45){
        ball1.x = -45;
        ball1.dx = -ball1.dx;
    }
    /*
       if (ball1.y >= 45){
       ball1.y = 45;
       ball1.dy = -ball1.dy;
       }else if(ball1.y <= -45){
       ball1.dy = -ball1.dy * E;
//f_y = f_y - E * ( ball1.r - ball1.y + 45);
ball1.y = -45;

if (ball1.dy < 1.5){
ball1.dy = 0.0;
}

}
*/
if(ball1.y <= -45){
    ball1.dy = -ball1.dy * E;
    //f_y = f_y - E * ( ball1.r - ball1.y + 45);
    ball1.y = -45;

    if (ball1.dy < 1.5){
        ball1.dy = 0.0;
    }
}


printf("座標\t:%f\t%f\t%f\n速度\t:%f\t%f\t%f\n加速度\t:%f\t%f\t%f\n", ball1.x, ball1.y, ball1.tht, ball1.dx, ball1.dy, ball1.dtht, ball1.ddx, ball1.ddy, ball1.ddtht);

//glutPostRedisplay();


/*ボールの座標・速度・加速度*/
/*y軸*/

//ball1.ddy = ball1.f_y / ball1.m;
ball1.y = ball1.y + ball1.dy * dt + ball1.ddy * dt * dt / 2.0;
ball1.dy = ball1.dy + ball1.ddy * dt;

/*x軸*/
/*
   ball1.ddx = ball1.f_x / ball1.m;
   ball1.x = ball1.x + ball1.dx * dt + ball1.ddx * dt * dt / 2.0;
   ball1.dx = ball1.dx + ball1.ddx * dt;
   */

ball1.x = ball1.x + ball1.dx * dt + ball1.ddx * dt * dt / 2.0;

//glutPostRedisplay();

/*ボールと足先の距離*/

dist_x = ball1.x - (leg1.joint_x + leg1.leg_len * sin(ang * M_PI / 180.0) + leg1.leg_wid * cos(ang * M_PI / 180.0));
dist_y = ball1.y - (leg1.joint_y - leg1.leg_len * cos(ang * M_PI / 180.0) - leg1.leg_wid * sin(ang * M_PI / 180.0));

d_ball = sqrt((dist_x * dist_x) + (dist_y * dist_y));

printf("距離\t:%f\nx差\t:%f\ny差\t:%f\n\n", d_ball, dist_x, dist_y);

/*力積による速度設定*/
//ball1.f_x = ball1.m * ball1.dx / dt;
//ball1.f_y = ball1.m * ball1.dy / dt;

/*
   if(leg1.joint_x < ball1.x){
   }else if(leg1.joint_x >= ball1.x){
   }

   if(leg1.joint_y < ball1.x){
   }else if(leg1.joint_y >= ball1.y){
   }
   */

/*ボールの当たり判定*/

if(d_ball <= 8.0 && d_ball >= 5.0){

    /*

    if(leg1.joint_x < ball1.x){
        ball1.dx = dist_x;
    }else if(leg1.joint_x >= ball1.x){
        ball1.dx = -dist_x;
    }

    if(leg1.joint_y < ball1.x){
        ball1.dy = dist_y;
    }else if(leg1.joint_y >= ball1.y){
        ball1.dy = dist_y;
    }
*/

    ball1.dx = dist_x * ball1.f_y / d_ball;
    ball1.dy = dist_y * ball1.f_y / d_ball;


    printf("\n\t接触\t\n\n\n");

    an = false;

}

glutPostRedisplay();

}

void idle(void)
{
    ang = ang + 2.5;
    glutPostRedisplay();
}

void init(void)
{
    glClearColor(0, 0, 0, 0.0);
    glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{
    glViewport( 0, 0, w, h );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-w, w, -h, h, -1.0, 1.0);
    glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if( state == GLUT_DOWN ){
                /*
                   glBegin(GL_POLYGON);
                   glVertex2d(x - 5.0, y + 5.0);
                   glVertex2d(x - 5.0, y - 45.0);
                   glVertex2d(x + 45.0, y - 45.0);
                   glVertex2d(x + 5.0, y + 5.0);
                   glEnd();
                   glutPostRedisplay();
                   */
                //ang = ang + 2.5;
                ang = -60.0;
                an = true;
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            printf("middle");
            break;
        case GLUT_RIGHT_BUTTON:
            printf("right");
            /*
               if( state == GLUT_DOWN ){
               ang = ang + 2.5;
               glutPostRedisplay();
               }*/
            break;
        default:
            break;
    }

    printf(" button is ");

    switch (state) {
        case GLUT_UP:
            printf("up");
            break;
        case GLUT_DOWN:
            printf("down");
            break;
        default:
            break;
    }

    printf(" at (%d, %d)\n", x, y);
}

void keyboard(unsigned char key, int x, int y)
{
    if ( key == '\x1b')
        exit(0);
}

