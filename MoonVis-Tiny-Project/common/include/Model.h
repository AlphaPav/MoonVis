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
// Author: Tang, Min tang_m@zju.edu.cn

#pragma once

#include <vec3f.h>
#include "box.h"
//#include "hit.h"
#include "matrix.h"
#include "line.h"
#include <gl/glew.h>
#include <gl/gl.h>
#include "glx.h"

#include <vector>
using namespace std;

#include <istream>
#include <assert.h>

// This scale is only used for converting from 1m to 1cm, so do not change it ...
// To make the model looks larger, change the SCALE_MDL in main.h instead.
// By tangmin, 2013/4/26
#define VTX_SCALE 100.f

class FCTInfo;
class ModelInfo;
class FaceInfo;
struct MatInfo;

typedef vector<FaceInfo *> FaceList;

class color3 {
public:
	unsigned char _rgbs[3];

	FORCEINLINE color3() {
		_rgbs[0] = _rgbs[1] = _rgbs[2] = 0;
	}

	FORCEINLINE void set(unsigned char rgb[]) {
		_rgbs[0] = rgb[0];
		_rgbs[1] = rgb[1];
		_rgbs[2] = rgb[2];
	}

	FORCEINLINE void set(unsigned char r, unsigned char g, unsigned char b) {
		_rgbs[0] = r;
		_rgbs[1] = g;
		_rgbs[2] = b;
	}

	friend bool operator == (const color3 & c1, const color3 & c2);
};


inline
bool operator ==(const color3 &c1, const color3 &c2)
{
	return (c1._rgbs[0] == c2._rgbs[0] &&
			c1._rgbs[1] == c2._rgbs[1] &&
			c1._rgbs[2] == c2._rgbs[2]);
}

class tri3f {
	unsigned int _ids[3];

public:
	FORCEINLINE tri3f() {
		_ids[0] = _ids[1] = _ids[2] = UINT_MAX;
	}

	FORCEINLINE tri3f(unsigned int id0, unsigned int id1, unsigned id2) {
		set(id0, id1, id2);
	}

	FORCEINLINE void set(unsigned int id0, unsigned int id1, unsigned int id2) {
		_ids[0] = id0;
		_ids[1] = id1;
		_ids[2] = id2;
	}

	FORCEINLINE unsigned int id(int i) { return _ids[i]; }
	FORCEINLINE unsigned int id0() {return _ids[0];}
	FORCEINLINE unsigned int id1() {return _ids[1];}
	FORCEINLINE unsigned int id2() {return _ids[2];}
	friend wistream & operator >> (wistream &is, tri3f &t);
	friend wostream & operator << (wostream &os, tri3f &t);

	FORCEINLINE void Save(FILE *fp) {
		fwrite(_ids, sizeof(unsigned int), 3, fp);
	}

	FORCEINLINE void Load(FILE *fp) {
		fread(_ids, sizeof(unsigned int), 3, fp);
	}
};

inline wistream &
operator >>(wistream &is, tri3f &t)
{
	/*{
	unsigned int num;
	is >> num;
	assert(num == 3);
	}*/

	is >> t._ids[0] >> t._ids[1] >> t._ids[2];
	return is;
}

inline wostream &
operator << (wostream &os, tri3f &t)
{
	//os << int(3) << endl;
	os << t._ids[0] << endl;
	os << t._ids[1] << endl;
	os << t._ids[2] << endl;
	return os;
}

class objModel {
public:
	unsigned int _num_vertex, _num_face, _num_ttri, _num_ntri;
	vec3f *_vertices;
	vec3f *_nrms;
	vec2f *_texs;
	tri3f *_tris;
	tri3f *_ttris;
	tri3f *_ntris;

	objModel(char *, float scale);
};

class ModelInfo {
	bool _visible;
	unsigned long _color;
	// if it is ture, a uniformed color (or no color) for the whole model
	// add by Tang 2018/10/26
	bool _uniform_color;

	matrix4f _trsf;
	matrix4f _trsf_local;
	matrix4f _trsf_local_base;

	unsigned int _pid;
	BOX _box;

	unsigned int _num_vertex;
	vec3f *_vertices;
	vec3f *_nrms;

	unsigned int _num_nrm;
	vec3f *_normals;

	FaceList _faces;

	//-- add by Meng
	GLuint _aboId;
	unsigned int _num_tri;
	//-- end add by Meng

