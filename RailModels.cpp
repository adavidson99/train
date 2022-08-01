//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  RailModels.cpp
//  A collection of functions for generating the models for a railway scene
//  ========================================================================

#define _USE_MATH_DEFINES 
#include <cmath>
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <climits>
#include "RailModels.h"
#include "loadBMP.h"
#include "loadTGA.h"

const int NPTS = 20;
GLuint txId[3];

const int N = 10;  // initial vertex definitions for the station

float vx_init[N] = { 160, 165, 185, 185, 195, 195, 160, 160, 200, 200 };
float vy_init[N] = { 0, 5, 5, 10, 10, 25, 25, 30, 30, 0 };
float nx_init[N] = { -0.707, 0, -1, 0, -1, 0, -1, 0, 1, 0 };
float ny_init[N] = { 0.707, 1, 0, 1, 0, -1, 0, 1, 0, -1 };

const int M = 33;  // initial vertex definitions for the tunnel

float tx_init[M] = { 120, 125, 129, 133, 137, 140, 143, 146, 149, 150, 151, 154, 157, 
					160, 163, 167, 171, 175, 180, 165, 165, 163, 160, 157, 154, 151,
					149, 146, 143, 140, 137, 135, 135};
float ty_init[M] = { 0, 3, 6, 9, 13, 16.1, 20.1, 23.1, 25.1, 25.1, 25.1, 23.1, 20.1, 16.1, 13, 9, 6, 3,
					0, 0, 7, 12, 16, 20, 23, 25, 25, 23, 20, 16, 12, 7, 0};
float* dx, * dy;

using namespace std;

float *x, *z;
float ux[492];
float uz[492];
float vx[492];
float vz[492];
float len[492];
int nitems;
int trackcounter = 0;


