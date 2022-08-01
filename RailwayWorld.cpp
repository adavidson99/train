//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  FILE NAME: RailwayWorld.cpp
//  See Lab02.pdf for details
//  ========================================================================

#define _USE_MATH_DEFINES 
#include <math.h>
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <climits>
#include "RailModels.h"


int arm_2 = 1;
float arm_angle2 = 90;
float arm_angle = 90;
float angle = 0;
float cam_angle = 0;
float cam_angle_2 = 0;
float cam_hgt = 50;
float eye_x = -80;
float eye_z = 250;
float look_x = 0;
float look_z = 0;

int count = 0;
int cam2_reset = 0;

//---------------------------------------------------------------------
void initialize(void)
{
    float grey[4] = { 0.2, 0.2, 0.2, 1.0 };
    float grey2[4] = { 0.5, 0.5, 0.5, 1.0 };
    float white[4] = { 1.0, 1.0, 1.0, 1.0 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHT1);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 100);

    //	Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, grey2);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.01);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0, 0.0, 0.0, 0.0);  //Background colour

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60., 1.0, 10.0, 1000.0);   //Perspective projection
}

//-------------------------------------------------------------------
void display(void)
{
    float lgt_pos[] = { 0.0f, 50.0f, 0.0f, 1.0f };  //light0 position (directly above the origin)

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye_x, cam_hgt, eye_z, look_x, 0.0, look_z, 0.0, 1.0, 0.0);
    glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position

    glRotatef(cam_angle_2, 0, 1, 0);   //rotate the entire scene

    floor();
    tracks(150, 10);  //mean radius 150 units, width 10 units
    railway_sleeper();

    glPushMatrix();
    glRotatef(angle, 0, 1, 0);
    glTranslatef(0, 1, -150);
    float lgt1_pos[] = { -10.0f, 14.0f, 0.0f, 1.0f };
    float lgt1_dir[] = { -1.0f, -1.0f, 0.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lgt1_pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lgt1_dir);
    glPopMatrix();

    glPushMatrix();
    glRotatef(angle - 3, 0, 1, 0);
    glTranslatef(0, 1, -150);
    engine();
    glPopMatrix();

    glPushMatrix();
    glRotatef(angle - 15, 0, 1, 0);
    glTranslatef(0, 1, -150);
    wagon();
    glPopMatrix();

    glPushMatrix();
    glRotatef(angle - 30, 0, 1, 0);
    glTranslatef(0, 1, -150);
    wagon();
    glPopMatrix();

    glPushMatrix();
    glRotatef(angle - 45, 0, 1, 0);
    glTranslatef(0, 1, -150);
    wagon();
    glPopMatrix();

    glPushMatrix();
    glRotatef(angle - 60, 0, 1, 0);
    glTranslatef(0, 1, -150);
    wagon();
    glPopMatrix();

    glPushMatrix();
    station();
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    tunnel();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-60, 0, 70);
    glRotatef(-90, 0, 1, 0);
    road();
    glPopMatrix();

    glPushMatrix();
    barrier_arm();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90, 10, 95);
    glRotatef(arm_angle, 0, 0, 1);
    arm();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-60, 10, 160);
    glRotatef(arm_angle2, 0, 0, 1);
    arm();
    glPopMatrix();


    glutSwapBuffers();   //Useful for animation
}


void train_timer(int value)
{
    if (value < 200000)
    {
        angle += 0.5;

        if (fmod(angle, 360) > 80 and fmod(angle, 360) <= 125)
        {
            arm_angle -= 1;
            arm_angle2 += 1;
        }

        else if (fmod(angle, 360) >= 200 and fmod(angle, 360) < 245)
        {
            arm_angle += 1;
            arm_angle2 -= 1;
        }

        if (fmodf(angle, 360) == 330)
        {
            glutPostRedisplay();
            glutTimerFunc(5000, train_timer, value);
        }
        else
        {
            if ((count % 3) == 2)
            {
                if (cam2_reset == 0)
                {
                    cam_angle_2 = (-90 - angle);
                    cam2_reset += 1;
                }
                cam_angle_2 -= 0.5;
            }
            glRotatef(angle, 0, 1, 0);
            glutPostRedisplay();
            value++;
            glutTimerFunc(30, train_timer, value);
        }
    }
}


// Move the camera with the keyboard arrow inputs
void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT) cam_angle_2 += 4;        //Rotate scene
    else if (key == GLUT_KEY_RIGHT) cam_angle_2 -= 4;
    else if (key == GLUT_KEY_UP)    //move forward
    {
        eye_x += 3 * sin(cam_angle);     
        eye_z -= 3 * cos(cam_angle);
    }
    else if (key == GLUT_KEY_DOWN)
    {  //Move backward
        eye_x -= 3 * sin(cam_angle);   // move backwards
        eye_z += 3 * cos(cam_angle);
    }
    else if (key == GLUT_KEY_PAGE_UP)
    {
        cam_hgt+=4;
    }
    else if (key == GLUT_KEY_PAGE_DOWN)
    {
        cam_hgt-=4;
    }

    if (cam_hgt < 5) cam_hgt = 5;
    else if (cam_hgt > 500) cam_hgt = 500;

    glutPostRedisplay();
}


void normal_key(unsigned char key, int x, int y)
{
    if (key == 'c')
    {
        count++;

        if ((count % 3) == 1)
        {
            eye_x = 190;
            eye_z = 35;
            cam_hgt = 20;
            cam_angle_2 = -60;
            look_x = 0;
            look_z = 0;
            cam2_reset = 0;
            glutPostRedisplay();
        }

        if ((count % 3) == 2)
        {
            eye_x = 160;
            eye_z = 0;
            cam_hgt = 15;
            look_x = 10;
            look_z = -250;
            glutPostRedisplay();
        }

        if ((count % 3) == 0)
        {
            eye_x = -80;
            eye_z = 250;
            cam_hgt = 50;
            cam_angle_2 = 0;
            look_x = 0;
            look_z = 0;
            glutPostRedisplay();
        }
    }
}

//---------------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Toy Train");
    initialize();
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutKeyboardFunc(normal_key);
    glutTimerFunc(30, train_timer, 0);
    glutMainLoop();
    return 0;
}