	void CreateVBO();

public:

public:
	ModelInfo(FILE *fp, matrix4f &);
	//	ModelInfo(wstring &name, matrix4f &);
	ModelInfo(FILE *fp);

	~ModelInfo();


	//==
	//void DisplayFace(MatInfo *matr, bool bAnimate, int id=-1, bool bShader=true);
	//void DisplayFace(bool bAnimate, int id=-1, bool bShader=true);

	void DisplayFace(MatInfo * matr, unsigned int * textureID, unsigned int * normalMapID, bool bAnimate, int id=-1, bool bShader=true);
	void DisplayFace(bool bAnimate, bool hasTex, int id=-1, bool bShader=true);

	void UpdateBox();

	BOX GetBox() const {	return _box; }
	void SetID(unsigned int id) { _pid = id; }
	unsigned int GetID() const { return _pid; }

	bool GetVtxCoord(unsigned int, vec3f &);
	bool GetEdgeCoord(unsigned int, vec3f &);
	bool GetFaceCoord(unsigned int, vec3f &);
	bool GetEdgeLnSeg(unsigned int, vec3f &, vec3f &);
	bool GetEdgeAxis(unsigned int, line &);

	float TriangleShadowRatio(vec3f &a, vec3f &b, vec3f &c, vec3f &sun, FCTInfo *fct, unsigned int pid);
	float GetShadowRatio(unsigned int, vec3f&, FCTInfo *, unsigned int pid);
	bool PointInShadow(vec3f &pt, vec3f &sun);

	bool GetFacePlane(unsigned int, vec3f &, vec3f &);
	bool GetFacePlaneExt(unsigned int, vec3f &, vec3f &);
	bool GetFaceAxis(unsigned int, vec3f &, vec3f &);

	bool IsLnSeg(unsigned int);

	float GetFaceArea(unsigned int);

	void SetColor(unsigned long color) {
		_color = color;
	}
	unsigned long GetColor() {
		return _color;
	}

	void GetModelInfo(int &fNum, int &vNum, int &tNum);
	float GetVolume();
	vec3f GetLowestVertex();

	matrix4f GetTransf() { return _trsf; }
	void SetTransf(matrix4f &trf) {
		_trsf = trf;
	}
	void ApplyTransf(matrix4f &trf) {
		_trsf *= trf;
	}

	const matrix4f& GetTransfLocal() const { return _trsf_local; }
	void SetTransfLocal(matrix4f &trf) { _trsf_local = trf; }

	const matrix4f& GetTransfLocalBase() const { return _trsf_local_base; }
	void SetTransfLocalBase(matrix4f &trf) { _trsf_local_base = trf;}

	FaceInfo *GetFace0() {
		if (_faces.size() == 0)
			return NULL;

		return _faces[0];
	}

	bool IsUniformColor() const { return _uniform_color; }
};

inline vec3f get_normal(vec3f *pnts, unsigned int id0, unsigned int id1, unsigned int id2)
{

	vec3f t = (pnts[id0]-pnts[id1]).cross(pnts[id0]-pnts[id2]);
	t.normalize();
	return t;
}

class FaceInfo {
public:
	unsigned int _id;
	color3 _color;

	unsigned int _num_triangle;
	tri3f *_triangles;
	tri3f *_normals;

	vec3f *_vdata, *_ndata;
	float *_tdata;
	GLuint _aboId; // array buffer

	void CreateVBO();

public:
	FaceInfo(tri3f *tris, tri3f *nrms, unsigned int num, unsigned int id) {
		_triangles = tris;
		_normals = nrms;
		_num_triangle = num;

		_id = id;
		_vdata = _ndata = NULL;
		_tdata = NULL;
		_aboId = -1;
	}
	FaceInfo(FILE *fp);

	~FaceInfo() {
		if (_triangles)
			delete [] _triangles;

		if (_normals)
			delete [] _normals;

		if (_vdata)
			delete [] _vdata;
		if (_ndata)
			delete [] _ndata;
		if (_tdata)
			delete [] _tdata;

		if (glIsBuffer(_aboId))
		{
			glDeleteBuffers(1, &_aboId);
		}
	}

	void Display(MatInfo *matr);
	void Display();

	void GetColor(unsigned long &color);
	void Display(MatInfo * matr, unsigned int * textureID, unsigned int * normalMapID);
	void Compile(vec3f *pnts, vec3f *nrms);	
	void Compile1(vec3f *pnts, vec3f *nrms);
	void GenerateTexCoord();

	friend class ModelInfo;
};