void loadTexture()
{
	glGenTextures(2, txId); 	// Create a Texture object

	glBindTexture(GL_TEXTURE_2D, txId[0]);		//Use this texture
	loadBMP("Road.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[1]);		//Use this texture
	loadBMP("WagonTexture.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[2]);  //Use this texture
	loadTGA("Wall.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPEAT);
}


void calculate_normals()
{
	float l1[M];
	float l2[M];
	dx = new float[M];
	dy = new float[M];
	float k;

	for (int i = 0; i < M; i++)
	{
		l1[i] = tx_init[i];
		l2[i] = ty_init[i];
	}

	for (int i = 0; i < (M-15); i++)
	{
		k = sqrt(pow(l1[i + 1] - l1[i], 2) + pow(l2[i + 1] - l2[i], 2));
		dy[i] = ((l1[i + 1] - l1[i]) / k);
		dx[i] = -1 * ((l2[i + 1] - l2[i]) / k);
	}

	for (int i = 17; i < (M); i++)
	{
		dy[i] = 0;
		dx[i] = 0;
	}

//	k = sqrt(pow(l1[0] - l1[32], 2) + pow(l2[0] - l2[32], 2));
//	dy[32] = ((l1[0] - l1[32]) / k);
//	dx[32] = -1 * ((l2[0] - l2[32]) / k);

}


void trackFile()
{
	ifstream fp_in;

	fp_in.open("Oval.txt", ios::in);
	if (!fp_in.is_open())
	{
		cout << "Error opening text file" << endl;
		exit(1);
	}

	fp_in >> nitems;		    // read number of vertices, polygons, edges (not used)

	x = new float[nitems];                        //create arrays
	z = new float[nitems];

	for (int i = 0; i < nitems; i++)                         //read vertex list 
		fp_in >> x[i] >> z[i];

	fp_in.close();
	cout << " File successfully read." << endl;
}


//------------- GROUND PLANE ------------------------------------
// This is a square shaped region on the xz-plane of size 400x400 units
// centered at the origin.  This region is constructed using small quads
// of unit size, to facilitate rendering of spotlights
//-----------------------------------------------------------------
void floor()
{
	glDisable(GL_TEXTURE_2D);

	float white[4] = { 1., 1., 1., 1. };
	float black[4] = { 0 };
	glColor4f(0.3, 0.8, 0.3, 1.0);  //The floor is gray in colour
	glNormal3f(0.0, 1.0, 0.0);

	glMaterialfv(GL_FRONT, GL_SPECULAR, black);

	//The floor is made up of several tiny squares on a 400 x 400 grid. Each square has a unit size.
	glBegin(GL_QUADS);
	for (int i = -200; i < 200; i++)
	{
		for (int j = -200; j < 200; j++)
		{
			glVertex3f(i, 0, j);
			glVertex3f(i, 0, j + 1);
			glVertex3f(i + 1, 0, j + 1);
			glVertex3f(i + 1, 0, j);
		}
	}
	glEnd();

	glMaterialfv(GL_FRONT, GL_SPECULAR, white);

}

//--------------- RAILWAY TRACK ------------------------------------
// A circular railway track of specified median radius and width
// The tracks are modelled using small quadrilateral segments each covering 5 deg arc
// The tracks have a height of 1 unit (refer to lab notes)
//-----------------------------------------------------------------
void tracks(float medRadius, float width)
{
	float inRad = medRadius - width * 0.5;
	float outRad = medRadius + width * 0.5;
	float angle1, angle2, ca1, sa1, ca2, sa2;
	float x1, z1, x2, z2, x3, z3, x4, z4;  //four points of a quad
	float toRad = 3.14159265 / 180.0;  //Conversion from degrees to radians

	glColor4f(0.0, 0.0, 0.3, 1.0);
	glBegin(GL_QUADS);
	float radius = inRad;
	for (int i = 0; i < 2; i++)   //Two parallel tracks (radius = inRad, outRad)
	{
		for (int i = 0; i < 360; i += 5)    //5 deg intervals
		{
			angle1 = i * toRad;       //Computation of angles, cos, sin etc
			angle2 = (i + 5) * toRad;
			ca1 = cos(angle1); ca2 = cos(angle2);
			sa1 = sin(angle1); sa2 = sin(angle2);
			x1 = (radius - 0.5) * sa1; z1 = (radius - 0.5) * ca1;
			x2 = (radius + 0.5) * sa1; z2 = (radius + 0.5) * ca1;
			x3 = (radius + 0.5) * sa2; z3 = (radius + 0.5) * ca2;
			x4 = (radius - 0.5) * sa2; z4 = (radius - 0.5) * ca2;

			glNormal3f(0., 1., 0.);       //Quad 1 facing up
			glVertex3f(x1, 1.0, z1);
			glVertex3f(x2, 1.0, z2);
			glVertex3f(x3, 1.0, z3);
			glVertex3f(x4, 1.0, z4);

			glNormal3f(-sa1, 0.0, -ca1);   //Quad 2 facing inward
			glVertex3f(x1, 0.0, z1);
			glVertex3f(x1, 1.0, z1);
			glNormal3f(-sa2, 0.0, -ca2);
			glVertex3f(x4, 1.0, z4);
			glVertex3f(x4, 0.0, z4);

			glNormal3f(sa1, 0.0, ca1);   //Quad 3 facing outward
			glVertex3f(x2, 1.0, z2);
			glVertex3f(x2, 0.0, z2);
			glNormal3f(sa2, 0.0, ca2);
			glVertex3f(x3, 0.0, z3);
			glVertex3f(x3, 1.0, z3);

		}
		radius = outRad;
	}
	glEnd();
}


void railway_sleeper()
{
	for (int i = 0; i < 360; i += 2)
	{
		glPushMatrix();

		glRotatef(i, 0, 1, 0);
		glTranslatef(142.5, 0, 0);

		glColor4f(0.5, 0, 0, 1.0);
		glBegin(GL_QUADS);

		glNormal3f(0, 1, 0);
		glVertex3f(0, 0.01, 0);
		glVertex3f(15, 0.01, 0);
		glVertex3f(15, 0.01, 2);
		glVertex3f(0, 0.01, 2);

		glEnd();

		glPopMatrix();
	}

}


//--------------- MODEL BASE --------------------------------------
// This is a common base for the locomotive and wagons
// The base is of rectangular shape (20 length x 10 width x 2 height)
// and has wheels and connectors attached.
//-----------------------------------------------------------------
void basewagon()
{
	glDisable(GL_TEXTURE_2D);

	glColor4f(0.3, 0.3, 0.3, 1.0);

	//Base plate
	glPushMatrix();
	glTranslatef(0.0, 2.5, 0.0);
	glScalef(36.0, 1.0, 6.0);
	glutSolidCube(1.0);
	glPopMatrix();

	float wx[4] = { -12,   12,   -12,    12 };
	float wz[4] = { 5.1, 5.1, -5.1, -5.1 };
	glColor4f(0.5, 0., 0., 1.0);    //Wheel color
	GLUquadric* q = gluNewQuadric();   //Disc

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glTranslatef(wx[i], 2.0, wz[i]);
		gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
	}

}


void baseengine()
{
	glColor4f(0.2, 0.2, 0.2, 1.0);   //Base color
	glPushMatrix();
	glTranslatef(0.0, 4.0, 0.0);
	glScalef(20.0, 2.0, 10.0);     //Size 20x10 units, thickness 2 units.
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();					//Connector between wagons
	glTranslatef(11.0, 4.0, 0.0);
	glutSolidCube(2.0);
	glPopMatrix();

	//4 Wheels (radius = 2 units)
	//x, z positions of wheels:
	float wx[4] = { -8,   8,   -8,    8 };
	float wz[4] = { 5.1, 5.1, -5.1, -5.1 };
	glColor4f(0.5, 0., 0., 1.0);    //Wheel color
	GLUquadric* q = gluNewQuadric();   //Disc

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glTranslatef(wx[i], 2.0, wz[i]);
		gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
	}
}


//--------------- LOCOMOTIVE --------------------------------
// This simple model of a locomotive consists of the base,
// cabin and boiler
//-----------------------------------------------------------
void engine()
{
	baseengine();

	//Cab
	glColor4f(0.8, 0.8, 0.0, 1.0);
	glPushMatrix();
	glTranslatef(7.0, 8.5, 0.0);
	glScalef(6.0, 7.0, 10.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(6.0, 14.0, 0.0);
	glScalef(4.0, 4.0, 8.0);
	glutSolidCube(1.0);
	glPopMatrix();

	//Boiler
	GLUquadric* q = gluNewQuadric();   //Cylinder
	glPushMatrix();
	glColor4f(0.5, 0., 0., 1.0);
	glTranslatef(4.0, 10.0, 0.0);
	glRotatef(-90.0, 0., 1., 0.);
	gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
	glTranslatef(0.0, 0.0, 14.0);
	gluDisk(q, 0.0, 5.0, 20, 3);
	glColor4f(1.0, 1.0, 0.0, 1.0);
	glTranslatef(0.0, 4.0, 0.2);
	gluDisk(q, 0.0, 1.0, 20, 2);  //headlight!
	glPopMatrix();

}

//--------------- WAGON ----------------------------------
// This simple model of a rail wagon consists of the base,
// and a cube(!)
//--------------------------------------------------------
void wagon()
{
	basewagon();

	glColor4f(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[1]);

	//3 large polygons (front, back, top)
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);   //Facing +z (Front side)
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-17.0, 3.0, 6.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(17.0, 3.0, 6.0);
	glTexCoord2f(1.0, 0.34);
	glVertex3f(17.0, 15.0, 6.0);
	glTexCoord2f(0.0, 0.34);
	glVertex3f(-17.0, 15.0, 6.0);

	glNormal3f(0.0, 0.0, -1.0);   //Facing -z (Back side)
	glTexCoord2f(0.0, 0.0);
	glVertex3f(17.0, 3.0, -6.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-17.0, 3.0, -6.0);
	glTexCoord2f(1.0, 0.34);
	glVertex3f(-17.0, 15.0, -6.0);
	glTexCoord2f(0.0, 0.34);
	glVertex3f(17.0, 15.0, -6.0);

	glNormal3f(0.0, 1.0, 0.0);   //Facing +y (Top side)
	glTexCoord2f(0.0, 0.45);
	glVertex3f(-17.0, 15.0, 6.0);
	glTexCoord2f(1.0, 0.45);
	glVertex3f(17.0, 15.0, 6.0);
	glTexCoord2f(1.0, 0.83);
	glVertex3f(17.0, 15.0, -6.0);
	glTexCoord2f(0.0, 0.83);
	glVertex3f(-17.0, 15.0, -6.0);
	glEnd();

	//2 small side polygons (left, right)

	glDisable(GL_TEXTURE_2D);

	glColor4f(0.5, 0.5, 0.0, 1.0);

	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0.0, 0.0);   //Facing -x (Left side)
	glVertex3f(-17.0, 3.0, -6.0);
	glVertex3f(-17.0, 3.0, 6.0);
	glVertex3f(-17.0, 15.0, 6.0);
	glVertex3f(-17.0, 15.0, -6.0);

	glNormal3f(1.0, 0.0, 0.0);   //Facing +x (Right side)
	glVertex3f(17.0, 3.0, 6.0);
	glVertex3f(17.0, 3.0, -6.0);
	glVertex3f(17.0, 15.0, -6.0);
	glVertex3f(17.0, 15.0, 6.0);
	glEnd();
}


