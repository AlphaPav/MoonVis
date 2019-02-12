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
#include "TailFlame.h"
#include <math.h>
#include <iostream>
using namespace std;

#define VZ_DIV 800

GLuint TailFlame::_tex = 0;

void TailFlame::InitData(const char tex_name[])
{
	if (glIsTexture(_tex))
	{
		glDeleteTextures(1, &_tex);
	}
	glGenTextures(1, &_tex);
	nv::Image *img = new nv::Image();
	if (!img->loadImageFromFile(tex_name))
	{
		cout<<"can not load tex file!"<<endl;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, _tex); 
	// 指定当前纹理的放大/缩小过滤方式
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, img->getInternalFormat(), img->getWidth(), img->getHeight(), 0, img->getFormat(), img->getType(), img->getLevel(0));
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		do 
		{
			cout<<err<<", ";
		}
		while ((err = glGetError()) != GL_NO_ERROR);
		cout<<endl;
	}
}

void TailFlame::Reset()
{
	for (int i = 0; i < _num_smoke; i++)
	{
		_particles[i].active = false;
		_particles[i].draw = false;
	}
}

void TailFlame::Step(const nv::vec3f& position, float velocity, float dt)
{
	for(int i = 0; i < _num_smoke; i++)
	{
		if(_particles[i].active==false)
		{
			//_InitOne(_particles[i], position, velocity);
			_particles[i].active=true;
			_particles[i].draw = false;
			_particles[i].pos = position;
			_particles[i].vel = nv::vec3f(
				(float)(rand()%100-rand()%100)/1000
				, (float)(rand()%100-rand()%100)/1000
				, /*(float)(rand()%100+200)/300.0f**/velocity
				);
			_particles[i].vel_ang = nv::vec3f(0.f, 0.f, 0.f);
			_particles[i].color = nv::vec3f(1.f, 1.f, 1.f);
			_particles[i].dcolor = nv::vec3f(.01f, .01f, .01f);
			_particles[i].dim=(float)(rand()%100)/50;
			_particles[i].ddim=(float)0.1;
		}
		else
		{
			_particles[i].draw = true;
			_particles[i].pos += _particles[i].vel;
			_particles[i].vel /= 1.001f;
			_particles[i].dim += _particles[i].ddim;
			if(_particles[i].dim>1)
			{
				_particles[i].active=false;
				_particles[i].draw = false;
			}
			_particles[i].color -= _particles[i].dcolor;
			if(_particles[i].color.x == 0)
			{
				_particles[i].active=false;
				_particles[i].draw = false;
			}
		}
	}
}

void TailFlame::Draw(const nv::vec3f& cam_pos)
{
	for(int i = 0; i < _num_smoke; i++)
	{
		if(_particles[i].draw == true)
		{
			if(_particles[i].active==true)
				_DrawPaticle(
				_particles[i].pos,
				_particles[i].dim,
				_particles[i].color,
				cam_pos
				);
		}
	}
}

void TailFlame::_DrawPaticle(const nv::vec3f& pos, float dimention, const nv::vec3f& color, const nv::vec3f& cam_pos)
{
	nv::vec3f view = pos - cam_pos;
	nv::vec3f dir_eject(0.f, 0.f, 1.f);
	nv::vec3f cross1 = cross(view, dir_eject);
	nv::vec3f cross2 = cross(view, cross1);
	cross1 = nv::normalize(cross1);
	cross2 = nv::normalize(cross2);
	cross1 *= dimention;
	cross2 *= dimention;

	glEnable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc( (1,1,1,1), (1,1,1,1));
	bool mode = glxEnableLighting(false);
	COLORREF color_t = glxSetColor(RGB(255*color.x, 128*color.y, 76*color.z/3));
	bool mode_depth = glxEnableDepthTest(false);

	glBindTexture( GL_TEXTURE_2D, _tex );
	glBegin(GL_QUADS);
	glTexCoord2d( 0.0, 0.0 );
	glVertex3d( pos.x + cross1.x, pos.y + cross1.y, pos.z + cross1.z);
	glTexCoord2d( 1.0, 0.0 );
	glVertex3d( pos.x - cross2.x, pos.y - cross2.y, pos.z - cross2.z);
	glTexCoord2d( 1.0, 1.0 );
	glVertex3d( pos.x - cross1.x, pos.y - cross1.y, pos.z - cross1.z);
	glTexCoord2d( 0.0, 1.0 );
	glVertex3d( pos.x + cross2.x, pos.y + cross2.y, pos.z + cross2.z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable (GL_BLEND);
	glxEnableLighting(mode);
	glxSetColor(color_t);
	glxEnableDepthTest(mode_depth);
}