// OpenGLTemplate_R1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <GL/glew.h>

#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/freeglut.h>
#include "OpenGL445Setup-2026.h"

/* Graphics Pgm 3 for Brian Clark
    EXTRA CREDIT: 
                1. The teapot rotates using a global rotation and OpenGL
                transformation commands.
                2. The gravitational constant for Pluto can be toggled 
                on and off once the animation has begun.


    Program Structure:

    Global Variables:  Position, orientation and state for the teapot,
                        landing pad and other game elements, particularly
                        state variables and constants used for the animation
                        are stored as global variables.

    Display:  The central display event handler is display_func(), which
              orchestrates the entire frame animation process from clear 
              to flush using subordinate drawing functions (beginning with
              "draw"). Variances in state are managed by global variables.

                Objects are position using legacy OpenGL transformstions.

    Animation:  Movement is accomplished using timer callbacks to timer_func()
               which handles teapot movement by calling in-place gravity
               functions getTeapotVelocity() and getTeapotPosition(). 
               Because the Pluto gravity constant can be toggled, these
               functions are used on a frame-by-frame basis.

    Keyboard event:  The keyboard)func() handles all keypresses, which both
                     start the game and set the teapot in motion, and turn
                     Pluto gravity on and off.

*/




// color named color values
fcolor g_c_background{ 1.0f, 1.0f, 1.0f };
fcolor g_c_teapot{ 0.0f, 0.0f, 0.0f };
fcolor g_c_aqua{ 0.0f, 0.8f, 0.8f };
fcolor g_c_red{ 1.0f, 0.0f, 0.0f };
fcolor g_c_green{ 0.117f, 0.843f, 0.375f };

// teapot globals
int g_teapot_timeout = 50;

float g_teapot_xpos = 40.0f;
float g_teapot_ypos = 475.0f;
float g_teapot_zpos = -50.0f;

float g_teapot_xrate = 4.0f;

float g_teapot_yvelocity = 0.0f;

double g_teapot_size = 40.0;

float g_teapot_rot_y = 0.0f;
float g_teapot_rot_y_step = 18.0f;


// teapot scale multipliers - derived from 
// freeglut glutWireTeapot source on GitHub
float teapot_depth_scale = 1.0f;
float teapot_width_scale = 3.2625f;
float teapot_height_scale = 1.575f;


// landing pad globals
fcolor g_landing_pad_color = g_c_red;
float g_landing_pad_top = 7.0f;
float g_landing_pad_center_x = 320.0f;

// gravitational constants
float g_grav_earth = -32.0f;
float g_grav_mars = -12.0f;
float g_grav_pluto = -2.0f;

// gravity specific state
char g_use_grav = ' ';
bool g_is_pluto = false;

// process state
boolean g_is_falling = false;
boolean g_is_at_bottom = false;


float g_platform_y = 475.0f;



bool g_win = false;
bool g_lose = false;

float getTeapotVelocity(float grav, float time, float prevVelocity) {
    // calculate the curent velocity    
    return (grav * time) + prevVelocity;
}

float getTeapotPosition(float grav, float time, float prevVelocity, float prevPosition)
{
    return (grav * time * time) / 2 + (prevVelocity * time) + prevPosition;
}

void timer_func(int value)
{
    if (value == 1) {
        if (g_is_falling) {
            float grav = 0;
            if (g_is_pluto) {
                grav = g_grav_pluto;  // EXTRA CREDIT 
            }
            else if (g_use_grav == 'M')
            {
                grav = g_grav_mars;
            }
            else if (g_use_grav == 'E')
            {
                grav = g_grav_earth;
            }
            // calculate minimum teapot y value
            float miny = 8 + (((float)g_teapot_size / teapot_width_scale) * teapot_height_scale / 2.0);
            // determine teapot vertical position mathematically
            float timePassed = g_teapot_timeout / 1000.0f;
            g_teapot_ypos = getTeapotPosition(grav, timePassed, g_teapot_yvelocity, g_teapot_ypos);
            g_teapot_yvelocity = getTeapotVelocity(grav, timePassed, g_teapot_yvelocity);
            // test to see if we've hit to bottom
            if (g_teapot_ypos <= miny) {
                // stop the fall 
                g_is_falling = false;
                g_is_at_bottom = true;
                g_teapot_ypos = miny;
                // evaluate win state
                float teapot_left = g_teapot_xpos - (g_teapot_size / 2);
                float teapot_right = g_teapot_xpos + (g_teapot_size / 2);

                if (teapot_left <= g_landing_pad_center_x && teapot_right >= g_landing_pad_center_x)
                {
                    g_win = true;
                }
                else
                {
                    g_lose = true;
                }
            }
            // EXTRA CREDIT - Teapot Rotation
            g_teapot_rot_y += g_teapot_rot_y_step;

            glutTimerFunc(g_teapot_timeout, timer_func, value);
            glutPostRedisplay();
        }
    }



}

