/*  Tyler Lugger
 *
 *  Computer Graphics Final Project
 *
 *  Key bindings:
 *  o          Open/Close cabin door
 *  d          Toggles day/night mode
 *  f          Toggles fpv/perspective views
 *  w          Increases camera view angle
 *  s          Decreases camera view angle
 *  c          Move camera directly to the ghost (cheat to win)
 *  l          Toggles lighting
 *  a/A        Decrease/increase ambient light
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode
 *  F3         Toggle light distance (1/5)
 *  F8         Change ball increment
 *  m          Toggles light movement
 *  []         Lower/rise light
 *  x          Toggle axes
 *  arrows     look left/right, move forward/backward
 *  PgDn/PgUp  Zoom in and out
 *  ESC        Exit
 */
#include "CSCIx229.h"
#include <time.h>


// Globals
int axes=0;       //  Display axes
int day = 1;      //  Day/Night mode
int move=1;       //  Move light
int side=0;       //  Two sided mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=10.0;   //  Size of world
int num = 500; // Draw a ton of squares to make up the ground so the flashlight looks real
int fpv = 1; // FPV mode
int l = 0; // Global variable for look angle
double Fx = 0.0; // Global variable for camera x pos
double Fy = 0.08; // Global variable for camera y pos
double Fz = 0.0; // Global variable for camera z pos
double Lx = 0; // Some large radius for the fpv camera to move around
double Ly = 0.08;
double Lz = 10; // Some large radius for the fpv camera to move around // Global variables for what camera looks at
// Light values
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int Th=0,Ph=30;   //  Light angles
float Exp=0;      //  Spot exponent
int at0=100;      //  Constant  attenuation %
int at1=0;        //  Linear    attenuation %
int at2=0;        //  Quadratic attenuation %
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;
float sco = 180;    //  Spot cutoff angle
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   3.0;  // Elevation of light
int rep       =   1;
int sky[6];
unsigned int texture[12];
double targetPos[2];
double Heights[350];
int door = 1;



/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
        double x = Sin(th)*Cos(ph);
        double y = Sin(ph);
        double z = Cos(th)*Cos(ph);
        //  For a sphere at the origin, the position
        //  and normal vectors are the same
        glNormal3d(x,y,z);
        glTexCoord2d(th/(360.0/3),ph/((180.0+0.5)/3));
        glVertex3d(x,y,z);
}


static void DVertex(double th,double ph)
{
        double x = -Sin(th)*Cos(ph);
        double y =  Cos(th)*Cos(ph);
        double z =          Sin(ph);
        //  For a sphere at the origin, the position
        //  and normal vectors are the same
        glNormal3d(x,fabs(y),z);
        glTexCoord2d(th/(360.0/3),ph/((180.0+0.5)/3));
        glVertex3d(x,fabs(y),z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
        int th,ph;
        float yellow[] = {1.0,1.0,0.0,1.0};
        float Emission[]  = {0.0,0.0,0.01*emission,1.0};
        //  Save transformation
        glPushMatrix();
        //  Offset, scale and rotate
        glTranslated(x,y,z);
        glScaled(r,r,r);
        //  White ball
        glColor3f(1,1,1);
        glMaterialf(GL_FRONT,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
        glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
        //  Bands of latitude
        for (ph=-90; ph<90; ph+=inc)
        {
                glBegin(GL_QUAD_STRIP);
                for (th=0; th<=360; th+=2*inc)
                {
                        Vertex(th,ph);
                        Vertex(th,ph+inc);
                }
                glEnd();
        }
        //  Undo transofrmations
        glPopMatrix();
}



static void ground(double r){

        double mul = 2.0/num;
        //  Set specular color to white
        float white[] = {1,1,1,1};
        float Emission[]  = {0.0,0.0,0.01*emission,1.0};
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
        glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

        glPushMatrix();
        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glColor3f(1,1,1);
        glBindTexture(GL_TEXTURE_2D,texture[0]);
        glNormal3f(0,1,0);
        glBegin(GL_QUADS);
        for (int i=0; i<num; i++)
                for (int j=0; j<num; j++)
                {
                        glTexCoord2d(mul*(i+0)*10,mul*(j+0)*10); glVertex3f(5*mul*(i+0)-5,0,5*mul*(j+0)-5);
                        glTexCoord2d(mul*(i+1)*10,mul*(j+0)*10); glVertex3f(5*mul*(i+1)-5,0,5*mul*(j+0)-5);
                        glTexCoord2d(mul*(i+1)*10,mul*(j+1)*10); glVertex3f(5*mul*(i+1)-5,0,5*mul*(j+1)-5);
                        glTexCoord2d(mul*(i+0)*10,mul*(j+1)*10); glVertex3f(5*mul*(i+0)-5,0,5*mul*(j+1)-5);
                }
        glEnd();
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
}

static void tree(double x, double z, double radius, double height){
        //  Set specular color to white
        float white[] = {1,1,1,1};
        float Emission[]  = {0.0,0.0,0.01*emission,1.0};
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
        glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

        glPushMatrix();
        glTranslated(x,0,z);


        double angle = 0.0;
        double angle_stepsize = 0.1;

        /** Draw the tube */
        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[1]);

        glColor3f(1,1,1);
        glBegin(GL_QUAD_STRIP);
        //angle = 0.0;
        while( angle < 2*M_PI+.1 ) {
                double c = radius * cos(angle);
                double s = radius * sin(angle);
                double c2 = radius/8 * cos(angle);
                double s2 = radius/8 * sin(angle);
                glNormal3f(cos(angle), 0, sin(angle));
                glTexCoord2f((angle / (2*M_PI+.1)), 1); glVertex3f(c2, height, s2);
                glTexCoord2f((angle / (2*M_PI+.1)), 0); glVertex3f(c, 0.0, s);
                angle = angle + angle_stepsize;
        }

        glEnd();
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);


        // Draw sphere on top of cylinder
        glPushMatrix();
        glTranslated(x,height,z);
        glScaled(4*radius,4*radius,4*radius);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[2]);

        const double d = 5;
        glBegin(GL_TRIANGLE_FAN);
        Vertex(0,-90);
        for (int th=0; th<=360; th+=d)
        {
                Vertex(th,d-90);
        }
        glEnd();

        //  Latitude bands
        for ( int ph=d-90; ph<=90-2*d; ph+=d)
        {
                glBegin(GL_QUAD_STRIP);
                for (int th=0; th<=360; th+=d)
                {
                        Vertex(th,ph);
                        Vertex(th,ph+d);
                }
                glEnd();


        }
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

}

