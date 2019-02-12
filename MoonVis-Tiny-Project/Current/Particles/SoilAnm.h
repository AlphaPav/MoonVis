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

#pragma once

#include <gl/GLU.h>
#include "soil.h"



class DumpSoilAnm : public DumpSoil
{
public:
	DumpSoilAnm(float _threshold_dis, float _radius, float _soil_radius = 0.02f)
		: DumpSoil(_threshold_dis, _radius, _soil_radius)
	{
	}

	DumpSoilAnm(const DumpSoilAnm& rhd)
		: DumpSoil(rhd.threshold_dis, rhd.radius, rhd.soil_radius)
	{
	}

	void Reset()
	{
		DumpSoil::reset();
	}

	void Step(float dt)
	{
		update(dt);
	}

	void Draw(nv::matrix4f mat, nv::vec3f pos, nv::vec3f dir)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(mat.get_value());
		glTranslatef(pos.x, pos.y, pos.z);
		nv::quaternionf quat(nv::vec3f(0.f, -1.f, 0.f), dir);
		nv::applyRotation(quat);
		float scale = 100.f;
		glScalef(scale, scale, scale);

		DumpSoil::draw();

		glPopMatrix();
	}
};

class DigSoilAnm : public DigSoil
{
public:
	DigSoilAnm(float _radius, float width, float _hwRatio, float _soilRadius, float _deltaRadius)
		: DigSoil(_radius, width, _hwRatio, _soilRadius, _deltaRadius)
	{
	}

	DigSoilAnm(const DigSoilAnm& rhd)
		: DigSoil(rhd)
	{
	}

	void Reset()
	{
		DigSoil::reset();
	}

	void Step(float dt)
	{
		update(dt);
	}

	void Draw(nv::matrix4f mat, nv::vec3f pos, nv::vec3f dir)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		nv::quaternionf quat(nv::vec3f(0.f, 1.f, 0.f), dir);
		nv::applyRotation(quat);
		float scale = 100.f;
		glScalef(scale, scale, scale);

		DigSoil::draw();

		glPopMatrix();
	}
};