void station()
{

//	loadTexture();
//	glBindTexture(GL_TEXTURE_2D, txId[2]);
//	glEnable(GL_TEXTURE_2D);

	float vx[N], vy[N], vz[N];   //vertex positions
	float wx[N], wy[N], wz[N];
	float nx[N], ny[N], nz[N];   //normal vectors
	float mx[N], my[N], mz[N];

	int nSlices = 36;

	float toRadians = M_PI / 180.0;   //Conversion from degrees to radians
	float angStep = 1.5 * toRadians;

	for (int i = 0; i < N; i++)		//Initialize data everytime the frame is refreshed
	{
		vx[i] = vx_init[i];
		vy[i] = vy_init[i];
		vz[i] = 0;
		nx[i] = nx_init[i];
		ny[i] = ny_init[i];
		nz[i] = 0;
	}

	glColor3f(0.8, 0.8, 0.5);


	for (int j = 0; j < nSlices; j++)
	{
		for (int i = 0; i < N; i++)
		{
			wx[i] = cos(angStep) * vx[i] + sin(angStep) * vz[i];
			wy[i] = vy[i];
			wz[i] = -sin(angStep) * vx[i] + cos(angStep) * vz[i];
			mx[i] = cos(angStep) * nx[i] + sin(angStep) * nz[i];
			my[i] = ny[i];
			mz[i] = -sin(angStep) * nx[i] + cos(angStep) * nz[i];
		}

		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i < N; i++)
		{
			glNormal3f(nx[i], ny[i], nz[i]);
			glTexCoord2f((float)j / nSlices, (float)i / (N - 1));
			glVertex3f(vx[i], vy[i], vz[i]);
			glNormal3f(mx[i], my[i], mz[i]);
			glTexCoord2f((float)(j + 1) / nSlices, (float)i / (N - 1));
			glVertex3f(wx[i], wy[i], wz[i]);
		}
		glEnd();

		for (int i = 0; i < N; i++)    //Update vertices
		{
			vx[i] = wx[i];
			vy[i] = wy[i];
			vz[i] = wz[i];
			nx[i] = mx[i];
			ny[i] = my[i];
			nz[i] = mz[i];
		}
	}

	glColor3f(0.8, 0.8, 0.5);

	glPushMatrix();
	glTranslatef(160, 0, 0);
	station_wall();
	glPopMatrix();

	glPushMatrix();
	glRotatef(54, 0, 1, 0);
	glTranslatef(160, 0, 0);
	station_wall();
	glPopMatrix();

}