/*
 *  Draw sky box
 */
static void Sky(double D)
{
        glColor3f(1,1,1);
        glEnable(GL_TEXTURE_2D);

        //  Sides
        glBindTexture(GL_TEXTURE_2D,sky[0]);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-D,-D,-D);
        glTexCoord2f(1,0); glVertex3f(+D,-D,-D);
        glTexCoord2f(1,1); glVertex3f(+D,+D,-D);
        glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,sky[1]);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(+D,-D,-D);
        glTexCoord2f(1,0); glVertex3f(+D,-D,+D);
        glTexCoord2f(1,1); glVertex3f(+D,+D,+D);
        glTexCoord2f(0,1); glVertex3f(+D,+D,-D);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,sky[2]);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(+D,-D,+D);
        glTexCoord2f(1,0); glVertex3f(-D,-D,+D);
        glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
        glTexCoord2f(0,1); glVertex3f(+D,+D,+D);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,sky[3]);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-D,-D,+D);
        glTexCoord2f(1,0); glVertex3f(-D,-D,-D);
        glTexCoord2f(1,1); glVertex3f(-D,+D,-D);
        glTexCoord2f(0,1); glVertex3f(-D,+D,+D);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,sky[4]);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(+D,+D,-D);
        glTexCoord2f(1,0); glVertex3f(+D,+D,+D);
        glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
        glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,sky[5]);
        glBegin(GL_QUADS);
        glTexCoord2f(1,1); glVertex3f(-D,-D,+D);
        glTexCoord2f(0,1); glVertex3f(+D,-D,+D);
        glTexCoord2f(0,0); glVertex3f(+D,-D,-D);
        glTexCoord2f(1,0); glVertex3f(-D,-D,-D);
        glEnd();

        glDisable(GL_TEXTURE_2D);
}

static void target(double x, double z, double r){
        float white[] = {1,1,1,1};
        float Emission[]  = {0.0,0.0,0.01*emission,1.0};
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
        glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

        glPushMatrix();
        glTranslated(x, .03, z);
        glScaled(r,r*4,r); // Make half dome
        glColor4f(.5,1,.5,0.1*100);

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[4]);
        //  Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        // Draw dome on top of nose cone
        int theta, phi;

        for (phi=0; phi<360; phi+=inc) {
                glBegin(GL_QUAD_STRIP);
                for (theta=0; theta<=90; theta+=inc) {
                        DVertex(theta,phi);
                        DVertex(theta,phi+inc);
                }
                glEnd();
        }
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
}

