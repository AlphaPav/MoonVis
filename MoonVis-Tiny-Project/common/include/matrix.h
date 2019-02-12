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
#include "vec4f.h"

// matrix
class matrix4f
{
public:

	matrix4f() { make_identity(); }

	matrix4f( float r ) 
	{ set_value(r); }

	matrix4f( float * m )
	{ set_value(m); }

	matrix4f( float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33 )
	{
		element(0,0) = a00;
		element(0,1) = a01;
		element(0,2) = a02;
		element(0,3) = a03;

		element(1,0) = a10;
		element(1,1) = a11;
		element(1,2) = a12;
		element(1,3) = a13;

		element(2,0) = a20;
		element(2,1) = a21;
		element(2,2) = a22;
		element(2,3) = a23;

		element(3,0) = a30;
		element(3,1) = a31;
		element(3,2) = a32;
		element(3,3) = a33;
	}


	void get_value( float * mp ) const
	{
		int c = 0;
		for(int j=0; j < 4; j++)
			for(int i=0; i < 4; i++)
				mp[c++] = element(i,j);
	}


	const float * get_value() const
	{ return m; }

	void set_value( float * mp)
	{
		int c = 0;
		for(int j=0; j < 4; j++)
			for(int i=0; i < 4; i++)
				element(i,j) = mp[c++];
	}

	void set_value( float r ) 
	{
		for(int i=0; i < 4; i++)
			for(int j=0; j < 4; j++)
				element(i,j) = r;
	}

	void make_identity()
	{
		element(0,0) = 1.0;
		element(0,1) = 0.0;
		element(0,2) = 0.0; 
		element(0,3) = 0.0;

		element(1,0) = 0.0;
		element(1,1) = 1.0; 
		element(1,2) = 0.0;
		element(1,3) = 0.0;

		element(2,0) = 0.0;
		element(2,1) = 0.0;
		element(2,2) = 1.0;
		element(2,3) = 0.0;

		element(3,0) = 0.0; 
		element(3,1) = 0.0; 
		element(3,2) = 0.0;
		element(3,3) = 1.0;
	}


	static matrix4f identity()
	{
		static matrix4f mident (
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0  );
		return mident;
	}


	void set_scale( float s )
	{
		element(0,0) = s;
		element(1,1) = s;
		element(2,2) = s;
	}

	void set_scale( const vec3f & s )
	{
		element(0,0) = s.v[0];
		element(1,1) = s.v[1];
		element(2,2) = s.v[2];
	}


	void set_translate( const vec3f & t )
	{
		element(0,3) = t.v[0];
		element(1,3) = t.v[1];
		element(2,3) = t.v[2];
	}

	//http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/index.htm
	void set_rotate(const vec3f &dir, float angle)
	{
		float x = dir.x;
		float y = dir.y;
		float z = dir.z;

		element(0,0) = 1 + (1-cos(angle))*(x*x-1);
		element(0,1) = -z*sin(angle)+(1-cos(angle))*x*y;
		element(0,2) = y*sin(angle)+(1-cos(angle))*x*z;
		element(0,3) = 0;

		element(1,0) = z*sin(angle)+(1-cos(angle))*x*y;
		element(1,1) = 1 + (1-cos(angle))*(y*y-1);
		element(1,2) = -x*sin(angle)+(1-cos(angle))*y*z;
		element(1,3) = 0;

		element(2,0) = -y*sin(angle)+(1-cos(angle))*x*z;
		element(2,1) = x*sin(angle)+(1-cos(angle))*y*z;
		element(2,2) = 1 + (1-cos(angle))*(z*z-1);
		element(2,3) = 0;

		element(3,0) = 0;
		element(3,1) = 0;
		element(3,2) = 0;
		element(3,3) = 1;
	}

	void set_row(int r, const vec4f & t)
	{
		element(r,0) = t.v[0];
		element(r,1) = t.v[1];
		element(r,2) = t.v[2];
		element(r,3) = t.v[3];
	}

	void set_column(int c, const vec4f & t)
	{
		element(0,c) = t.v[0];
		element(1,c) = t.v[1];
		element(2,c) = t.v[2];
		element(3,c) = t.v[3];
	}