void station_wall()
{
	glColor3f(0.8, 0.8, 0.5);
	glBegin(GL_QUADS);

	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 30, 0);
	glVertex3f(40, 30, 0);
	glVertex3f(40, 0, 0);

	glNormal3f(0, 0, -1);
	glVertex3f(0, 0, -5);
	glVertex3f(0, 30, -5);
	glVertex3f(40, 30, -5);
	glVertex3f(40, 0, -5);

	glNormal3f(-1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, -5);
	glVertex3f(0, 30, -5);
	glVertex3f(0, 30, 0);

	glNormal3f(0, 1, 0);
	glVertex3f(0, 30, 0);
	glVertex3f(0, 30, -5);
	glVertex3f(40, 30, -5);
	glVertex3f(40, 30, 0);

	glNormal3f(1, 0, 0);
	glVertex3f(40, 30, 0);
	glVertex3f(40, 30, -5);
	glVertex3f(40, 0, -5);
	glVertex3f(40, 0, 0);

	glEnd();

}


void tunnel()
{

	glDisable(GL_TEXTURE_2D);

	float vx[M], vy[M], vz[M];   //vertex positions
	float wx[M], wy[M], wz[M];
	float nx[M], ny[M], nz[M];   //normal vectors
	float mx[M], my[M], mz[M];

	calculate_normals();

	int nSlices = 36;

	float toRadians = M_PI / 180.0;   //Conversion from degrees to radians
	float angStep = 2 * toRadians;

	for (int i = 0; i < M; i++)		//Initialize data everytime the frame is refreshed
	{
		vx[i] = tx_init[i];
		vy[i] = ty_init[i];
		vz[i] = 0;
		nx[i] = dx[i];
		ny[i] = dy[i];
		nz[i] = 0;
	}

	glColor3f(0.0, 0.7, 0.7);


	for (int j = 0; j < nSlices; j++)
	{
		for (int i = 0; i < M; i++)
		{
			wx[i] = cos(angStep) * vx[i] + sin(angStep) * vz[i];
			wy[i] = vy[i];
			wz[i] = -sin(angStep) * vx[i] + cos(angStep) * vz[i];
			mx[i] = cos(angStep) * nx[i] + sin(angStep) * nz[i];
			my[i] = ny[i];
			mz[i] = -sin(angStep) * nx[i] + cos(angStep) * nz[i];
		}

		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i < M; i++)
		{
			glNormal3f(nx[i], ny[i], nz[i]);
			glTexCoord2f((float)j / nSlices, (float)i / (M - 1));
			glVertex3f(vx[i], vy[i], vz[i]);
			glNormal3f(mx[i], my[i], mz[i]);
			glTexCoord2f((float)(j + 1) / nSlices, (float)i / (M - 1));
			glVertex3f(wx[i], wy[i], wz[i]);
		}
		glEnd();

		for (int i = 0; i < M; i++)    //Update vertices
		{
			vx[i] = wx[i];
			vy[i] = wy[i];
			vz[i] = wz[i];
			nx[i] = mx[i];
			ny[i] = my[i];
			nz[i] = mz[i];
		}
	}


	glPushMatrix();
	glTranslatef(120, 0, 0);
	tunnel_seal1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(180, 0, 0);
	glRotatef(-180, 0, 0, 1);
	glRotatef(180, 1, 0, 0);
	tunnel_seal2();
	glPopMatrix();

	glPushMatrix();
	glRotatef(72, 0, 1, 0);
	glTranslatef(120, 0, 0);
	tunnel_seal1();
	glPopMatrix();

	glPushMatrix();
	glRotatef(72, 0, 1, 0);
	glTranslatef(180, 0, 0);
	glRotatef(-180, 0, 0, 1);
	glRotatef(180, 1, 0, 0);
	tunnel_seal2();
	glPopMatrix();
}


