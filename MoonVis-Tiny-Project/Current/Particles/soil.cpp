/*************************************************************************\

Copyright 2018 Zhejiang University.
All Rights Reserved.

Permission to use, copy, modify and distribute this software and its
documentation for educational, research and non-profit purposes, without
fee, and without a written agreement is hereby granted, provided that the
above copyright notice and the following three paragraphs appear in all
copies.

IN NO EVENT SHALL ZHEJIANG UNIVERSITY BE
LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF ZHEJIANG
UNIVERSITY HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

ZHEJIANG UNIVERSITY SPECIFICALLY DISCLAIM ANY
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND ZHEJIANG UNIVERSITY
HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

The authors may be contacted via:

Mail:             College of Computer Science and Technology
Zhejiang Univeristy
Hangzhou, Zhejiang, China 310027

EMail:            tang_m@zju.edu.cn

\**************************************************************************/

#include "stdafx.h"
#include "soil.h"


void Soil::init()
{
	int i;
	float rad = radius;
	radius *= (rand()%100)/30.0;
	points[0] = nv::vec3f(center.x-radius, center.y+radius, center.z+radius);
	points[1] = nv::vec3f(center.x+radius, center.y+radius, center.z+radius);
	points[2] = nv::vec3f(center.x+radius, center.y+radius, center.z-radius);
	points[3] = nv::vec3f(center.x-radius, center.y+radius, center.z-radius);
	points[4] = nv::vec3f(center.x-radius, center.y-radius, center.z+radius);
	points[5] = nv::vec3f(center.x+radius, center.y-radius, center.z+radius);
	points[6] = nv::vec3f(center.x+radius, center.y-radius, center.z-radius);
	points[7] = nv::vec3f(center.x-radius, center.y-radius, center.z-radius);
	REP(i,8)
	{
		points[i].x += (rand()%100-60)/100.0*radius;
		points[i].y += (rand()%100-60)/100.0*radius;
		points[i].z += (rand()%100-60)/100.0*radius;
	}
	radius = rad;
}
bool Soil::vanish()
{
	return distance > threshold_dis;
}
void Soil::update(float DT)
{
	int i;
	velocity += gravity * DT * (1.0 + (rand() % 100 - 50) / 500.0);
	float d = (velocity - gravity * DT / 2) * DT;
	d *= (2.0 + rand()%4) / 5;
	REP(i,8) points[i].y -= d;
	distance += d;
}
void Soil::draw()
{
	int i,vt[6][4] = {{0,1,2,3},{4,7,6,5},{1,5,6,2},{0,3,7,4},{4,5,1,0},{3,2,6,7}};

	bool mode = glxEnableLighting(true);
	//COLORREF color_t = glxSetColor(RGB(90, 90, 90));
	glxSetMaterial(1.f, 1.f, 1.f, 1.f);

	REP(i,6)
	{
		glBegin(GL_TRIANGLE_FAN);
		nv::vec3f normalv = cross(points[vt[i][1]] - points[vt[i][0]], points[vt[i][2]] - points[vt[i][0]]);
		normalv = nv::normalize(normalv);
		glNormal3fv(normalv.get_value());
		glVertex3f(points[vt[i][0]].x, points[vt[i][0]].y, points[vt[i][0]].z);
		glVertex3f(points[vt[i][1]].x, points[vt[i][1]].y, points[vt[i][1]].z);
		glVertex3f(points[vt[i][2]].x, points[vt[i][2]].y, points[vt[i][2]].z);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		normalv = cross(points[vt[i][2]] - points[vt[i][0]], points[vt[i][3]] - points[vt[i][0]]);
		normalv = normalize(normalv);
		glNormal3fv(normalv.get_value());
		glVertex3f(points[vt[i][2]].x, points[vt[i][2]].y, points[vt[i][2]].z);
		glVertex3f(points[vt[i][3]].x, points[vt[i][3]].y, points[vt[i][3]].z);
		glVertex3f(points[vt[i][0]].x, points[vt[i][0]].y, points[vt[i][0]].z);
		glEnd();
	}

	glxEnableLighting(mode);
	//glxSetColor(color_t);
}



void DumpSoil::setSoilRadius(float r)
{
	soil_radius = r;
	dt = sqrt(4 * soil_radius / gravity);
}
void DumpSoil::produceNew(float DT)
{
	int n = DT / dt;
	if (n < 1) n = 1;
	while ( n--)
	{
		int i,j,k1,k2;
		int wn = radius / soil_radius;
		REP(k1, 2) REP(k2, 2) REP(i, wn) REP(j, wn)
		{
			float x = i * (k1==0?-1:1) * soil_radius * 2;
			float z = j * (k2==0?-1:1) * soil_radius * 2;
			x *= 1 + (rand()%100-50)/200.0;
			z *= 1 + (rand()%100-50)/200.0;
			if (x * x + z * z > radius * radius) continue;
			nv::vec3f cnt(x, 0.5 * gravity * (n * dt) * (n * dt), z);
			Soil soil(cnt, soil_radius, threshold_dis);
			soils.push_back(soil);
		}
	}
}

void DumpSoil::reset()
{
	soils.clear();
	end = false;
}

void DumpSoil::update(float DT)
{
	list<Soil>::iterator it;
	for (it = soils.begin(); it != soils.end(); )
	{
		it->update(DT);
		if (it->vanish()) soils.erase(it++);
		else it++;
	}

	if (!end)
	{
		produceNew(DT);
	}
}
void DumpSoil::draw()
{
	list<Soil>::iterator it;
//	cout<<soils.size()<<endl;
	for (it = soils.begin(); it != soils.end(); it++) it->draw();
}

////////////////////////////////////////DIGSOIL////////////////////////////
inline float getProbability(float p) // [1-p,1+p]
{
	//	return 1;
	return 1 + (rand()%101/100.0 * 2 * p) - p;
}

void DigSoil::update(float DT)
{
	radius += deltaRadius * DT;
	if (radius > maxRadius) radius = maxRadius;

	if (radius - lastRadius < 0.0001)
	{
		return;
	}
	lastRadius = radius;

	soils.clear();
	float r = radius;
	float h = 0;
	float dr = (maxRadius - minRadius) / hwRatio;
	if (dr > soilRadius) dr = soilRadius;
	while (r > minRadius)
	{

		float theta = soilRadius / r;
		float circular = 0;
		while (circular < 2 * 3.1415)
		{
			Soil soil(nv::vec3f(r * cos(circular) * getProbability(0.1), h, r * sin(circular) * getProbability(0.1)), soilRadius);
			soils.push_back(soil);
			circular += theta;
		}
		h += soilRadius * 2;
		r -= dr;
	}
}

void DigSoil::draw()
{
	for (vector<Soil>::iterator itr = soils.begin(); itr != soils.end(); itr++)
	{
		itr->draw();
	}
}

void DigSoil::reset()
{
	radius = minRadius + soilRadius;
	soils.clear();
}
