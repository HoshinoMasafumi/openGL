/////////////////////////////////////////////////////////////////////
//sでキック開始
//fでキック終了
//qで足の角度を-30度に戻す
//左クリックでその座標を保存
//bでボールの位置を左クリックで保存した位置に移動
//lで足の根本の位置を左クリックで保存した位置に移動
//escでシュミレーションの終
/////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GLUT/glut.h>

#define G 9.8
#define E 0.70
#define rfc 0.10//回転摩擦
#define dt 0.1

/*初期値の設定*/

static GLfloat ang = -30.0;
static GLdouble i = 0.0;
static GLdouble t = 0.0;

static GLdouble d_ball = 0.0;

static GLdouble dist_x = 0.0;
static GLdouble dist_y = 0.0;

static GLdouble height = 200.0;
static GLdouble width = 200.0;

static GLdouble mouse_x = 0.0;
static GLdouble mouse_y = 0.0;

int a;
bool an = false;

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

struct ball ball1 =
{
    0,
    5.0,
    10,     0,
    0.0,    0.0,  0.0,
    0.0,    0.0,            0.0,
    0.0,    -G,             0.0,
    10.0,   0.0
};

struct ball foot =
{
    0,
    3.536,
    50.0,   0,
    0.0,    0.0,    0.0,
    0.0,    0.0,    0.0,
    0.0,    0.0,    0.0,
    0.0,    0.0,
};

struct leg
{
    int id;
    double joint_x, joint_y, leg_tht;
    double leg_hei, leg_wid, leg_len;
    double coord_x, coord_y;
};

