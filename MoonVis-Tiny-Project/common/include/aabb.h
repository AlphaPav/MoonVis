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

#include "vec3f.h"
#include <float.h>
#include <stdlib.h>

inline void
vmin(vec3f &a, const vec3f &b)
{
	a.set_value(
		MIN(a[0], b[0]),
		MIN(a[1], b[1]),
		MIN(a[2], b[2]));
}

inline void
vmax(vec3f &a, const vec3f &b)
{
	a.set_value(
		MAX(a[0], b[0]),
		MAX(a[1], b[1]),
		MAX(a[2], b[2]));
}


class aabb {
public:
	vec3f _min;
	vec3f _max;

	FORCEINLINE aabb() {
		empty();
	}

	FORCEINLINE aabb(const vec3f &v) {
		_min = _max = v;
	}

	FORCEINLINE aabb(const vec3f &a, const vec3f &b) {
		_min = a;
		_max = a;
		vmin(_min, b);
		vmax(_max, b);
	}

	FORCEINLINE bool overlaps(const aabb& b) const
	{
		if (_min[0] > b._max[0]) return false;
		if (_min[1] > b._max[1]) return false;
		if (_min[2] > b._max[2]) return false;

		if (_max[0] < b._min[0]) return false;
		if (_max[1] < b._min[1]) return false;
		if (_max[2] < b._min[2]) return false;

		return true;
	}

	FORCEINLINE bool overlaps(const aabb &b, aabb &ret) const
	{
		if (!overlaps(b))
			return false;

		ret._min.set_value(
			MAX(_min[0],  b._min[0]),
			MAX(_min[1],  b._min[1]),
			MAX(_min[2],  b._min[2]));

		ret._max.set_value(
			MIN(_max[0], b._max[0]),
			MIN(_max[1], b._max[1]),
			MIN(_max[2], b._max[2]));

		return true;
	}

	FORCEINLINE bool inside(const vec3f &p) const
	{
		if (p[0] < _min[0] || p[0] > _max[0]) return false;
		if (p[1] < _min[1] || p[1] > _max[1]) return false;
		if (p[2] < _min[2] || p[2] > _max[2]) return false;

		return true;
	}

	FORCEINLINE aabb &operator += (const vec3f &p)
	{
		vmin(_min, p);
		vmax(_max, p);
		return *this;
	}

	FORCEINLINE aabb &operator += (const aabb &b)
	{
		vmin(_min, b._min);
		vmax(_max, b._max);
		return *this;
	}

	FORCEINLINE aabb operator + ( const aabb &v) const
		{ aabb rt(*this); return rt += v; }

	FORCEINLINE float width()  const { return _max[0] - _min[0]; }
	FORCEINLINE float height() const { return _max[1] - _min[1]; }
	FORCEINLINE float depth()  const { return _max[2] - _min[2]; }
	FORCEINLINE vec3f center() const { return (_min+_max)*0.5; }
	FORCEINLINE float volume() const { return width()*height()*depth(); }

	FORCEINLINE void empty() {
		_max = vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		_min = vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	FORCEINLINE bool isEmpty() const {
		return _max.x < _min.x;
	}


	FORCEINLINE void get_corners(vec3f corners[])
	{
		corners[0] = _min;
		corners[1] = vec3f(_min.x, _min.y, _max.z);
		corners[2] = vec3f(_min.x, _max.y, _min.z);
		corners[3] = vec3f(_max.x, _min.y, _min.z);
		corners[4] = vec3f(_max.x, _max.y, _min.z);
		corners[5] = vec3f(_min.x, _max.y, _max.z);
		corners[6] = vec3f(_max.x, _min.y, _max.z);
		corners[7] = _max;
	}

	FORCEINLINE void get_xy_corners(vec3f corners[])
	{
		corners[0] = vec3f(_min.x, _min.y, 0);
		corners[1] = vec3f(_min.x, _max.y, 0);
		corners[2] = vec3f(_max.x, _max.y, 0);
		corners[3] = vec3f(_max.x, _min.y, 0);
	}

	FORCEINLINE bool intersect(const vec3f &org, const vec3f &dir) {
		float tmin = (_min.x - org.x) / dir.x;
		float tmax = (_max.x - org.x) / dir.x;
		
		if (tmin > tmax) swap(tmin, tmax);
		
		float tymin = (_min.y - org.y) / dir.y;
		float tymax = (_max.y - org.y) / dir.y;   
		
		if (tymin > tymax) swap(tymin, tymax);   
		
		if ((tmin > tymax) || (tymin > tmax))
			return false;
		
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;
		
		float tzmin = (_min.z - org.z) / dir.z;
		float tzmax = (_max.z - org.z) / dir.z;
		
		if (tzmin > tzmax) swap(tzmin, tzmax);
		if ((tmin > tzmax) || (tzmin > tmax)) 
			return false;
		
		if (tzmin > tmin) tmin = tzmin;
		if (tzmax < tmax) tmax = tzmax;
		
		if (tmax < 0)
			return false;
		
		return true;
	}

	void Save(FILE *fp) {
		_min.Save(fp);
		_max.Save(fp);
	}

	void Load(FILE *fp) {
		_min.Load(fp);
		_max.Load(fp);
	}

	void visulization();
};