static void firewood(double x, double y, double z, double radius, double height, double th){
        float white[] = {1,1,1,1};
        float Emission[]  = {0.0,0.0,0.01*emission,1.0};
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
        glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

        //  Save transformation
        glPushMatrix();
        //  Offset
        glTranslated(x,y,z);
        glRotated(th,0,0,1);

        double angle = 0.0;
        double angle_stepsize = 0.1;

        /** Draw the tube */
        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[1]);

        glColor3f(1,1,1);
        glBegin(GL_QUAD_STRIP);
        //angle = 0.0;
        while( angle < 2*M_PI+.1 ) {
                double c = radius * cos(angle);
                double s = radius * sin(angle);
                glNormal3f(cos(angle), 0, sin(angle));
                glTexCoord2f((angle / (2*M_PI+.1)), .2); glVertex3f(c, height, s);
                glTexCoord2f((angle / (2*M_PI+.1)), 0); glVertex3f(c, 0.0, s);
                angle = angle + angle_stepsize;
        }

        glEnd();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[9]);


        glNormal3f(0,1,0);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5,0.5);
        glVertex3f(0,height,0);
        for (int k=0; k<=360; k+=10)
        {
                glTexCoord2f(0.4*Cos(k)+0.5,0.4*Sin(k)+0.5);
                glVertex3f(radius*Cos(k), height,radius*Sin(k));
        }
        glEnd();


        glPopMatrix();
        glDisable(GL_TEXTURE_2D);

}