struct leg leg1 =
{
    0,
    -15.0,  -50.0,  0.0,
    47.5,   5.0,    47.76243,
    0.0,    0.0
};

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

    ball1.y = -height - ball1.r;

    ball1.f_y = -ball1.m * G;

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
    glRotatef(ang, 0.0, 0.0, 1.0);//回転
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
        ang = ang + 0.5;
        if ( ang > 360.0 )
            ang = ang - 360.0;
    }

    /*ボールの座標・速度・加速度*/
    /*y軸*/

    ball1.ddy = ball1.f_y / ball1.m;
    ball1.y = ball1.y + ball1.dy * dt + ball1.ddy * dt * dt / 2.0;
    ball1.dy = ball1.dy + ball1.ddy * dt;

    /*x軸*/

    //printf("\n\n%f\t%f\n\n", ball1.dy, ball1.ddy * dt);
    //if( ball1.y == -(height - ball1.r) ){
    if( ball1.dy == (ball1.ddy * dt) ){

        if(ball1.dx > 0){

            ball1.ddx = rfc * ball1.f_y / ball1.m;
            ball1.x = ball1.x + ball1.dx * dt + ball1.ddx * dt * dt / 2.0;
            ball1.dx = ball1.dx + ball1.ddx * dt;

        }else if (ball1.dx < 0){

            ball1.ddx = -rfc * ball1.f_y / ball1.m;
            ball1.x = ball1.x + ball1.dx * dt + ball1.ddx * dt * dt / 2.0;
            ball1.dx = ball1.dx + ball1.ddx * dt;

        }

    }else{

        ball1.x = ball1.x + ball1.dx * dt + ball1.ddx * dt * dt / 2.0;

    }

    /*跳ね返り*/
    /*x軸*/

    if (ball1.x >= (width - ball1.r)){
        ball1.x = width - ball1.r;
        ball1.dx = -ball1.dx;
    }else if(ball1.x <= -(width - ball1.r)){
        ball1.x = -(width - ball1.r);
        ball1.dx = -ball1.dx;
    }

    /*y軸*/

    if(ball1.y <= -(height - ball1.r)){
        ball1.dy = -ball1.dy * E;
        //f_y = f_y - E * ( ball1.r - ball1.y + 45);
        ball1.y = -(height - ball1.r);

        if (ball1.dy < 1.7){
            ball1.dy = 0.0;
        }
    }

    //printf("座標\t:%f\t%f\t%f\n速度\t:%f\t%f\t%f\n加速度\t:%f\t%f\t%f\n", ball1.x, ball1.y, ball1.tht, ball1.dx, ball1.dy, ball1.dtht, ball1.ddx, ball1.ddy, ball1.ddtht);

    if ((-0.10 <=  ball1.dx) && (ball1.dx <= 0.10)){
        ball1.dx = 0.0;
    }

    /*ボールと足先の距離*/

    //leg1.coord_x = leg1.joint_x + leg1.leg_hei * sin(ang * M_PI / 180.0) + leg1.leg_wid * cos(ang * M_PI / 180.0);
    //leg1.coord_y = leg1.joint_y - leg1.leg_hei * cos(ang * M_PI / 180.0) - leg1.leg_wid * sin(ang * M_PI / 180.0);

    leg1.coord_x = leg1.joint_x + leg1.leg_len * sin((ang + 6.0) * M_PI / 180.0);
    leg1.coord_y = leg1.joint_y - leg1.leg_len * cos((ang + 6.0)* M_PI / 180.0);

    dist_x = ball1.x - leg1.coord_x;
    dist_y = ball1.y - leg1.coord_y;

    dist_x = fabs(dist_x);
    dist_y = fabs(dist_y);

    d_ball = sqrt((dist_x * dist_x) + (dist_y * dist_y));

    //printf("足座標\t:%f\t%f\n", leg1.coord_x, leg1.coord_y);

    //printf("差xy\t:%f\t%f\n距離\t:%f\n\n", dist_x, dist_y, d_ball);

    /*力積による速度設定*/
    //ball1.f_x = ball1.m * ball1.dx / dt;
    //ball1.f_y = ball1.m * ball1.dy / dt;

    /*ボールの当たり判定*/

    if ( ((ball1.r + foot.r) * (ball1.r + foot.r)) > ((dist_x * dist_x) + (dist_y * dist_y))){

        // if(d_ball <= (ball1.r + foot.r) ){

        if(leg1.joint_x <= ball1.x && leg1.joint_y <= ball1.y)//x+ y+ 右上
        {
            //ball1.dx = dist_x * ball1.f_y / d_ball;
            //ball1.dy = dist_y * ball1.f_y / d_ball;
            ball1.dx = dist_x;
            ball1.dy = dist_y;

            printf("x+ y+\n");

        }else if(leg1.joint_x <= ball1.x && leg1.joint_y > ball1.y)//x+ y- 右下
        {
            //ball1.dx = dist_x * ball1.f_y / d_ball;
            //ball1.dy = -dist_y * ball1.f_y / d_ball;
            ball1.dx = dist_x;
            ball1.dy = -dist_y;

            printf("x+ y-\n");

        }else if(leg1.joint_x > ball1.x && leg1.joint_y > ball1.y)//x- y- 左下
        {
            //ball1.dx = -dist_x * ball1.f_y / d_ball;
            //ball1.dy = -dist_y * ball1.f_y / d_ball;
            ball1.dx = -dist_x;
            ball1.dy = -dist_y;

            printf("x- y-\n");

        }else if(leg1.joint_x > ball1.x && leg1.joint_y <= ball1.y)//x- y+ 左上
        {
            //ball1.dx = -dist_x * ball1.f_y / d_ball;
            //ball1.dy = dist_y * ball1.f_y / d_ball;
            ball1.dx = -dist_x;
            ball1.dy = dist_y;

            printf("x- y+\n");

        }

        ball1.dx = ball1.dx * 10;
        ball1.dy = ball1.dy * 10;

        //printf("接触\t\n\n\n");

        an = false;
        //ang = -60.0;

        printf("%f\n", 180 * atan(dist_y/dist_x) / M_PI);
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
        glOrtho(-width, width, -height, height, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    }

    void mouse(int button, int state, int x, int y)
    {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                if( state == GLUT_DOWN ){

                    mouse_x = x;
                    mouse_y = y;

                    if ((mouse_x >= 0) && (mouse_x < 200) && (mouse_y >= 0) && (mouse_y < 200)){//x-,y+
                        mouse_x = -(200 - mouse_x);
                        mouse_y = 200 - mouse_y;
                    }else if ((mouse_x >= 200) && (mouse_x <= 400) && (mouse_y >= 0) && (mouse_y < 200)){//x+,y+
                        mouse_x = mouse_x - 200;
                        mouse_y = 200 - mouse_y;
                    }else if ((mouse_x >= 0) && (mouse_x < 200) && (mouse_y >= 200) && (mouse_y <= 400)){//x-,y-
                        mouse_x = -(200 - mouse_x);
                        mouse_y = -(mouse_y - 200);
                    }else if ((mouse_x >= 200) && (mouse_x <= 400) && (mouse_y >= 200) && (mouse_y <= 400)){//x+,y-
                        mouse_x = mouse_x - 200;
                        mouse_y = -(mouse_y - 200);
                    }

                    //an = true;
                }
                break;
            case GLUT_MIDDLE_BUTTON:
                printf("middle");
                break;
            case GLUT_RIGHT_BUTTON:
                printf("right");
                if( state == GLUT_DOWN ){
                    /*
                       ang = ang + 2.5;
                       glutPostRedisplay();
                       */
                    //an = false;
                }
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
        if ( key == 'q' )
            ang = -30;
        if ( key == 's' )
            an = true;
        if ( key == 'f' )
            an = false;
        if ( key == 'b' ){
            ball1.x = mouse_x;
            ball1.y = mouse_y;
        }
        if ( key == 'l'){
            leg1.joint_x = mouse_x;
            leg1.joint_y = mouse_y;
        }

    }

