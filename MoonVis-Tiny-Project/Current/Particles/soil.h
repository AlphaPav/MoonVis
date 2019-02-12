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

#ifndef _DUMPSOIL_H
#define _DUMPSOIL_H

#include <gl\glut.h>
#include <math.h>
#include <iostream>
#include <list>
#include <vector>

#include <nvMath.h>
#include <glx.h>

using namespace std;
#define REP(i,n) for(i=0;i<(n);++i)
#define FORIT(a,aa) for(a=aa.begin();a!=aa.end();++a)

/*
3 2
0 1

7 6
4 5
*/

const float gravity = 9.8;
class Soil{
	nv::vec3f center;
	nv::vec3f points[8];
	float threshold_dis;
	float distance;
	float velocity;
	float dt;
	float radius;
public:

	Soil(){}
	Soil(nv::vec3f _center, float _radius, float _threshold_dis = 1): 
	  center(_center), radius(_radius), threshold_dis(_threshold_dis), distance(0), velocity(0)
	  {
		  init();
	  }
	void init();
	bool vanish();
	void update(float DT);
	void draw();
};


class DumpSoil
{
protected:
	float radius;
	float threshold_dis;
	list<Soil> soils;
	float soil_radius;
	float dt;
	bool end;
public:
	DumpSoil(float _threshold_dis, float _radius, float _soil_radius = 0.02f): 
	  threshold_dis(_threshold_dis), radius(_radius), soil_radius(_soil_radius)
		  , end(false)
	{
		setSoilRadius(soil_radius);
	}
	void setSoilRadius(float r);
	void produceNew(float DT);
	void reset();
	void update(float DT);
	void draw();
	void setEnd() { end = true; }
	bool isEmpty() const { return soils.empty(); }
};


class DigSoil
{
	float minRadius; //土堆的最小半径
	float maxRadius; //土堆的最大半径
	float hwRatio;  //土堆的高度宽度比
	float soilRadius;
	float radius; //土堆的当前半径
	float lastRadius; //上次土堆的半径, 用于控制形状的更新频率
	float deltaRadius; //土堆的半径增长速率
	vector<Soil> soils;

public:
	DigSoil(float _radius, float width, float _hwRatio, float _soilRadius, float _deltaRadius)
		: minRadius(_radius)
		, maxRadius(_radius + width)
		, hwRatio(_hwRatio)
		, soilRadius(_soilRadius)
		, radius(minRadius + soilRadius)
		, lastRadius(radius)
		, deltaRadius(_deltaRadius)
	{
	}
	DigSoil(const DigSoil& rhd)
		: minRadius(rhd.minRadius)
		, maxRadius(rhd.maxRadius)
		, hwRatio(rhd.hwRatio)
		, soilRadius(rhd.soilRadius)
		, radius(rhd.radius)
		, lastRadius(rhd.radius)
		, deltaRadius(rhd.deltaRadius)
	{
	}

	void update(float DT);
	void draw();
	void reset();
};


#endif