	void get_row(int r, vec4f & t) const
	{
		t.v[0] = element(r,0);
		t.v[1] = element(r,1);
		t.v[2] = element(r,2);
		t.v[3] = element(r,3);
	}

	vec4f get_row(int r) const
	{
		vec4f v; get_row(r, v);
		return v;
	}

	void get_column(int c, vec4f & t) const
	{
		t.v[0] = element(0,c);
		t.v[1] = element(1,c);
		t.v[2] = element(2,c);
		t.v[3] = element(3,c);
	}

	vec4f get_column(int c) const
	{
		vec4f v; get_column(c, v);
		return v;
	}

	matrix4f inverse() const
	{
		matrix4f minv;

		float r1[8], r2[8], r3[8], r4[8];
		float *s[4], *tmprow;

		s[0] = &r1[0];
		s[1] = &r2[0];
		s[2] = &r3[0];
		s[3] = &r4[0];

		register int i,j,p,jj;
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				s[i][j] = element(i,j);
				if(i==j) s[i][j+4] = 1.0;
				else     s[i][j+4] = 0.0;
			}
		}
		float scp[4];
		for(i=0;i<4;i++)
		{
			scp[i] = float(fabs(s[i][0]));
			for(j=1;j<4;j++)
				if(float(fabs(s[i][j])) > scp[i]) scp[i] = float(fabs(s[i][j]));
			if(scp[i] == 0.0) return minv; // singular matrix!
		}

		int pivot_to;
		float scp_max;
		for(i=0;i<4;i++)
		{
			// select pivot row
			pivot_to = i;
			scp_max = float(fabs(s[i][i]/scp[i]));
			// find out which row should be on top
			for(p=i+1;p<4;p++)
				if(float(fabs(s[p][i]/scp[p])) > scp_max)
				{ scp_max = float(fabs(s[p][i]/scp[p])); pivot_to = p; }
				// Pivot if necessary
				if(pivot_to != i)
				{
					tmprow = s[i];
					s[i] = s[pivot_to];
					s[pivot_to] = tmprow;
					float tmpscp;
					tmpscp = scp[i];
					scp[i] = scp[pivot_to];
					scp[pivot_to] = tmpscp;
				}

				float mji;
				// perform gaussian elimination
				for(j=i+1;j<4;j++)
				{
					mji = s[j][i]/s[i][i];
					s[j][i] = 0.0;
					for(jj=i+1;jj<8;jj++)
						s[j][jj] -= mji*s[i][jj];
				}
		}
		if(s[3][3] == 0.0) return minv; // singular matrix!

		//
		// Now we have an upper triangular matrix.
		//
		//  x x x x | y y y y
		//  0 x x x | y y y y 
		//  0 0 x x | y y y y
		//  0 0 0 x | y y y y
		//
		//  we'll back substitute to get the inverse
		//
		//  1 0 0 0 | z z z z
		//  0 1 0 0 | z z z z
		//  0 0 1 0 | z z z z
		//  0 0 0 1 | z z z z 
		//

		float mij;
		for(i=3;i>0;i--)
		{
			for(j=i-1;j > -1; j--)
			{
				mij = s[j][i]/s[i][i];
				for(jj=j+1;jj<8;jj++)
					s[j][jj] -= mij*s[i][jj];
			}
		}

		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				minv(i,j) = s[i][j+4] / s[i][i];

		return minv;
	}


	matrix4f transpose() const
	{
		matrix4f mtrans;

		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				mtrans(i,j) = element(j,i);		
		return mtrans;
	}

	matrix4f & mult_right( const matrix4f & b )
	{
		matrix4f mt(*this);
		set_value(float(0));

		for(int i=0; i < 4; i++)
			for(int j=0; j < 4; j++)
				for(int c=0; c < 4; c++)
					element(i,j) += mt(i,c) * b(c,j);
		return *this;
	}    

	matrix4f & mult_left( const matrix4f & b )
	{
		matrix4f mt(*this);
		set_value(float(0));

		for(int i=0; i < 4; i++)
			for(int j=0; j < 4; j++)
				for(int c=0; c < 4; c++)
					element(i,j) += b(i,c) * mt(c,j);
		return *this;
	}

	// dst = M * src
	void mult_matrix_vec( const vec3f &src, vec3f &dst ) const
	{
		float w = (
			src.v[0] * element(3,0) +
			src.v[1] * element(3,1) + 
			src.v[2] * element(3,2) +
			element(3,3)          );

		assert(w != GLH_ZERO);

		dst.v[0]  = (
			src.v[0] * element(0,0) +
			src.v[1] * element(0,1) +
			src.v[2] * element(0,2) +
			element(0,3)          ) / w;
		dst.v[1]  = (
			src.v[0] * element(1,0) +
			src.v[1] * element(1,1) +
			src.v[2] * element(1,2) +
			element(1,3)          ) / w;
		dst.v[2]  = (
			src.v[0] * element(2,0) +
			src.v[1] * element(2,1) + 
			src.v[2] * element(2,2) +
			element(2,3)          ) / w;
	}

	void mult_matrix_vec( vec3f & src_and_dst) const
	{ mult_matrix_vec(vec3f(src_and_dst), src_and_dst); }


	// dst = src * M
	void mult_vec_matrix( const vec3f &src, vec3f &dst ) const
	{
		float w = (
			src.v[0] * element(0,3) +
			src.v[1] * element(1,3) +
			src.v[2] * element(2,3) +
			element(3,3)          );

		assert(w != GLH_ZERO);

		dst.v[0]  = (
			src.v[0] * element(0,0) +
			src.v[1] * element(1,0) + 
			src.v[2] * element(2,0) + 
			element(3,0)          ) / w;
		dst.v[1]  = (
			src.v[0] * element(0,1) +
			src.v[1] * element(1,1) +
			src.v[2] * element(2,1) +
			element(3,1)          ) / w;
		dst.v[2]  = (
			src.v[0] * element(0,2) +
			src.v[1] * element(1,2) +
			src.v[2] * element(2,2) +
			element(3,2)          ) / w;
	}


	void mult_vec_matrix( vec3f & src_and_dst) const
	{ mult_vec_matrix(vec3f(src_and_dst), src_and_dst); }

	// dst = M * src
	void mult_matrix_vec( const vec4f &src, vec4f &dst ) const
	{
		dst.v[0]  = (
			src.v[0] * element(0,0) +
			src.v[1] * element(0,1) +
			src.v[2] * element(0,2) +
			src.v[3] * element(0,3));
		dst.v[1]  = (
			src.v[0] * element(1,0) +
			src.v[1] * element(1,1) +
			src.v[2] * element(1,2) +
			src.v[3] * element(1,3));
		dst.v[2]  = (
			src.v[0] * element(2,0) +
			src.v[1] * element(2,1) + 
			src.v[2] * element(2,2) +
			src.v[3] * element(2,3));
		dst.v[3] = (
			src.v[0] * element(3,0) +
			src.v[1] * element(3,1) + 
			src.v[2] * element(3,2) +
			src.v[3] * element(3,3));
	}

	void mult_matrix_vec( vec4f & src_and_dst) const
	{ mult_matrix_vec(vec4f(src_and_dst), src_and_dst); }


	// dst = src * M
	void mult_vec_matrix( const vec4f &src, vec4f &dst ) const
	{
		dst.v[0]  = (
			src.v[0] * element(0,0) +
			src.v[1] * element(1,0) + 
			src.v[2] * element(2,0) + 
			src.v[3] * element(3,0));
		dst.v[1]  = (
			src.v[0] * element(0,1) +
			src.v[1] * element(1,1) +
			src.v[2] * element(2,1) +
			src.v[3] * element(3,1));
		dst.v[2]  = (
			src.v[0] * element(0,2) +
			src.v[1] * element(1,2) +
			src.v[2] * element(2,2) +
			src.v[3] * element(3,2));
		dst.v[3] = (
			src.v[0] * element(0,3) +
			src.v[1] * element(1,3) +
			src.v[2] * element(2,3) +
			src.v[3] * element(3,3));
	}


	void mult_vec_matrix( vec4f & src_and_dst) const
	{ mult_vec_matrix(vec4f(src_and_dst), src_and_dst); }


	// dst = M * src
	void mult_matrix_dir( const vec3f &src, vec3f &dst ) const
	{
		dst.v[0]  = (
			src.v[0] * element(0,0) +
			src.v[1] * element(0,1) +
			src.v[2] * element(0,2) ) ;
		dst.v[1]  = ( 
			src.v[0] * element(1,0) +
			src.v[1] * element(1,1) +
			src.v[2] * element(1,2) ) ;
		dst.v[2]  = ( 
			src.v[0] * element(2,0) +
			src.v[1] * element(2,1) + 
			src.v[2] * element(2,2) ) ;
	}


	void mult_matrix_dir( vec3f & src_and_dst) const
	{ mult_matrix_dir(vec3f(src_and_dst), src_and_dst); }


	// dst = src * M
	void mult_dir_matrix( const vec3f &src, vec3f &dst ) const
	{
		dst.v[0]  = ( 
			src.v[0] * element(0,0) +
			src.v[1] * element(1,0) +
			src.v[2] * element(2,0) ) ;
		dst.v[1]  = ( 
			src.v[0] * element(0,1) +
			src.v[1] * element(1,1) +
			src.v[2] * element(2,1) ) ;
		dst.v[2]  = (
			src.v[0] * element(0,2) +
			src.v[1] * element(1,2) + 
			src.v[2] * element(2,2) ) ;
	}


	void mult_dir_matrix( vec3f & src_and_dst) const
	{ mult_dir_matrix(vec3f(src_and_dst), src_and_dst); }


	float & operator () (int row, int col)
	{ return element(row,col); }

	const float & operator () (int row, int col) const
	{ return element(row,col); }

	float & element (int row, int col)
	{ return m[row | (col<<2)]; }

	const float & element (int row, int col) const
	{ return m[row | (col<<2)]; }

	matrix4f & operator *= ( const matrix4f & mat )
	{
		mult_right( mat );
		return *this;
	}

	matrix4f & operator *= ( const float & r )
	{
		for (int i = 0; i < 4; ++i)
		{
			element(0,i) *= r;
			element(1,i) *= r;
			element(2,i) *= r;
			element(3,i) *= r;
		}
		return *this;
	}

	matrix4f & operator += ( const matrix4f & mat )
	{
		for (int i = 0; i < 4; ++i)
		{
			element(0,i) += mat.element(0,i);
			element(1,i) += mat.element(1,i);
			element(2,i) += mat.element(2,i);
			element(3,i) += mat.element(3,i);
		}
		return *this;
	}

	friend matrix4f operator * ( const matrix4f & m1,	const matrix4f & m2 );
	friend bool operator == ( const matrix4f & m1, const matrix4f & m2 );
	friend bool operator != ( const matrix4f & m1, const matrix4f & m2 );

	void Load(FILE *fp) {
		fread(m, sizeof(float), 16, fp);
	}

	//protected:
	float m[16];
};

inline  
matrix4f operator * ( const matrix4f & m1, const matrix4f & m2 )
{
	matrix4f product;

	product = m1;
	product.mult_right(m2);

	return product;
}

inline
bool operator ==( const matrix4f &m1, const matrix4f &m2 )
{
	return ( 
		m1(0,0) == m2(0,0) &&
		m1(0,1) == m2(0,1) &&
		m1(0,2) == m2(0,2) &&
		m1(0,3) == m2(0,3) &&
		m1(1,0) == m2(1,0) &&
		m1(1,1) == m2(1,1) &&
		m1(1,2) == m2(1,2) &&
		m1(1,3) == m2(1,3) &&
		m1(2,0) == m2(2,0) &&
		m1(2,1) == m2(2,1) &&
		m1(2,2) == m2(2,2) &&
		m1(2,3) == m2(2,3) &&
		m1(3,0) == m2(3,0) &&
		m1(3,1) == m2(3,1) &&
		m1(3,2) == m2(3,2) &&
		m1(3,3) == m2(3,3) );
}

inline
bool operator != ( const matrix4f & m1, const matrix4f & m2 )
{ return !( m1 == m2 ); }  