void keyboard_func(unsigned char keycode, int x, int y) {
    if (g_is_falling == false && g_use_grav == ' ')
    {
        if (keycode == 'M' || keycode == 'm')
        {
            g_use_grav = 'M';
            glutPostRedisplay();
        }
        else if (keycode == 'E' || keycode == 'e') {
            g_use_grav = 'E';
            glutPostRedisplay();
        }
    }
    if (g_use_grav != ' ')
    {
        if (!g_is_at_bottom)
        {
            // handle left/right movement
            if (keycode == 'A' || keycode == 'a' || keycode == 'S' || keycode == 's')
            {
                if (keycode == 'A' || keycode == 'a') {
                    g_teapot_xpos -= g_teapot_xrate;
                }
                else
                {
                    g_teapot_xpos += g_teapot_xrate;
                }

                if (!g_is_falling) {
                    g_is_falling = true;
                    glutTimerFunc(g_teapot_timeout, timer_func, 1);
                }
            }
        }
        
    }
    // EXTRA CREDIT - Pluto Gravity toggle
    if (g_is_falling) {
        if (keycode == 'C' || keycode == 'c') {
            g_is_pluto = !g_is_pluto;
        }
    }
}


void drawStartText()
{
    glColor3f(5.0f, 0.0f, 0.0f);
    glRasterPos2f(220.0f, 337.0f);

    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'P');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 's');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 's');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'M');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'E');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'S');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '.');

    glRasterPos2f(223.0f, 320.0f);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'P');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'r');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'e');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 's');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 's');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'C');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ' ');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 't');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'o');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ' ');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 't');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'o');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'g');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'g');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'l');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'e');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ' ');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'P');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'l');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'u');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 't');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'o');

}

void drawWinText()
{
    glColor3f(g_c_green.r, g_c_green.g, g_c_green.b);
    glRasterPos2f(284.0f, 320.0f);

    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'Y');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'W');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '!');

}

void drawLoseText()
{
    glColor3f(5.0f, 0.0f, 0.0f);
    glRasterPos2f(295.0f, 320.0f);

    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'S');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');

}

void drawLandingPad()
{
    float xlow = 270.0f;
    float xhi = 370.0f;
    float ylow = 5.0f;
    float yhi = 7.0f;
    float zlow = -70.0f;
    float zhi = -30.0f;

    glColor3f(g_landing_pad_color.r, g_landing_pad_color.g, g_landing_pad_color.b);

    glBegin(GL_LINES);

    glVertex3f(xlow, ylow, zlow);
    glVertex3f(xlow, yhi, zlow);

    glVertex3f(xlow, ylow, zlow);
    glVertex3f(xhi, ylow, zlow);

    glVertex3f(xlow, yhi, zlow);
    glVertex3f(xhi, yhi, zlow);

    glVertex3f(xhi, ylow, zlow);
    glVertex3f(xhi, yhi, zlow);

    glVertex3f(xhi, ylow, zlow);
    glVertex3f(xhi, ylow, zhi);

    glVertex3f(xhi, yhi, zlow);
    glVertex3f(xhi, yhi, zhi);

    glVertex3f(xhi, ylow, zhi);
    glVertex3f(xhi, yhi, zhi);

    glVertex3f(xhi, ylow, zhi);
    glVertex3f(xlow, ylow, zhi);

    glVertex3f(xhi, yhi, zhi);
    glVertex3f(xlow, yhi, zhi);

    glVertex3f(xlow, ylow, zhi);
    glVertex3f(xlow, yhi, zhi);

    glVertex3f(xlow, ylow, zhi);
    glVertex3f(xlow, ylow, zlow);

    glVertex3f(xlow, yhi, zhi);
    glVertex3f(xlow, yhi, zlow);

    glEnd();
}

void drawTeapot()
{
    glColor3f(g_c_teapot.r, g_c_teapot.g, g_c_teapot.b);


    // pose the teapot
    glPushMatrix();

    glTranslatef(g_teapot_xpos, g_teapot_ypos, g_teapot_zpos);

    glRotatef(g_teapot_rot_y, 0.0f, 1.0f, 0.0f);

    glutWireTeapot(g_teapot_size / teapot_width_scale);


    glPopMatrix();
}

void drawPlatform()
{
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINES);

    glVertex3f(0.0f, g_platform_y, g_teapot_zpos);
    glVertex3f(50.0f, g_platform_y, g_teapot_zpos);
    glEnd();
}

void display_func()
{
    // this is a display callback to draw a line,
    // learner's not: display callbacks are automatically invoked after GLUT finds that a window needs to be
    // displayed or redisplayed

    glClearColor(g_c_background.r, g_c_background.g, g_c_background.b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (g_use_grav != ' ') {
        drawTeapot();
    }

    drawPlatform();

    // draw water
    glColor3f(g_c_aqua.r, g_c_aqua.g, g_c_aqua.b);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 4.0f, -50.0f);
    glVertex3f(640.0, 4.0f, -50.0f);
    glEnd();

    drawLandingPad();

    if (g_use_grav == ' ')
    {
        drawStartText();
    }
    if (g_win)  drawWinText();
    if (g_lose) drawLoseText();
    
    glFlush();   // may need to uncomment this line!
}

// can customize the below 3 items to make canvas of ones own size and labeling

#define canvas_Width 640
#define canvas_Height 640
char canvas_Name[] = "Program 3 Teapot Toss II";

// #define canvas_name "NAME OF CANVAS";


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    my_setup(canvas_Width, canvas_Height, canvas_Name);
    // calculate the y-position for the teapot to allow it to rest on the platform.
    g_teapot_ypos = g_platform_y + 1 + (((float)g_teapot_size / teapot_width_scale) * teapot_height_scale / 2.0);
    g_teapot_xpos = ((float)g_teapot_size / 2.0);
    glutDisplayFunc(display_func);
    glutKeyboardFunc(keyboard_func);
    glutMainLoop();
    return 0;
}

