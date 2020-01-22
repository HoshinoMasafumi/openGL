#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GLUT/glut.h>

#define G 9.8
#define E 0.70

int a;

struct ball
{
    int id;
    double r; /* radius */
    double m, I; /* mass and inertial */
    double x, y, tht;/*座標*/
    double dx, dy, dtht;/*速度*/
    double ddx, ddy, ddtht;/*加速度*/
};

struct ball ball1, leg_ball;

struct leg
{
    int id;
    double joint_x, joint_y, leg_tht;
    struct ball foot;
    double leg_len;
};

struct leg leg1;

static GLfloat ang = 0.0;
static GLdouble i = 0.0;
static GLdouble time_1 = 0.00;
static GLdouble count_1 = 0.0;
static GLdouble count_2 = 0.0;
static GLdouble count_up = 0.3;
static GLdouble f_x = 10;
static GLdouble f_y = 10;
static GLdouble tai_z = 10;
static GLdouble t = 0;
static GLdouble dt = 0.1;

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
    glVertex2d(-2.5, -44.43);
    glVertex2d(2.5, -44.43);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2d(-2.5, -44.43);
    glVertex2d(-2.5, -49.43);
    glVertex2d(7.5, -49.43);
    glVertex2d(7.5, -44.43);
    glEnd();

    glPushMatrix();
    glTranslatef(5.0, -46.93, 0.0);
    ball(leg_ball.r);
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
    ang = ang + 2.5;
    if ( ang > 360.0 )
        ang = ang - 360.0;
    //glutPostRedisplay();//再描画

    /*跳ね返りの条件*/
    if (ball1.x >= 45){
        ball1.x = 45;
        ball1.dx = -ball1.dx;
    }else if(ball1.x <= -45){
        ball1.x = -45;
        ball1.dx = -ball1.dx;
    }

    if (ball1.y >= 45){
        ball1.y = 45;
        ball1.dy = -ball1.dy;
    }else if(ball1.y <= -45){
        ball1.dy = -ball1.dy * E;
        //f_y = f_y - E * ( ball1.r - ball1.y + 45);
        ball1.y = -45;
    }

    printf("座標:%f %f %f \n速度:%f %f %f \n加速度:%f %f %f\n\n", ball1.x, ball1.y, ball1.tht, ball1.dx, ball1.dy, ball1.dtht, ball1.ddx, ball1.ddy, ball1.ddtht);

    glutPostRedisplay();

    /*ボールの座標・速度・加速度*/

    //ball1.ddx = f_x / ball1.m;
    //ball1.ddy = -f_y / ball1.m;
    ball1.ddx = 0;
    ball1.ddy = -G;
    ball1.ddtht = tai_z / ball1.I;

    ball1.x = ball1.x + ball1.dx * dt + ball1.ddx * dt * dt / 2.0;
    ball1.y = ball1.y + ball1.dy * dt + ball1.ddy * dt * dt / 2.0;
    ball1.tht = ball1.tht + ball1.dtht * dt + ball1.ddtht * dt * dt / 2.0;

    ball1.dx = ball1.dx + ball1.ddx * dt;
    //ball1.dx = 10;
    ball1.dy = ball1.dy + ball1.ddy * dt;
    ball1.dtht = ball1.dtht + ball1.ddtht * dt;

    t = t + dt;


/*
    ball1.ddx = f_x / ball1.m;
    ball1.x = ball1.x + ball1.dx * dt + ball1.ddx * dt * dt / 2.0;
    ball1.dx = ball1.dx + ball1.ddx * dt;

    ball1.ddy = f_y / ball1.m;
    ball1.y = ball1.y + ball1.dy * dt + ball1.ddy * dt * dt / 2.0;
    ball1.dy = ball1.dy + ball1.ddy * dt;

    ball1.ddtht = tai_z / ball1.I;
    ball1.tht = ball1.tht + ball1.dtht * dt + ball1.ddtht * dt * dt / 2.0;
    ball1.dtht = ball1.dtht + ball1.ddtht * dt;
*/

    //printf("%f %f %f \n%f %f %f \n%f %f %f\n\n", ball1.x, ball1.y, ball1.tht, ball1.dx, ball1.dy, ball1.dtht, ball1.ddx, ball1.ddy, ball1.ddtht);


    //glutPostRedisplay();

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
                glBegin(GL_POLYGON);
                glVertex2d(x - 5.0, y + 5.0);
                glVertex2d(x - 5.0, y - 45.0);
                glVertex2d(x + 45.0, y - 45.0);
                glVertex2d(x + 5.0, y + 5.0);
                glEnd();
                glutPostRedisplay();
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            printf("middle");
            break;
        case GLUT_RIGHT_BUTTON:
            printf("right");
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

int main(int argc, char *argv[])
{

    ball1.m = 100;

    f_x = 100;
    f_y = -ball1.m * G;
    tai_z = 100;

    //ball1.ddy = -9.8;
    ball1.m = 100;

    leg1.joint_x = -15;
    leg1.joint_y = 0;

    ball1.r = 5;
    leg_ball.r = 3.536;

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
