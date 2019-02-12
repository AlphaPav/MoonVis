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

#ifndef TAILFLAME_H
#define TAILFLAME_H

#include <nvImage.h>
#include <nvMath.h>
#include <glx.h>
#include "Model.h"

class TailFlame
{
protected:
	struct Smoke{
		bool active;
		bool draw;
		nv::vec3f pos;
		nv::vec3f vel;
		nv::vec3f vel_ang;
		nv::vec3f color;
		nv::vec3f dcolor;
		float dim;
		float ddim;
	};

	int _num_smoke;
	Smoke *_particles;
	float _height;
	float _width;
	static GLuint _tex;

public:
	TailFlame(){}

	TailFlame(float h, float w, int num = 128)
		: _height(h) , _width(w) 
		, _num_smoke(num)

	{
		_particles = new Smoke[_num_smoke];
		Reset();
	}

	~TailFlame(){ delete[] _particles;}

	static void InitData(const char tex_name[]);
	void Reset();
	void Step(const nv::vec3f& position, float velocity, float dt);
	void Draw(const nv::vec3f& cam_pos);

protected:
	void _DrawPaticle(const nv::vec3f& pos, float dimentsion, const nv::vec3f& color, const nv::vec3f& cam_pos);
};

#endif