static void cabin(double x, double y, double z, double dx, double dy, double dz, double th, int closed){
        float white[] = {1,1,1,1};
        float Emission[]  = {0.0,0.0,0.01*emission,1.0};
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
        glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

        //  Save transformation
        glPushMatrix();
        //  Offset
        glTranslated(x,y,z);
        glRotated(th,0,1,0);
        glScaled(dx,dy,dz);

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[3]);

        //  Walls
        glBegin(GL_QUADS);
        //  Front
        glColor3f(1, 1, 1);
        glNormal3f(0,0,1);

        glTexCoord2f(0,.5); glVertex3f(-1,.5, 1);
        glTexCoord2f(0,1); glVertex3f(-1,1, 1);
        glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
        glTexCoord2f(1,.5); glVertex3f(1,.5, 1);

        glTexCoord2f(0,0); glVertex3f(-1,0, 1);
        glTexCoord2f(0,.5); glVertex3f(-1,.5, 1);
        glTexCoord2f(.4,.5); glVertex3f(-.3,.5, 1);
        glTexCoord2f(.4,0); glVertex3f(-.3,0, 1);

        glTexCoord2f(.6,0); glVertex3f(.3,0, 1);
        glTexCoord2f(.6,.5); glVertex3f(.3,.5, 1);
        glTexCoord2f(1,.5); glVertex3f(1,.5, 1);
        glTexCoord2f(1,0); glVertex3f(1,0, 1);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Door
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[5]);
        if (closed) {
                glBegin(GL_QUADS);
                glNormal3f(0,0,1);
                glTexCoord2f(0,0); glVertex3f(-.3,0, 1);
                glTexCoord2f(0,1); glVertex3f(-.3,.5, 1);
                glTexCoord2f(1,1); glVertex3f(.3,.5, 1);
                glTexCoord2f(1,0); glVertex3f(.3,0, 1);
                glEnd();
        }
        else{
                glBegin(GL_QUADS);
                glNormal3f(1,0,0);
                glTexCoord2f(0,0); glVertex3f(-.3,0, 1);
                glTexCoord2f(0,1); glVertex3f(-.3,.5, 1);
                glTexCoord2f(.5,1); glVertex3f(-.3,.5, 1.3);
                glTexCoord2f(.5,0); glVertex3f(-.3,0, 1.3);
                glEnd();

                glBegin(GL_QUADS);
                glNormal3f(1,0,0);
                glTexCoord2f(1,0); glVertex3f(.3,0, 1);
                glTexCoord2f(1,1); glVertex3f(.3,.5, 1);
                glTexCoord2f(.5,1); glVertex3f(.3,.5, 1.3);
                glTexCoord2f(.5,0); glVertex3f(.3,0, 1.3);
                glEnd();

        }

        glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[3]);
        glBegin(GL_QUADS);
        //  Back
        glNormal3f(0, 0, -1);
        glTexCoord2f(1,0); glVertex3f(+1,0,-1);
        glTexCoord2f(0,0); glVertex3f(-1,0,-1);
        glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
        glTexCoord2f(1,1); glVertex3f(+1,+1,-1);


        //  Right
        glNormal3f(1, 0, 0);
        glTexCoord2f(1,0); glVertex3f(+1,0,+1);
        glTexCoord2f(0,0); glVertex3f(+1,0,-1);
        glTexCoord2f(0,.2); glVertex3f(+1,+.2,-1);
        glTexCoord2f(1,.2); glVertex3f(+1,+.2,+1);

        glTexCoord2f(1,.4); glVertex3f(+1,.4,+1);
        glTexCoord2f(0,.4); glVertex3f(+1,.4,-1);
        glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
        glTexCoord2f(1,1); glVertex3f(+1,+1,+1);

        glTexCoord2f(.4,.2); glVertex3f(+1,.2,-.2);
        glTexCoord2f(0,.2); glVertex3f(+1,.2,-1);
        glTexCoord2f(0,.4); glVertex3f(+1,.4,-1);
        glTexCoord2f(.4,.4); glVertex3f(+1,.4,-.2);

        glTexCoord2f(1,.2); glVertex3f(+1,.2,.2);
        glTexCoord2f(.6,.2); glVertex3f(+1,.2,1);
        glTexCoord2f(.6,.4); glVertex3f(+1,.4,1);
        glTexCoord2f(1,.4); glVertex3f(+1,.4,.2);

        glEnd();
        glDisable(GL_TEXTURE_2D);


        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[3]);
        glBegin(GL_QUADS);
        //  Left
        glNormal3f(-1, 0, 0);
        glTexCoord2f(1,0); glVertex3f(-1,0,+1);
        glTexCoord2f(0,0); glVertex3f(-1,0,-1);
        glTexCoord2f(0,.2); glVertex3f(-1,+.2,-1);
        glTexCoord2f(1,.2); glVertex3f(-1,+.2,+1);

        glTexCoord2f(1,.4); glVertex3f(-1,.4,+1);
        glTexCoord2f(0,.4); glVertex3f(-1,.4,-1);
        glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
        glTexCoord2f(1,1); glVertex3f(-1,+1,+1);

        glTexCoord2f(.4,.2); glVertex3f(-1,.2,-.2);
        glTexCoord2f(0,.2); glVertex3f(-1,.2,-1);
        glTexCoord2f(0,.4); glVertex3f(-1,.4,-1);
        glTexCoord2f(.4,.4); glVertex3f(-1,.4,-.2);

        glTexCoord2f(1,.2); glVertex3f(-1,.2,.2);
        glTexCoord2f(.6,.2); glVertex3f(-1,.2,1);
        glTexCoord2f(.6,.4); glVertex3f(-1,.4,1);
        glTexCoord2f(1,.4); glVertex3f(-1,.4,.2);

        glEnd();
        glDisable(GL_TEXTURE_2D);


        // Roof
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[6]);

        glBegin(GL_QUADS);
        glNormal3f(-.5, 1, 0);
        glTexCoord2f(1,0); glVertex3f(-1,+1,+1);
        glTexCoord2f(0,0); glVertex3f(-1,+1,-1);
        glTexCoord2f(0,1); glVertex3f(+0,+1.5,-1);
        glTexCoord2f(1,1); glVertex3f(+0,+1.5,+1);


        glNormal3f(.5, 1, 0);
        glTexCoord2f(1,1); glVertex3f(0,+1.5,+1);
        glTexCoord2f(0,1); glVertex3f(0,+1.5,-1);
        glTexCoord2f(0,0); glVertex3f(+1,+1,-1);
        glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[3]);

        glBegin(GL_TRIANGLES);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0,0); glVertex3f(-1, +1, +1);
        glTexCoord2f(.5,.5); glVertex3f(0, 1.5, +1);
        glTexCoord2f(1,0); glVertex3f(+1, +1, +1);

        glNormal3f(0, 0, -1);
        glTexCoord2f(0,0); glVertex3f(-1, +1, -1);
        glTexCoord2f(.5,.5); glVertex3f(0, 1.5, -1);
        glTexCoord2f(1,0); glVertex3f(+1, +1, -1);
        //  End
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Draw metal light above door
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[8]);

        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0,0); glVertex3f(-.01, 1.04, 1);
        glTexCoord2f(0,1); glVertex3f(-.01, 1.05, 1);
        glTexCoord2f(10,1); glVertex3f(-.01, 1.05, 1.2);
        glTexCoord2f(10,0); glVertex3f(-.01, 1.04, 1.2);

        glNormal3f(1, 0, 0);
        glTexCoord2f(0,0); glVertex3f(.01, 1.04, 1);
        glTexCoord2f(0,1); glVertex3f(.01, 1.05, 1);
        glTexCoord2f(10,1); glVertex3f(.01, 1.05, 1.2);
        glTexCoord2f(10,0); glVertex3f(.01, 1.04, 1.2);

        glNormal3f(0, 1, 0);
        glTexCoord2f(0,0); glVertex3f(-.01, 1.05, 1);
        glTexCoord2f(1,0); glVertex3f(.01, 1.05, 1);
        glTexCoord2f(1,10); glVertex3f(.01, 1.05, 1.2);
        glTexCoord2f(0,10); glVertex3f(-.01, 1.05, 1.2);

        glNormal3f(0, -1, 0);
        glTexCoord2f(0,0); glVertex3f(-.01, 1.04, 1);
        glTexCoord2f(1,0); glVertex3f(.01, 1.04, 1);
        glTexCoord2f(1,10); glVertex3f(.01, 1.04, 1.2);
        glTexCoord2f(0,10); glVertex3f(-.01, 1.04, 1.2);

        glNormal3f(0, 0, 1);
        glTexCoord2f(0,0); glVertex3f(-.01, 1.04, 1.2);
        glTexCoord2f(0,1); glVertex3f(-.01, 1.05, 1.2);
        glTexCoord2f(1,1); glVertex3f(.01, 1.05, 1.2);
        glTexCoord2f(1,0); glVertex3f(.01, 1.04, 1.2);

        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Draw fireplace inside the cabin

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[10]);

        glBegin(GL_QUADS);

        glNormal3f(-1, 0, 0);
        glTexCoord2f(0,0); glVertex3f(0, 0, -.99);
        glTexCoord2f(2,0); glVertex3f(0, 0, -.75);
        glTexCoord2f(2,2); glVertex3f(0, .25,-.75);
        glTexCoord2f(0,2); glVertex3f(0, .25,-.99);

        glNormal3f(1, 0, 0);
        glTexCoord2f(0,0); glVertex3f(.5, 0, -.99);
        glTexCoord2f(1,0); glVertex3f(.5, 0, -.75);
        glTexCoord2f(1,1); glVertex3f(.5, .25,-.75);
        glTexCoord2f(0,1); glVertex3f(.5, .25,-.99);

        glNormal3f(0, 1, 0);
        glTexCoord2f(0,0); glVertex3f(0, .25, -.99);
        glTexCoord2f(0,2); glVertex3f(0, .25, -.75);
        glTexCoord2f(2,2); glVertex3f(.5, .25, -.75);
        glTexCoord2f(2,0); glVertex3f(.5, .25, -.99);


        glNormal3f(-1, 0, 0);
        glTexCoord2f(0,0); glVertex3f(.15, .25, -.99);
        glTexCoord2f(.9,0); glVertex3f(.15, .25, -.75);
        glTexCoord2f(.9,12); glVertex3f(.15, 1.7,-.75);
        glTexCoord2f(0,12); glVertex3f(.15, 1.7,-.99);

        glNormal3f(1, 0, 0);
        glTexCoord2f(0,0); glVertex3f(.35, .25, -.99);
        glTexCoord2f(.9,0); glVertex3f(.35, .25, -.75);
        glTexCoord2f(.9,12); glVertex3f(.35, 1.7,-.75);
        glTexCoord2f(0,12); glVertex3f(.35, 1.7,-.99);

        glNormal3f(0, 0, 1);
        glTexCoord2f(0,0); glVertex3f(.15, .25, -.75);
        glTexCoord2f(.9,0); glVertex3f(.35, .25, -.75);
        glTexCoord2f(.9,12); glVertex3f(.35, 1.7,-.75);
        glTexCoord2f(0,12); glVertex3f(.15, 1.7,-.75);

        glNormal3f(0, 0, -1);
        glTexCoord2f(0,0); glVertex3f(.15, .25, -.99);
        glTexCoord2f(.9,0); glVertex3f(.35, .25, -.99);
        glTexCoord2f(.9,12); glVertex3f(.35, 1.7,-.99);
        glTexCoord2f(0,12); glVertex3f(.15, 1.7,-.99);

        glEnd();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[11]);

        glBegin(GL_QUADS);

        glNormal3f(0, 0, 1);
        glTexCoord2f(0,0); glVertex3f(0, 0, -.75);
        glTexCoord2f(1,0); glVertex3f(.5, 0, -.75);
        glTexCoord2f(1,1); glVertex3f(.5, .25,-.75);
        glTexCoord2f(0,1); glVertex3f(0, .25,-.75);

        glEnd();
        glDisable(GL_TEXTURE_2D);

        double angle = 0.0;
        double angle_stepsize = 0.1;

        /** Draw the tube */
        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[8]);

        glBegin(GL_QUAD_STRIP);
        //angle = 0.0;
        while( angle < 2*M_PI+.1 ) {
                double c = 0.05 * cos(angle);
                double s = 0.05 * sin(angle);
                double c2 = 0.01 * cos(angle);
                double s2 = 0.01 * sin(angle);
                glNormal3f(cos(angle), 0, sin(angle));
                glTexCoord2f((angle / (2*M_PI+.1)), 1); glVertex3f(c2, 1.04, s2+1.19);
                glTexCoord2f((angle / (2*M_PI+.1)), 0); glVertex3f(c, .98, s+1.19);
                angle = angle + angle_stepsize;
        }

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        glPushMatrix();
        glTranslated(x,y,z);
        glRotated(th,0,1,0);
        glScaled(dx,dy,dz);

        glColor4f(1,1,1,0.01*100);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[6]);

        glBegin(GL_QUADS);

        glNormal3f(1, 0, 0);
        glVertex3f(1, .2, -.2);
        glVertex3f(1, .4, -.2);
        glVertex3f(1, .4, .2);
        glVertex3f(1, .2, .2);

        glNormal3f(-1, 0, 0);
        glVertex3f(-1, .2, -.2);
        glVertex3f(-1, .4, -.2);
        glVertex3f(-1, .4, .2);
        glVertex3f(-1, .2, .2);

        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
        const double len=2.0; //  Length of axes

        //  Erase the window and the depth buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //  Enable Z-buffering in OpenGL
        glEnable(GL_DEPTH_TEST);

        //  Undo previous transformations
        glLoadIdentity();
        //  Perspective - set eye position
        if (fpv) {
                ph = 0;
                th = 0;
                Lx = 100*Sin(l);
                Lz = 100*Cos(l);
                gluLookAt(Fx,Fy,Fz, Lx,Ly,Lz, 0,1,0); // Look from camera position to a certian direction.
                glRotatef(ph,1,0,0);
                glRotatef(th,0,1,0);
        }
        //  Orthogonal - set world orientation
        else
        {
                glRotatef(ph,1,0,0);
                glRotatef(th,0,1,0);
        }

        //  Flat or smooth shading
        glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

        //  Light switch
        if (day)
        {
                glDisable(GL_LIGHTING);
                ambient = 30;
                sco = 180;

                float Ambient[]   = {0.01*ambient,0.01*ambient,0.01*ambient,1.0};
                float Diffuse[]   = {0.01*diffuse,0.01*diffuse,0.01*diffuse,1.0};
                float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
                //float yellow[] = {1.0,1.0,0.0,1.0};
                //float Direction[] = {Cos(Th)*Sin(Ph),Sin(Th)*Sin(Ph),-Cos(Ph),0};
                //  Light position
                float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
                //  Draw light position as ball (still no lighting here)
                //  OpenGL should normalize normal vectors

                glEnable(GL_NORMALIZE);
                //  Enable lighting
                glEnable(GL_LIGHTING);
                glColor3f(1,1,1);
                ball(Position[0],Position[1],Position[2], .1);
                //  Location of viewer for specular calculations
                glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
                //  glColor sets ambient and diffuse color materials
                glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
                glEnable(GL_COLOR_MATERIAL);
                //  Enable light 0
                glEnable(GL_LIGHT0);
                //  Set ambient, diffuse, specular components and position of light 0
                glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
                glLightfv(GL_LIGHT0,GL_DIFFUSE,Diffuse);
                glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
                glLightfv(GL_LIGHT0,GL_POSITION,Position);

                glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,sco);

        }
        else {
                ambient = 0;
                sco = 15;

                float Ambient[]   = {0.01*ambient,0.01*ambient,0.01*ambient,1.0};
                float Diffuse[]   = {0.01*diffuse,0.01*diffuse,0.01*diffuse,1.0};
                float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
                float yellow[] = {1.0,1.0,0.0,1.0};
                float Direction[] = {Sin(l),Ly/100,Cos(l)};
                float Position[]  = {Fx,Fy,Fz,1.0};

                //  OpenGL should normalize normal vectors
                glEnable(GL_NORMALIZE);
                //  Enable lighting
                glEnable(GL_LIGHTING);
                //  Location of viewer for specular calculations
                glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
                //  Two sided mode
                glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,side);
                //  glColor sets ambient and diffuse color materials
                glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
                glEnable(GL_COLOR_MATERIAL);
                //  Set specular colors
                glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
                glMaterialf(GL_FRONT,GL_SHININESS,shiny);
                //Enable light 0
                glEnable(GL_LIGHT0);
                //  Set ambient, diffuse, specular components and position of light 0
                glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
                glLightfv(GL_LIGHT0,GL_DIFFUSE,Diffuse);
                glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
                glLightfv(GL_LIGHT0,GL_POSITION,Position);

                //  Set spotlight parameters
                glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,Direction);
                glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,sco);
                glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,Exp);
                //  Set attenuation
                glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,at0/100.0);
                glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,at1/100.0);
                glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,at2/100.0);

                sco = 25;

                float Direction2[] = {0,-1,.238};
                float Position2[]  = {0,.312,.238, 1.0};

                glEnable(GL_LIGHT1);

                //  Set ambient, diffuse, specular components and position of light 0
                glLightfv(GL_LIGHT1,GL_AMBIENT,Ambient);
                glLightfv(GL_LIGHT1,GL_DIFFUSE,Diffuse);
                glLightfv(GL_LIGHT1,GL_SPECULAR,Specular);
                glLightfv(GL_LIGHT1,GL_POSITION,Position2);

                //  Set spotlight parameters
                glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,Direction2);
                glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,sco);
                glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,Exp);
                //  Set attenuation
                glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,at0/100.0);
                glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,at1/100.0);
                glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,at2/100.0);


        }

        //  Draw scene
        ground(10);

        int randSize;
        for (double i=0; i<90; i+=5) {
                for(double j=0; j<90; j+=5) {
                        if (i != 45 || j != 45) {
                                if (i == 0 || j == 0)
                                        randSize = (int)((((i)/10)*2)+(((j)/10)*2));
                                else
                                        randSize = (int)(((i/10)*2)*((j/10)*2));
                                tree((i-45)/10, (j-45)/10, (.03 * Heights[randSize]), Heights[randSize]);

                        }
                }
        }

        tree(0, 1, .02, .5);

        ball(0,.312-.012,.238, .005);

        firewood(.2, .02, -.14, .02, .05, -90);
        firewood(.2, .02, -.1, .02, .05, -90);
        firewood(.2, .02, -.06, .02, .05, -90);
        firewood(.2, .02, -.02, .02, .05, -90);
        firewood(.2, .055, -.12, .02, .05, -90);
        firewood(.2, .055, -.08, .02, .05, -90);
        firewood(.22, 0, -.18, .02, .05, 0);

        cabin(0, 0, 0, .2, .3, .2, 0, door);

        target(targetPos[0], targetPos[1], .03);

        glWindowPos2i(0, 575);
        if(fabs(Fx-targetPos[0]) < .1 && fabs(Fz-targetPos[1]) < .1) {
                Print("You Win!");
        }
        else
                Print("Find the ghost");

        if (fpv) Sky(5);

        //  Draw axes - no lighting from here on
        glDisable(GL_LIGHTING);
        glColor3f(1,1,1);
        if (axes)
        {
                glBegin(GL_LINES);
                glVertex3d(0.0,0.0,0.0);
                glVertex3d(len,0.0,0.0);
                glVertex3d(0.0,0.0,0.0);
                glVertex3d(0.0,len,0.0);
                glVertex3d(0.0,0.0,0.0);
                glVertex3d(0.0,0.0,len);
                glEnd();
                //  Label axes
                glRasterPos3d(len,0.0,0.0);
                Print("X");
                glRasterPos3d(0.0,len,0.0);
                Print("Y");
                glRasterPos3d(0.0,0.0,len);
                Print("Z");
        }

        //  Display parameters
        glWindowPos2i(5,5);
        Print("Angle=%d,%d  Dim=%.1f FOV=%d Light=%s",
              th,ph,dim,fov,light ? "On" : "Off");
        if (light)
        {
                glWindowPos2i(5,45);
                Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth ? "Smooth" : "Flat",local ? "On" : "Off",distance,ylight);
                glWindowPos2i(5,25);
                Print("Ambient=%d",ambient);
        }

        //  Render the scene and make it visible
        ErrCheck("display");
        glFlush();
        glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
        //  Elapsed time in seconds
        double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
        zh = fmod(90*t,360.0);
        //  Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
        if (fpv) {
                //  Right arrow key - decrease angle by 45 degrees
                if (key == GLUT_KEY_RIGHT) {
                        l -= 5;
                }
                //  Left arrow key - increase angle by 45 degrees
                else if (key == GLUT_KEY_LEFT) {
                        l += 5;
                }
                //  Up arrow key - increase camera posoition and posotion of what camera looks at
                else if (key == GLUT_KEY_UP) {
                        Fx += .05*Sin(l);
                        Fz += .05*Cos(l);
                        Lx += .05*Sin(l);
                        Lz += .05*Cos(l);
                }
                //  Down arrow key - decrease camera position and position of what camera looks at
                else if (key == GLUT_KEY_DOWN) {
                        Fx -= .05*Sin(l);
                        Fz -= .05*Cos(l);
                        Lx -= .05*Sin(l);
                        Lz -= .05*Cos(l);
                }
        }
        if(!fpv) {
                //  Right arrow key - increase angle by 5 degrees
                if (key == GLUT_KEY_RIGHT)
                        th += 5;
                //  Left arrow key - decrease angle by 5 degrees
                else if (key == GLUT_KEY_LEFT)
                        th -= 5;
                //  Up arrow key - increase elevation by 5 degrees
                else if (key == GLUT_KEY_UP)
                        ph += 5;
                //  Down arrow key - decrease elevation by 5 degrees
                else if (key == GLUT_KEY_DOWN)
                        ph -= 5;
                //  PageUp key - increase dim
                else if (key == GLUT_KEY_PAGE_DOWN)
                        dim += 0.1;
                //  PageDown key - decrease dim
                else if (key == GLUT_KEY_PAGE_UP && dim>1)
                        dim -= 0.1;
                //  Smooth color model
                else if (key == GLUT_KEY_F1)
                        smooth = 1-smooth;
                //  Local Viewer
                else if (key == GLUT_KEY_F2)
                        local = 1-local;
                else if (key == GLUT_KEY_F3)
                        distance = (distance==1) ? 5 : 1;
                //  Toggle ball increment
                else if (key == GLUT_KEY_F8)
                        inc = (inc==10) ? 3 : 10;
        }
        //  Keep angles to +/-360 degrees
        l  %= 360;
        th %= 360;
        ph %= 360;

        if (Fx > 4.9)
                Fx = 4.9;
        else if (Fz > 4.9)
                Fz = 4.9;
        else if (Fx < -4.9)
                Fx = -4.9;
        else if (Fz < -4.9)
                Fz = -4.9;

        //  Update projection
        if (fpv)
                Project(1,fov,asp,dim);
        else
                Project(0,fov,asp,dim);
        //  Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
        //  Exit on ESC
        if (ch == 27)
                exit(0);
        //  Toggle axes
        else if (ch == 'x' || ch == 'X')
                axes = 1-axes;
        //  Toggle lighting
        else if (ch == 'l' || ch == 'L')
                light = 1-light;
        //  Toggle light movement
        else if (ch == 'm' || ch == 'M')
                move = 1-move;
        //  Change field of view angle
        else if (ch == '-' && ch>1)
                fov--;
        else if (ch == '+' && ch<179)
                fov++;
        else if (ch == 'f')
                fpv = 1 - fpv;
        //  Light elevation
        else if (ch=='[')
                ylight -= .1;
        else if (ch==']')
                ylight += .1;
        //  Ambient level
        else if (ch=='a' && ambient>0)
                ambient -= 5;
        else if (ch=='A' && ambient<100)
                ambient += 5;
        else if (ch == 'd')
                day = 1-day;
        else if (ch == 'w')
                Ly += 2;
        else if (ch == 's')
                Ly -= 2;
        else if (ch == 'c') {
                Fx = targetPos[0];
                Fz = targetPos[1];
        }
        else if (ch == 'o')
                door = 1 - door;


        //  Reproject
        if (fpv)
                Project(1,fov,asp,dim);
        else
                Project(0,fov,asp,dim);
        //  Animate if requested
        glutIdleFunc(move ? idle : NULL);
        //  Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
        //  Ratio of the width to the height of the window
        asp = (height>0) ? (double)width/height : 1;
        //  Set the viewport to the entire window
        glViewport(0,0, width,height);
        //  Set projection
        if (fpv)
                Project(1,fov,asp,dim);
        else
                Project(0,fov,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
        srand(time(NULL));

        targetPos[0] = ((rand() % 5) - (rand() % 5)) + .1;
        targetPos[1] = ((rand() % 5) - (rand() % 5)) + .1;

        for (int k = 0; k < 324; k++) {
                Heights[k] = (rand() % 2)+0.5;
        }

        //  Initialize GLUT
        glutInit(&argc,argv);
        //  Request double buffered, true color window with Z buffering at 600x600
        glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
        glutInitWindowSize(600,600);
        glutCreateWindow("Tyler Lugger");
        //  Set callbacks
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutSpecialFunc(special);
        glutKeyboardFunc(key);
        glutIdleFunc(idle);
        //  Load textures
        texture[0] = LoadTexBMP("ground.bmp");
        texture[1] = LoadTexBMP("bark.bmp");
        texture[2] = LoadTexBMP("leaves.bmp");
        texture[3] = LoadTexBMP("cabin.bmp");
        texture[4] = LoadTexBMP("ghost.bmp");
        texture[5] = LoadTexBMP("door.bmp");
        texture[6] = LoadTexBMP("roof.bmp");
        texture[7] = LoadTexBMP("glass.bmp");
        texture[8] = LoadTexBMP("metal.bmp");
        texture[9] = LoadTexBMP("rings.bmp");
        texture[10] = LoadTexBMP("brick.bmp");
        texture[11] = LoadTexBMP("fireplace.bmp");
        // Load SkyCube textures
        sky[0] = LoadTexBMP("negz.bmp");
        sky[1] = LoadTexBMP("negx.bmp");
        sky[2] = LoadTexBMP("posz.bmp");
        sky[3] = LoadTexBMP("posx.bmp");
        sky[4] = LoadTexBMP("posy.bmp");
        sky[5] = LoadTexBMP("negy.bmp");
        //  Pass control to GLUT so it can interact with the user
        ErrCheck("init");
        glutMainLoop();
        return 0;
}