void tunnel_seal1()
{
	glColor3f(0.0, 0.7, 0.7);

	glBegin(GL_TRIANGLES);

	glNormal3f(-0.707, 0, -0.707);
	glVertex3f(0, 0, 0);
	glVertex3f(17, 13, 0);
	glVertex3f(17, 0, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(17, 0, 1);
	glVertex3f(17, 13, 0);

	glVertex3f(17, 0, 0);
	glVertex3f(17, 0, 1);
	glVertex3f(17, 13, 0);

	glVertex3f(17, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(17, 0, 1);

	glEnd();
}


void tunnel_seal2()
{
	glColor3f(0.0, 0.7, 0.7);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.707, 0, 0.707);
	glVertex3f(0, 0, 0);
	glVertex3f(17, 13, 0);
	glVertex3f(17, 0, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(17, 0, 1);
	glVertex3f(17, 13, 0);

	glVertex3f(17, 0, 0);
	glVertex3f(17, 0, 1);
	glVertex3f(17, 13, 0);

	glVertex3f(17, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(17, 0, 1);

	glEnd();
}


void road()
{
	loadTexture();

	glBindTexture(GL_TEXTURE_2D, txId[0]);

	float a[] = { -60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130 };
	float b[] = { -40, -40, -39, -37, -32.5, -25, -17, -12.5, -11, -10, -9, -7, -3, 1, 5, 9, 13, 17, 21, 25 };

	float s = 0;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUAD_STRIP);    //Draw a polygonal line using GL_LINE_STRIP
	for (int i = 0; i < NPTS; i++)
	{
		glNormal3f(0, 1, 0);
		glTexCoord2f(s, 0.0);
		glVertex3f(a[i], 0.02, b[i]);
		glNormal3f(0, 1, 0);
		glTexCoord2f(s, 1.0);
		glVertex3f(a[i], 0.02, b[i] + 30);

		s += 0.5;
	}
	glEnd();

	glFlush();

}

void barrier_arm()
{

	GLUquadric* q = gluNewQuadric();   //Cylinder

	glColor4f(1, 0, 0, 1);

	glPushMatrix();
	glTranslatef(-90, 20, 95);
	glRotatef(90, 1, 0, 0);
	gluCylinder(q, 1, 1, 20, 20, 5);

	glPopMatrix();


	glPushMatrix();

	glTranslatef(-60, 20, 160);
	glRotatef(90, 1, 0, 0);
	gluCylinder(q, 1, 1, 20, 20, 5);

	glPopMatrix();

}


void arm()
{

	glPushMatrix();

	glColor4f(1, 1., 1., 1.0);
	glTranslatef(15, 0, 0);
	glScalef(50, 1, 1);
	glutSolidCube(1);
	glPushMatrix();

	glPopMatrix();

}
