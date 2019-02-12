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

#include <istream>
#include <ostream>
using namespace std;

#define     GLH_ZERO            float(0.0)
#define     GLH_EPSILON         float(10e-6)
#define		GLH_EPSILON_2		float(10e-12)
#define     equivalent(a,b)     (((a < b + GLH_EPSILON) && (a > b - GLH_EPSILON)) ? true : false)

#define MAX(a,b)	((a) > (b) ? (a) : (b))
#define MIN(a,b)	((a) < (b) ? (a) : (b))

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

class vec2f {
public:
	float x, y;

	vec2f() {
		x = y = 0;
	}

	vec2f(float x, float y) {
		this->x = x;
		this->y = y;
	}
};

class vec3f {
	union {
		struct {
		float x, y, z;
		};
		struct {
		float v[3];
		};
	};
public:

	FORCEINLINE vec3f ()
	{x=0; y=0; z=0;}

	FORCEINLINE vec3f(const vec3f &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	FORCEINLINE vec3f(const float *v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	FORCEINLINE vec3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	FORCEINLINE float operator [] ( int i ) const {return v[i];}

	FORCEINLINE vec3f &operator += (const vec3f &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	FORCEINLINE vec3f &operator -= (const vec3f &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	FORCEINLINE void negate() {
		x = -x;
		y = -y;
		z = -z;
	}

	FORCEINLINE vec3f operator - () const {
		return vec3f(-x, -y, -z);
	}

	FORCEINLINE vec3f operator+ (const vec3f &v) const
	{
		return vec3f(x+v.x, y+v.y, z+v.z);
	}

	FORCEINLINE vec3f operator- (const vec3f &v) const
	{
		return vec3f(x-v.x, y-v.y, z-v.z);
	}

	FORCEINLINE vec3f operator *(float t) const
	{
		return vec3f(x*t, y*t, z*t);
	}

     // cross product
     FORCEINLINE const vec3f cross(const vec3f &vec) const
     {
          return vec3f(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
     }

	 FORCEINLINE float dot(const vec3f &vec) const {
		 return x*vec.x+y*vec.y+z*vec.z;
	 }

	 FORCEINLINE void normalize() 
	 { 
		 float sum = x*x+y*y+z*z;
		 if (sum > GLH_EPSILON_2) {
			 float base = float(1.0/sqrt(sum));
			 x *= base;
			 y *= base;
			 z *= base;
		 }
	 }

	 FORCEINLINE float length() const {
		 return float(sqrt(x*x + y*y + z*z));
	 }

	FORCEINLINE vec3f & set_value( const float &vx, const float &vy, const float &vz)
	{ x = vx; y = vy; z = vz; return *this; }

	FORCEINLINE bool equal_abs(const vec3f &other) {
		return x == other.x && y == other.y && z == other.z;
	}

	FORCEINLINE float square_norm() const {
		return x*x+y*y+z*z;
	}

	void getNZ(unsigned char rgb[]) {
		rgb[0] = (x+1.f)*0.5*255;
		rgb[1] = (y+1.f)*0.5f*255;
		rgb[2] = (z+1.f)*0.5f*255;
	}

	void SaveNZ(FILE *fp) {
		unsigned char rgb[3];

		rgb[0] = (x+1.f)*0.5*255;
		rgb[1] = (y+1.f)*0.5f*255;
		rgb[2] = (z+1.f)*0.5f*255;
		
		fwrite(rgb, sizeof(unsigned char), 3, fp);
	}

	void setNZ(unsigned char rgb[]) {
		x = rgb[0]*2.f/255.f-1.f;
		y = rgb[1]*2.f/255.f-1.f;
		z = rgb[2]*2.f/255.f-1.f;
	}

	void LoadNZ(FILE *fp) {
		unsigned char rgb[3];

		fread(rgb, sizeof(unsigned char), 3, fp);
		x = rgb[0]*2.f/255.f-1.f;
		y = rgb[1]*2.f/255.f-1.f;
		z = rgb[2]*2.f/255.f-1.f;
	}

	void Save(FILE *fp) {
		fwrite(&x, sizeof(float), 1, fp);
		fwrite(&y, sizeof(float), 1, fp);
		fwrite(&z, sizeof(float), 1, fp);
	}

	void Load(FILE *fp) {
		fread(&x, sizeof(float), 1, fp);
		fread(&y, sizeof(float), 1, fp);
		fread(&z, sizeof(float), 1, fp);
	}

	friend wistream & operator >> (wistream &is, vec3f &v);
	friend wostream & operator << (wostream &os, vec3f &v);
#ifdef _DEBUG
	FORCEINLINE void output() {
		printf("x=%f, y=%f, z=%f\n", x, y, z);
	}
#endif
};

inline wistream &
operator >>(wistream &is, vec3f &v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

inline wostream &
operator <<(wostream &os, const vec3f &v)
{
	os << v[0] << endl;//	os << v.x << endl;
	os << v[1] << endl;// os << v.y << endl;
	os << v[2] << endl;// os << v.z << endl;
	return os;
}

/// Gets the angle (in radians) between two vectors or two unit vectors
/// in the range 0 <= angle < 2 pi
inline double Determinant( const vec3f & A, const vec3f & B, const vec3f & C)
{
	return A[0] * B[1] * C[2] + A[1] * B[2] * C[0] + A[2] * B[0] * C[1] -
		A[2] * B[1] * C[0] - A[1] * B[0] * C[2] - A[0] * B[2] * C[1];
	//return A.x* B.y* C.z+ A.y* B.z* C.x+ A.z* B.x* C.y- 
	//	   A.z* B.y* C.x- A.y* B.x* C.z- A.x* B.z* C.y;
}

inline double Determinant(	const vec3f & A, const vec3f & B, vec3f * pUp)
{
	// The default normal is (0,0,1);
	vec3f N(0, 0, 1);
	
	if(!pUp)
		N = A.cross(B);
	else
		N = *pUp;

	return Determinant(A, B, N);
}

inline double  Angle( const vec3f & A, const vec3f & B, vec3f * pUp/* = NULL */)
{
	double rSin = Determinant(A, B, pUp);

	double rCos = A.dot(B);

	if ((fabs(rCos) <= 1e-9)  && (fabs(rSin)<= 1e-9))
		return  0.0;
	else
		return	atan2(rSin, rCos);
}

// rotate a vector around an axis which passes through 
// the origin of the coordinate system
// notes and assumptions :	
//		1. the rotaion angle <theta> is specified in 
//		   counter clockwise order from the positive 
//		   side of the axis;
//		2. the axis is assumed to be normalised;
//		3. if the rotated vector is a null vector, 
//		   return NULL;
inline bool RotateVector(const vec3f& axis, vec3f & vec, float angle)
{
	if( sin(Angle(axis, vec, NULL)) < 1e-9 )
		return true;

	float	d = vec.dot(axis), vlen = vec.length();
	vec3f v1 = vec - axis * d;
	v1 = v1 * cos(angle);

	vec3f v2 = axis.cross(vec);
	v2 = v2 * sin(angle);

	vec = v1 + v2 + axis*d;
	vec.normalize();
	vec = vec*vlen;

	return true;
}

inline double AngleBetween(vec3f const & v1, vec3f const & v2)
{
	double j1, j2;
	j1 = v1.length();
	j2 = v2.length();
	double c = v1.dot(v2);

	double cos = c/ (j1 * j2);
	double sin = sqrt(1-cos*cos);
	
	if ((fabs(cos) <= 1e-9)  && (fabs(sin)<= 1e-9))
		return  0.0;

// 	double tan = sin/cos;
//	return acos( c/ (j1 * j2));
// 	return atan(tan);
	if(fabs(sin)<= 1e-9)
		return (cos > 0.0 ? 0.0 : M_PI);
	else if(sin > 1e-9)
		return acos(cos);
	else
		return (2 * M_PI - acos(cos));
}

inline double DistPntLn(vec3f const &pt, vec3f const &org, vec3f const &dir)
{
	return (dir.cross(org-pt)).length();
}

inline float DistLnLn(vec3f const &org1, vec3f const &dir1, vec3f const &org2, vec3f const &dir2, float &t1, float &t2)
{
	float delta = dir1.dot(dir2);
	float delta2 = delta*delta;
	if (delta2 >= 1) return 9999.f;

	delta2 = 1.f/(1-delta2);

	vec3f r12 = org2-org1;
	float r12e1 = r12.dot(dir1);
	float r12e2 = r12.dot(dir2);

	t1 = r12e1-r12e2*delta;
	t1 *= delta2;

	t2 = r12e1*delta-r12e2;
	t2 *= delta2;

	return (r12+dir2*t2-dir1*t1).length();
}

inline float TriangleArea(vec3f &a, vec3f &b, vec3f c)
{
	float la = (b-a).length();
	float lb = (c-b).length();
	float lc = (a-c).length();

	float p = (la+lb+lc)*0.5f;

	return sqrt(p*(p-la)*(p-lb)*(p-lc));
}

// projected to Z=0 plane
inline float TriangleVolume(vec3f &a, vec3f &b, vec3f c)
{
	return (a[2] + b[2] + c[2])*(a[0] * b[1] - b[0] * a[1] + b[0] * c[1] - c[0] * b[1] + c[0] * a[1] - a[0] * c[1]) / 6.f;
	//return (a.z+b.z+c.z)*(a.x*b.y-b.x*a.y+b.x*c.y-c.x*b.y+c.x*a.y-a.x*c.y)/6.f;
	//V  =  (z1+z2+z3)(x1y2-x2y1+x2y3-x3y2+x3y1-x1y3)/6
}

/*
 * @param a,b,c为三角形面片的三个顶点
 * @param o为射线的起点origin
 * @param d为射线的方向dir
 * @param o+t*d = (1-u-v)a+u*b+v*c;
 */

inline bool IntersectLnTri(vec3f const &a, vec3f const &b, vec3f  const &c, vec3f const &o, vec3f const &d, float &t)
{
	// uses intersection test from
	//"a fast minimum-storage triangle intersection test"
	//lasthit contains information of the previous hit -
	vec3f e1 = b - a;
	vec3f e2 = c - a;
	vec3f p = d.cross(e2);
	float det = p.dot(e1);

	if (det > -0.00001f && det < 0.00001f)
		return false;

	float invdet = 1.0f / det;
	vec3f tvec = o - a;
	float u = p.dot(tvec) * invdet;
	vec3f q = tvec.cross(e1);
	float v = q.dot(d) * invdet;
	
	if ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f)) {
		t = q.dot(e2) * invdet;
		return true;
	} else
		return false;
}

inline void set_pt(vec3f& dst, vec3f& src)
{
#ifdef FOR_CAR
	dst = src;
#else
	dst.x = src.z;
	dst.y = src.x;
	dst.z = src.y;
#endif
}

inline void unset_pt(vec3f& dst, vec3f& src)
{
#ifdef FOR_CAR
	dst = src;
#else
	dst.x = src.y;
	dst.y = src.z;
	dst.z = src.x;
#endif
}