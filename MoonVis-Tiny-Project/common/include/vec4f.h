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

#include <math.h>
#include <stdio.h>
#include "forceinline.h"
#include "vec3f.h"
#include <assert.h>

class vec4f {
	union {
		struct {
		float x, y, z, w;
		};
		struct {
		float v[3];
		};
	};
public:
	FORCEINLINE vec4f() {
		x = y = z = w = 0.f;
	}

	FORCEINLINE vec4f(const vec4f &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	FORCEINLINE vec4f(const float *v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}

	FORCEINLINE vec4f(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	FORCEINLINE vec4f(const vec3f &t, float fourth)
	{ v[0] = t.v[0]; v[1] = t.v[1]; v[2] = t.v[2]; v[3] = fourth; }

	void get_value(float & x, float & y, float & z, float & w) const
	{ x = v[0]; y = v[1]; z = v[2]; w = v[3]; }

	vec4f & set_value( const float & x, const float & y, const float & z, const float & w)
	{ v[0] = x; v[1] = y; v[2] = z; v[3] = w; return *this; }
};

inline
vec3f homogenize(const vec4f & v)
{
	vec3f rt;
	assert(v.v[3] != GLH_ZERO);
	rt.v[0] = v.v[0]/v.v[3];
	rt.v[1] = v.v[1]/v.v[3];
	rt.v[2] = v.v[2]/v.v[3];
	return rt;
}