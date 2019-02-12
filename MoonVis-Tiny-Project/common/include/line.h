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

class line {
public:
	vec3f _org;
	vec3f _dir;

	FORCEINLINE line() {
		empty();
	}

	FORCEINLINE line(const vec3f &org, const vec3f &dir) {
		_org = org;
		_dir = dir;
	}

	FORCEINLINE void empty() {
		_org = vec3f(0.f, 0.f, 0.f);
		_dir = vec3f(0.f, 0.f, 1.f);
	}

	void Save(FILE *fp) {
		_org.Save(fp);
		_dir.Save(fp);
	}

	void Load(FILE *fp) {
		_org.Load(fp);
		_dir.Load(fp);
	}

	void Load(wistream &is) {
		vec3f tem;
		is>>tem;
		set_pt(_org, tem);
		is>>tem;
		set_pt(_dir, tem);
		
	/*
		is >> _org;
			is >> _dir;
		*/
	}

	friend wostream & operator << (wostream &os, line &);
};

inline wostream &operator << (wostream &os, const line &item)
{
	os << item._org << endl;
	os << item._dir << endl;
	return os;
}