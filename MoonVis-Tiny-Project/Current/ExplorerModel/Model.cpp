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

#include "..\MoonVis\stdafx.h"
#include "stdafx.h"

#include <assert.h>
#include <windows.h>

#include <fstream>
#include <iostream>

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

bool g_is_use_aggregating = true; 
// true - vbo used, faster but a lot of GPU memory
// false - vertex array, slower (very slower) but tolerant large scene

#include "Model.h"
#include "DataFile.h"

#define BUFSIZE 512
static TCHAR   szTempPath[BUFSIZE];   
static TCHAR   szTempFile[BUFSIZE];   

#include "zlib/zlib.h"
extern int zlib_def(FILE *source, FILE *dest, int level);
extern int zlib_inf(FILE *source, FILE *dest);

static vec3f s_center(0, 0, 0);
static double s_scale = 1.f;
static matrix4f s_trf;

#define CALC_NRM

void SetCenter(vec3f &c)
{
	s_center = c;
}

void SetScale(double sc)
{
	s_scale = sc;
}

void SetMatrix(matrix4f &trf)
{
	s_trf = trf;
}

#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

void
FaceInfo::GetColor(unsigned long &color)
{
	color=RGB(_color._rgbs[0],_color._rgbs[1],_color._rgbs[2]);
}

void
FaceInfo::Display(MatInfo *matr, unsigned int *textureID, unsigned int *normalMapID)
{
	COLORREF color;
	GetColor(color);//这color貌似没有用上！
	//*************************************************
	//在下面的代码中使用material的属性
	if (color != 0) {
		glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
		glxSetMat(matr, textureID, normalMapID, color);
		glxEnableMat();
	}
	//************************************************

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer(3, GL_FLOAT, 3 * sizeof(float), _vdata);//_vdata[0].v);
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer(GL_FLOAT, 3 * sizeof(float), _ndata);// _ndata[0].v);
	glEnableClientState( GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 2*sizeof(float), _tdata);

/*	int loc = glxGetLoc();
	loc = 1;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 3*sizeof(float), _tangent[0].v);
*/	

	glDrawArrays(GL_TRIANGLES, 0, _num_triangle*3);

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY);

//	glDisableVertexAttribArray(loc);

	if (color != 0) {
		glPopAttrib();
	}
}

void
FaceInfo::Display()
{
	bool vbo = false;

	if (g_is_use_aggregating)
	{
		CreateVBO();

		if (glIsBuffer(_aboId)) {
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glBindBuffer(GL_ARRAY_BUFFER, _aboId);
			glInterleavedArrays(GL_T2F_N3F_V3F, 0, (GLfloat*)NULL + 0);

			glDrawArrays(GL_TRIANGLES, 0, _num_triangle * 3);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			vbo = true;
		}
	}
	

	if (!vbo) //vbo failed, so conventional way ...
	{
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer(3, GL_FLOAT, 3 * sizeof(float), _vdata);// _vdata[0].v);
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer(GL_FLOAT, 3 * sizeof(float), _ndata);//_ndata[0].v);
		glEnableClientState( GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 2*sizeof(float), _tdata);

		glDrawArrays(GL_TRIANGLES, 0, _num_triangle*3);

		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY);
	}
}

ModelInfo::ModelInfo(FILE *fp, matrix4f &trf)
: _color(0)
{
	vector<vec3f> pnts;
	vector<vec3f> nrms;
	vector<tri3f> vidxs;
	vector<tri3f> nidxs;

	struct fInfo {
		vector<tri3f> _vidxs, _nidxs;

		fInfo(vector <tri3f> &a, vector <tri3f> &b) {
			_vidxs = a;
			_nidxs = b;
		}
	};
	vector<fInfo> faces;

	char buffer[512];

	int line = 0;
	int action = 0;

	while (fgets(buffer, 512, fp)) {
		printf("Processing line %d ...\n", line++);

		if (action == 1) {// reading points
			if (strstr(buffer, "]}")) {
				action = 0;
				continue;
			}

			vec3f pt;
			//sscanf(buffer, "%g%g%g", pt[0], pt[1], pt[2]);
			sscanf(buffer, "%g%g%g", &pt.x, &pt.y, &pt.z);
			pnts.push_back(pt);
		}
		else if (action == 2) {// reading vectors
			if (strstr(buffer, "]}")) {
				action = 0;
				continue;
			}

			vec3f vct;
			//sscanf(buffer, "%g%g%g", vct[0], vct[1], vct[2]);
			sscanf(buffer, "%g%g%g", &vct.x, &vct.y, &vct.z);
			nrms.push_back(vct);
		}
		else if (action == 3) {// reading pnt index
			if (strstr(buffer, "]")) {
				action = 0;
				continue;
			}

			char *loc = buffer;
			while (strstr(loc, "-1,")) {
				int id1, id2, id3;
				sscanf(loc, "%d,%d,%d", &id1, &id2, &id3);
				vidxs.push_back(tri3f(id1, id2, id3));

				loc = strstr(loc, "-1,") + 3;
			}
		}
		else if (action == 4) {// reading nrm index
			if (strstr(buffer, "]")) {
				action = 0;

				if (vidxs.size() != nidxs.size()) {
					printf("Error: vidxs.size != nidxs.size\n");
					exit(0);
				}
				faces.push_back(fInfo(vidxs, nidxs));
				vidxs.clear();
				nidxs.clear();

				continue;
			}

			char *loc = buffer;
			while (strstr(loc, "-1,")) {
				int id1, id2, id3;
				sscanf(loc, "%d,%d,%d", &id1, &id2, &id3);
				nidxs.push_back(tri3f(id1, id2, id3));

				loc = strstr(loc, "-1,") + 3;
			}
		}
		else if (strstr(buffer, "point  [")) {
			action = 1; // read points
		}
		else if (strstr(buffer, "vector  [")) {
			action = 2; //read vectors
		}
		else if (strstr(buffer, "coordIndex [")) {
			action = 3; //read coordindex
		}
		else if (strstr(buffer, "normalIndex [")) {
			action = 4; // read normalindex
		}
	}

	fclose(fp);
	printf("Vrml parser done ...\n");

	printf("Vtx# = %d\n", pnts.size());
	printf("Nrm# = %d\n", nrms.size());
	printf("Tri# = %d\n", vidxs.size());

	// Now we get enough information
	_pid = 0;
	_num_vertex = pnts.size();
	_num_nrm = nrms.size();
	_vertices = new vec3f[_num_vertex];
	_nrms = new vec3f[_num_vertex];
	_normals = new vec3f[_num_nrm];

	for (unsigned int i=0; i<_num_vertex; i++) {
		_vertices[i] = pnts[i]*VTX_SCALE;
		vec3f t = _vertices[i];
		trf.mult_matrix_vec(t, _vertices[i]);
	}

	for (unsigned int i=0; i<_num_nrm; i++) {
		_normals[i] = nrms[i];
		vec3f t = _normals[i];
		trf.mult_matrix_dir(t, _normals[i]);
	}

	for (size_t i=0; i<faces.size(); i++) {
		unsigned int num_triangle = faces[i]._vidxs.size();
		tri3f *triangles = new tri3f[num_triangle];
		tri3f *normals = new tri3f[num_triangle];

		for (unsigned int j=0; j<num_triangle; j++) {
			triangles[j] = faces[i]._vidxs[j];
			normals[j] = faces[i]._nidxs[j];
		}

		FaceInfo *fac = new FaceInfo(triangles, normals, num_triangle, i);

#ifdef CALC_NRM
		for (unsigned int k=0; k<_num_vertex; k++)
			_nrms[k] = vec3f();

		fac->Compile1(_vertices, _nrms);
#else
		fac->Compile(_vertices, _normals);
#endif
		_faces.push_back(fac);
	}

	UpdateBox();
	// output to obj file
	/*
	fp = fopen("XXXX", "wt");

	for (size_t i=0; i<pnts.size(); i++)
	fprintf(fp, "v %f %f %f\n", pnts[i].z, pnts[i].x, pnts[i].y);

	for (size_t i=0; i<nrms.size(); i++)
	fprintf(fp, "nv %f %f %f\n", nrms[i].z, nrms[i].x, nrms[i].y);

	for (size_t i=0; i<vidxs.size(); i++)
	fprintf(fp, "f %d//%d %d//%d %d//%d\n",
	vidxs[i].id0()+1, nidxs[i].id0()+1,
	vidxs[i].id1()+1, nidxs[i].id1()+1,
	vidxs[i].id2()+1, nidxs[i].id2()+1);

	fclose(fp);
	*/

	_aboId = -1;

}

void
ModelInfo::CreateVBO()
{
	if (_aboId != -1)
		return;

	//---use the buffer object----> add by Meng.
	if (_faces.size() != 0) // for g_is_use_aggregating
	{
		std::vector<GLfloat> array_data_vec;

		for (int fi = 0; fi < _faces.size(); fi++)
		{
			FaceInfo* f = _faces[fi];
			int num_triangle = f->_num_triangle;
			vec3f* ndata = f->_ndata;
			vec3f* vdata = f->_vdata;
			float* tdata = f->_tdata;

			for (unsigned int i = 0; i < num_triangle * 3; i++)
			{
				array_data_vec.push_back(tdata[i * 2]);
				array_data_vec.push_back(tdata[i * 2 + 1]);

				array_data_vec.push_back(ndata[i][0]);// (ndata[i].x);
				array_data_vec.push_back(ndata[i][1]);// (ndata[i].y);
				array_data_vec.push_back(ndata[i][2]);// (ndata[i].z);

				array_data_vec.push_back(vdata[i][0]);// (vdata[i].x);
				array_data_vec.push_back(vdata[i][1]);// (vdata[i].y);
				array_data_vec.push_back(vdata[i][2]);// (vdata[i].z);
			}
		}
		_num_tri = array_data_vec.size() / 8 / 3;
		int data_size = _num_tri * 3 * 8 * sizeof(GLfloat);

		glGenBuffers(1, &_aboId);
		glBindBuffer(GL_ARRAY_BUFFER, _aboId);
		glBufferData(GL_ARRAY_BUFFER, _num_tri * 3 * 8 * sizeof(GLfloat), &array_data_vec[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	//-----------< end add by Meng.
}

ModelInfo::ModelInfo(FILE *fp)
: _color(0)
{
	fread(&_visible, sizeof(bool), 1, fp);
	fread(&_color, sizeof(unsigned long), 1, fp);

	_trsf.Load(fp);

	fread(&_pid, sizeof(unsigned int), 1, fp);
	_box.Load(fp);

	fread(&_num_vertex, sizeof(unsigned int), 1, fp);

	_vertices = new vec3f[_num_vertex];
	fread(_vertices, sizeof(vec3f), _num_vertex, fp);
	_nrms = new vec3f[_num_vertex];
	fread(_nrms, sizeof(vec3f), _num_vertex, fp);

	fread(&_num_nrm, sizeof(unsigned int), 1, fp);
	_normals = new vec3f[_num_nrm];
	fread(_normals, sizeof(vec3f), _num_nrm, fp);

	size_t num;
	fread(&num, sizeof(size_t), 1, fp);

	for (size_t i=0; i<num; i++) {
		_faces.push_back(new FaceInfo(fp));
	}

	for (size_t i=0; i<num; i++) {
#ifdef CALC_NRM
		_faces[i]->Compile1(_vertices, _nrms);
#else
		_faces[i]->Compile(_vertices, _normals);
#endif
	}

	_aboId = -1;

	//---check for a uniform color----> add by Tang, 2018/10/26
	color3 mdl_color;

	_uniform_color = true;
	for (int fi = 0; fi < _faces.size(); fi++)
	{
		FaceInfo* f = _faces[fi];
		if (fi == 0) {
			mdl_color = f->_color;
			continue;
		}

		if (!(f->_color == mdl_color)) {
			_uniform_color = false;
			break;
		}
	}
	//-----------< end add by Tang.

}

void
ModelInfo::UpdateBox()
{
	_box.empty();

	for (unsigned int i=0; i<_num_vertex; i++)
	{
		_box += _vertices[i];
	}
}

ModelInfo::~ModelInfo()
{
	delete [] _vertices;
	delete [] _normals;

	for (size_t i=0; i<_faces.size(); i++)
		delete _faces[i];

	_faces.clear();

	if (glIsBuffer(_aboId))
	{
		glDeleteBuffers(1, &_aboId);
	}
}

static float predefined_mat[15][3] = {
	{0xFF/255.0, 0x7F/255.0, 0x00/255.0},
	{1.f, 1.f, 0.f},
	{1.f, 0.2f, 0.f},
	{1.f, 0.6f, 1.f},
	{1.f, 1.f, 0.8f},
	{1.f, 0.f, 0.2f},
	{0.f, 1.f, 0.f},
	{0.f, 0.8f, 0.2f},
	{0.5f, 0.5f, 0.5f},
	{0.8f, 0.8f, 0.2f},
	{0.2f, 0.8f, 0.8f},
	{0.5f, 0.8f, 1.f},
	{0.6f, 0.8f, 0.1f},
	{0.2f, 0.2f, 0.2f},
	{0.2f, 0.8f, 0.8f},
};

static int count = 0;
void beginToonProgram();
void beginGoochProgram();
void beginProgram(bool tex_enable, bool bump_enable=false, bool show_grid = false, float r = 1.f, float g = 1.f, float b = 0.f, float shiness=30.f);
extern void endProgram();
extern GLuint dump_tex, flag_tex, wing_tex, noise_tex;

void
ModelInfo::DisplayFace(MatInfo *matr, unsigned int *textureID, unsigned int *normalMapID, bool bAnimate, int id, bool bShader)
{
	if (bAnimate) 
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(GetTransf().get_value());
	}

	glShadeModel(GL_SMOOTH);

	unsigned long color = _color;
	bool has_bump = false;
	bool has_tex = (color == 0) ? false : (textureID[color - 1] == 0 ? false : true);

	if (bShader) {
		if (color != 0) {
			beginProgram(has_tex, has_bump, false,
				GetRValue(color)/255.f,
				GetGValue(color)/255.f,
				GetBValue(color)/255.f);
		} else
			beginProgram(false);
	}

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noise_tex);

	for (FaceList::iterator it = _faces.begin(); it != _faces.end(); it++)
	{
		(*it)->Display(matr, textureID, normalMapID);
		//(*it)->Display();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	if (bShader)
		endProgram();

	glDisable(GL_CULL_FACE);

	if (bAnimate)
		glPopMatrix();
}

void 
ModelInfo::DisplayFace(bool bAnimate, bool hasTex, int id, bool bShader)
{

	GET_GLERROR
	if (bAnimate)
	{
		GET_GLERROR
		glMatrixMode(GL_MODELVIEW);
		GET_GLERROR
		glPushMatrix();
		GET_GLERROR
		glMultMatrixf(GetTransf().get_value());
		GET_GLERROR
	}

	glShadeModel(GL_SMOOTH);
	GET_GLERROR

	unsigned long color = _color;
	bool has_bump = false;

	if (bShader) {
		if (color != 0) {
			beginProgram(hasTex, has_bump, false,
				GetRValue(color)/255.f,
				GetGValue(color)/255.f,
				GetBValue(color)/255.f);
		} else
			beginProgram(false);
	}
	GET_GLERROR

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noise_tex);
	GET_GLERROR

	GET_GLERROR

	bool vbo = false;

	if (g_is_use_aggregating &&  id == -1)
	{
		CreateVBO();

		if (glIsBuffer(_aboId)) {
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			glBindBuffer(GL_ARRAY_BUFFER, _aboId);
			glInterleavedArrays(GL_T2F_N3F_V3F, 0, (GLfloat*)NULL + 0);

			glDrawArrays(GL_TRIANGLES, 0, _num_tri * 3);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			vbo = true;
		}
	}
	
	GET_GLERROR

	if (!vbo)
	{
		int count=0;

		if (id == -1)
			for (FaceList::iterator it=_faces.begin(); it!=_faces.end(); it++) 
			{
				float *mat = predefined_mat[(count++)%15];

				glxSetMaterial(mat[0], mat[1], mat[2], 0.f);
				glxEnableMaterial();

				//		_faces[100]->Display();
				(*it)->Display();
			}
		else
			_faces[id]->Display();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	if (bShader)
		endProgram();

	glDisable(GL_CULL_FACE);

	if (bAnimate)
		glPopMatrix();
}

void
ModelInfo::GetModelInfo(int &fNum, int &vNum, int &tNum)
{
	for (FaceList::iterator it=_faces.begin(); it!=_faces.end(); it++)
	{
		FaceInfo *face = (*it);
		fNum++;
		tNum += face->_num_triangle;
		vNum = 0;
	}
}

float
ModelInfo::GetVolume()
{
	float volume = 0.f;
	for (FaceList::iterator it=_faces.begin(); it!=_faces.end(); it++)
	{
		FaceInfo *face = (*it);

		for (unsigned int i=0; i<face->_num_triangle; i++) {
			tri3f &tri = face->_triangles[i];
			volume += TriangleVolume(_vertices[tri.id0()], _vertices[tri.id1()], _vertices[tri.id2()]);
		}
	}

	double total = s_scale *  s_scale * s_scale;
	return volume/total;
}

vec3f
ModelInfo::GetLowestVertex()
{
	if (0 == _num_vertex)
	{
		return vec3f(0.f, 1000000.f, 0.f);
	}

	unsigned int lowestId = 0;
	for (unsigned int i = 0; i < _num_vertex; i++)
	{
#ifdef FOR_CAR
		if(_vertices[i][1] < _vertices[lowestId][1])
		//if (_vertices[i].y < _vertices[lowestId].y)
#else
		if (_vertices[i].x < _vertices[lowestId].x)
#endif
		{
			lowestId = i;
		}
	}

	vec3f dst;
	set_pt(dst, _vertices[lowestId]);
	return dst;
}

float
ModelInfo::GetFaceArea(unsigned int id)
{
	FaceInfo *face = _faces[id];
	float area = 0.f;

	for (unsigned int i=0; i<face->_num_triangle; i++) {
		tri3f &tri = face->_triangles[i];
		area += TriangleArea(_vertices[tri.id0()], _vertices[tri.id1()], _vertices[tri.id2()]);
	}

	return area/(s_scale*s_scale);
}

static int
getTrianglePoints(vec3f &a, vec3f &b, vec3f &c, vec3f *pts, int xres, int yres)
{
	int idx=0;
 
	for (int i=0; i<xres; i++)
		for (int j=0; j<yres; j++) {
			float u = float(i)/xres;
			float v = float(j)/yres;
			float w = 1.f-u-v;

			if (w >= 0.f && w <= 1.f) {
				pts[idx++] = a*u+b*v+c*w;
			}
		}

	return idx;
}

bool
ModelInfo::PointInShadow(vec3f &pt, vec3f &sun)
{
	if (!_box.intersect(pt, sun))
		return false;

	for (FaceList::iterator it=_faces.begin(); it!=_faces.end(); it++) {
		FaceInfo *face = (*it);

		for (unsigned int i=0; i<face->_num_triangle; i++) {
			tri3f &tri = face->_triangles[i];
			float t;
			if (IntersectLnTri(_vertices[tri.id0()], _vertices[tri.id1()], _vertices[tri.id2()], pt, sun, t) && t > 0.f)
				return true;
		}

	}

	return false;
}

#define SHADOW_X_RES 10
#define SHADOW_Y_RES 10

float
ModelInfo::TriangleShadowRatio(vec3f &a, vec3f &b, vec3f &c, vec3f &sun, FCTInfo *fct, unsigned int pid)
{
	vec3f pts[SHADOW_X_RES*SHADOW_Y_RES];
	int count = getTrianglePoints(a, b, c, pts, SHADOW_X_RES, SHADOW_Y_RES);
	int shadow=0;

	for (int i=0; i<count; i++) {
		if (fct->PointInShadow(pts[i], sun, pid))
			shadow++;
	}

	return float(shadow)/count;
}

float
ModelInfo::GetShadowRatio(unsigned int id, vec3f &sun, FCTInfo *fct, unsigned int pid)
{
	float total = 0;
	float shadow = 0;

	for (unsigned int i=0; i<_faces[id]->_num_triangle; i++) {
		tri3f &tri = _faces[id]->_triangles[i];
		vec3f a = _vertices[tri.id0()];
		vec3f b = _vertices[tri.id1()];
		vec3f c = _vertices[tri.id2()];

		float area = TriangleArea(a, b, c);

		float ratio = TriangleShadowRatio(a, b, c, sun, fct, pid);

		total += area;
		shadow += ratio*area;
	}

	return shadow/total;
}

bool
ModelInfo::GetFacePlane(unsigned int id, vec3f &org, vec3f &norm)
{
	tri3f  &tri0 = _faces[id]->_triangles[0];

	vec3f p0 = _vertices[tri0.id0()];
	vec3f p1 = _vertices[tri0.id1()];
	vec3f p2 = _vertices[tri0.id2()];

	org = p0;
	norm = (p1-p0).cross(p2-p0);
	norm.normalize();

	return true;
}


inline void SwapPoint(vec3f& a, vec3f& b)
{vec3f tmp; tmp=a; a=b; b=tmp;}

// The smallest circle passing through two points
void SolveDisc2(vec3f p1, vec3f p2, vec3f & center, float &rad)
{
	center = (p1+p2)*0.5f;
	rad = (p1-center).length();
}

// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/

/*
Calculate the line segment PaPb that is the shortest route between
two lines P1P2 and P3P4. Calculate also the values of mua and mub where
Pa = P1 + mua (P2 - P1)
Pb = P3 + mub (P4 - P3)
Return FALSE if no solution exists.
*/
inline bool LineLineIntersect(const vec3f &p1, const vec3f &p2, const vec3f &p3, const vec3f &p4, vec3f &pa)
{
	vec3f p13,p43,p21;
	float d1343,d4321,d1321,d4343,d2121;
	float numer,denom;

	p13 = p1 - p3;
	p43 = p4 - p3;
	if (fabs(p43[0])  < GLH_EPSILON && fabs(p43[1])  < GLH_EPSILON && fabs(p43[2])  < GLH_EPSILON)
		return false;

	p21 = p2 - p1;
	if (fabs(p21[0])  < GLH_EPSILON && fabs(p21[1])  < GLH_EPSILON && fabs(p21[2])  < GLH_EPSILON)
		return false;

	d1343 = p13.dot(p43);
	d4321 = p43.dot(p21);
	d1321 = p13.dot(p21);
	d4343 = p43.dot(p43);
	d2121 = p21.dot(p21);

	denom = d2121 * d4343 - d4321 * d4321;
	if (fabs(denom) < GLH_EPSILON_2)
		return false;
	numer = d1343 * d4321 - d1321 * d4343;

	float mua = numer / denom;
	pa = p1 + p21*mua;
	return true;
}

// The unique circle passing through exactly three non-collinear points  p1, p2 ,p3
void SolveDisc3(vec3f p1, vec3f p2, vec3f p3, vec3f & center, float &rad)
{
	vec3f m1 = (p1+p2)*.5f;
	vec3f m2 = (p2+p3)*.5f;

	vec3f n = (p1-p2).cross(p2-p3);
	vec3f d1 = n.cross(p1-p2);
	vec3f d2 = n.cross(p2-p3);

	bool ret = LineLineIntersect(m1, m1+d1*100.f, m2, m2+d2*100.f, center);

	rad = (p1-center).length();
}

void
MiniDisc(vec3f *set, int n, vec3f *basis, int b, vec3f &center, float &rad)
{
	vec3f cl;
	float rl;
	int k;

	if (b==3)  SolveDisc3(basis[0], basis[1], basis[2], cl, rl);

	if ((n==1)&&(b==0)) {rl=0.0; cl=set[0];}
	if ((n==2)&&(b==0))  SolveDisc2(set[0],set[1], cl, rl); 
	if ((n==0)&&(b==2))  SolveDisc2(basis[0],basis[1], cl, rl); 
	if ((n==1)&&(b==1))  SolveDisc2(basis[0], set[0], cl, rl); 

	// General case
	if ((b<3)&&(n+b>2))
	{
		// Randomization: choosing a pivot
		k=rand()%n; // between 0 and n-1
		if (k!=0) SwapPoint(set[0],set[k]);

		MiniDisc(&set[1],n-1,basis,b,cl,rl);

		if ((cl-set[0]).length()>rl)
		{
			// Then set[0] necessarily belongs to the basis.
			basis[b++]=set[0];
			MiniDisc(&set[1],n-1,basis,b,cl,rl);
		}	
	}
	center=cl;rad=rl;
}

void
getCircle(vec3f *pnts, int num, vec3f &center, float &radius)
{
	vec3f basis[3];
	vec3f *set = new vec3f[num];

	memcpy(set, pnts, sizeof(vec3f)*num);
	MiniDisc(set, num, basis, 0, center, radius);
	delete [] set;
}

bool
ModelInfo::GetFaceAxis(unsigned int id, vec3f &org, vec3f &norm)
{
	if (_faces[id]->_num_triangle < 2)
		return false;

	tri3f &tri0 = _faces[id]->_triangles[0];
	vec3f p0 = _vertices[tri0.id0()];
	vec3f p1 = _vertices[tri0.id1()];
	vec3f p2 = _vertices[tri0.id2()];
	vec3f nrm1 = -(p2-p0).cross(p1-p0);
	nrm1.normalize();

	vec3f nrm2;

	for (unsigned int i=1; i<_faces[id]->_num_triangle; i++) {
		tri0 = _faces[id]->_triangles[i];
		p0 = _vertices[tri0.id0()];
		p1 = _vertices[tri0.id1()];
		p2 = _vertices[tri0.id2()];
		nrm2 = -(p2-p0).cross(p1-p0);
		nrm2.normalize();

		if (nrm1.dot(nrm2)>0.999f) // the same norm, just skip them
			continue;
		else
			break;
	}

	norm = nrm1.cross(nrm2);
	norm.normalize();

	float dist = _faces[id]->_vdata[0].dot(norm);
	vec3f *buffer = new vec3f[_faces[id]->_num_triangle*3];
	int idx=0;
	for (unsigned int i=1; i<_faces[id]->_num_triangle*3; i++) {
		vec3f p = _faces[id]->_vdata[i];
		if (fabs(p.dot(norm)-dist) < 0.001f) {
			buffer[idx++] = p;
		}
	}

	float radius;
	if (idx > 3) {
		getCircle(buffer, idx, org, radius);
		delete [] buffer;
		return true;
	} else {
		delete [] buffer;
		return false;
	}
}

bool
ModelInfo::GetFacePlaneExt(unsigned int id, vec3f &org, vec3f &norm)
{
	tri3f  &tri0 = _faces[id]->_triangles[0];

	vec3f p0 = _vertices[tri0.id0()];
	vec3f p1 = _vertices[tri0.id1()];
	vec3f p2 = _vertices[tri0.id2()];

	org = p0;
	norm = -(p2-p0).cross(p1-p0);
	norm.normalize();

	float radius;

	getCircle(_faces[id]->_vdata, _faces[id]->_num_triangle*3, org, radius);
	return true;
}

FaceInfo::FaceInfo(FILE *fp)
{
	fread(&_id, sizeof(unsigned int), 1, fp);
	fread(&_color, sizeof(color3), 1, fp);

	fread(&_num_triangle, sizeof(unsigned int), 1, fp);
	_triangles = new tri3f[_num_triangle];
	fread(_triangles, sizeof(tri3f), _num_triangle, fp);
	_normals = new tri3f[_num_triangle];
	fread(_normals, sizeof(tri3f), _num_triangle, fp);

	/*_vdata = new vec3f[_num_triangle*3];
	_ndata = new vec3f[_num_triangle*3];
	fread(_vdata, sizeof(vec3f), _num_triangle*3, fp);
	fread(_ndata, sizeof(vec3f), _num_triangle*3, fp);*/

	_vdata = _ndata = NULL;
	_tdata = NULL;
	_aboId = -1;
}

void
FaceInfo::CreateVBO()
{
	if (_aboId != -1)
		return;

	//---use the buffer object----> add by Meng.
	if (_num_triangle>0) // for g_is_use_aggregating
	{
		int total_vertices_num = _num_triangle * 3;
		GLfloat* array_data = new GLfloat[total_vertices_num * 8];
		for (unsigned int i = 0; i < _num_triangle * 3; i++)
		{
			array_data[i * 8 + 0] = _tdata[i * 2];
			array_data[i * 8 + 1] = _tdata[i * 2 + 1];

			array_data[i * 8 + 2] = _ndata[i][0];// .x;
			array_data[i * 8 + 3] = _ndata[i][1];// .y;
			array_data[i * 8 + 4] = _ndata[i][2];// .z;

			array_data[i * 8 + 5] = _vdata[i][0];// .x;
			array_data[i * 8 + 6] = _vdata[i][1];// .y;
			array_data[i * 8 + 7] = _vdata[i][2];// .z;

		}
		glGenBuffers(1, &_aboId);
		glBindBuffer(GL_ARRAY_BUFFER, _aboId);
		glBufferData(GL_ARRAY_BUFFER, total_vertices_num * 8 * sizeof(GLfloat), array_data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete[] array_data;
	}
	//-----------< end add by Meng.
}

void
FaceInfo::Compile1(vec3f *pnts, vec3f *nrms)
{
	for (unsigned int i=0; i<_num_triangle; i++) {
		unsigned int id0 = _triangles[i].id0();
		unsigned int id1 = _triangles[i].id1();
		unsigned int id2 = _triangles[i].id2();

		vec3f nrm = get_normal(pnts, id0, id1, id2);
		nrms[id0] += nrm;
		nrms[id1] += nrm;
		nrms[id2] += nrm;
	}

	_vdata = new vec3f[_num_triangle*3];
	_ndata = new vec3f[_num_triangle*3];

	int vidx=0, nidx = 0;
	for (unsigned int i=0; i<_num_triangle; i++) {
		set_pt(_vdata[vidx++], pnts[_triangles[i].id0()]);
		set_pt(_vdata[vidx++], pnts[_triangles[i].id1()]);
		set_pt(_vdata[vidx++], pnts[_triangles[i].id2()]);

		set_pt(_ndata[nidx++], nrms[_triangles[i].id0()]);
		set_pt(_ndata[nidx++], nrms[_triangles[i].id1()]);
		set_pt(_ndata[nidx++], nrms[_triangles[i].id2()]);
	}

	GenerateTexCoord();
}


vec3f getUpDir(vec3f &view)
{
	vec3f tmp;
	float ret = view.dot(vec3f(0.f, 1.f, 0.f));
	if (std::abs(ret) == 1.f)
	{
		tmp = view.cross(vec3f(1.f, 0.f, 0.f));
	}
	else
	{
		tmp = view.cross(vec3f(0.f, 1.f, 0.f));
	}

	vec3f up = tmp.cross(view);
	up.normalize();
	return up;
}

void
FaceInfo::GenerateTexCoord()
{
	if (_tdata) return;

	_tdata = new float[_num_triangle*3*2];
	vec3f *tdata = new vec3f[_num_triangle*3];

	vec3f n;

	if (true) {// to get a better projection direction: n
		BOX totalBx;
		for (int i = 0; i < _num_triangle * 3; i++)
			totalBx += _vdata[i];

		int choice = 0;
		float w = totalBx.width(); //dx
		float h = totalBx.height(); //dy
		float d = totalBx.depth(); //dz

		float xy = w*h;
		float xz = w*d;
		float yz = h*d;

		if (xy >= xz && xy >= yz) {
			n = vec3f(0, 0, 1);
		}
		else if (xz >= xy && xz >= yz) {
			n = vec3f(0, 1, 0);
		}
		else
			n = vec3f(1, 0, 0);
	}
	else {
		n = (_vdata[2] - _vdata[0]).cross(_vdata[1] - _vdata[0]);
		n.normalize();
	}

	vec3f p = _vdata[0];
	vec3f y = getUpDir(n);
	vec3f x = y.cross(n);
	x.normalize();

	BOX bx;
	for (unsigned int i=0; i<_num_triangle*3; i++) {
		//q_proj = q - dot(q - p, n) * n
		vec3f q = _vdata[i];
		vec3f proj = q - n*(q-p).dot(n) - p;
		tdata[i] = vec3f(proj.dot(x), proj.dot(y), proj.dot(n));
		bx += tdata[i];
	}

	vec3f c = bx.center();
	float width = bx.width();
	float height = bx.height();

	for (unsigned int i=0; i<_num_triangle*3; i++) {
		float x, y, z;
		x = tdata[i][0] / width;
		y = tdata[i][1] / height;
		z = tdata[i][2];
		//tdata[i].x /= width;
		//tdata[i].y /= height;
		tdata[i].set_value(x, y, z);
		tdata[i] = (tdata[i] + vec3f(1, 1, 0))*0.5f;

		_tdata[i * 2] = tdata[i][0];//.x;
		_tdata[i * 2 + 1] = tdata[i][1];// .y;
	}

	delete [] tdata;
}

void
FaceInfo::Compile(vec3f *pnts, vec3f *nrms)
{
	_vdata = new vec3f[_num_triangle*3];
	_ndata = new vec3f[_num_triangle*3];

	int vidx=0, nidx = 0;
	for (unsigned int i=0; i<_num_triangle; i++) {
		set_pt(_vdata[vidx++], pnts[_triangles[i].id0()]);
		set_pt(_vdata[vidx++], pnts[_triangles[i].id1()]);
		set_pt(_vdata[vidx++], pnts[_triangles[i].id2()]);

		set_pt(_ndata[nidx++], nrms[_normals[i].id0()]);
		set_pt(_ndata[nidx++], nrms[_normals[i].id1()]);
		set_pt(_ndata[nidx++], nrms[_normals[i].id2()]);
		/*

		_vdata[vidx++] = pnts[_triangles[i].id0()];
		_vdata[vidx++] = pnts[_triangles[i].id1()];
		_vdata[vidx++] = pnts[_triangles[i].id2()];

		_ndata[nidx++] = nrms[_normals[i].id0()];
		_ndata[nidx++] = nrms[_normals[i].id1()];
		_ndata[nidx++] = nrms[_normals[i].id2()];
		*/
	}

	GenerateTexCoord();
}


bool readobjfile(const char *path,
	unsigned int &numVtx, unsigned int &numTri, unsigned int &numNrm, unsigned int &numTex,
	tri3f *&tris, vec3f *&vtxs, vec3f *&nrms, vec2f *&texs, float scale, vec3f shift,
	unsigned int &numTTri, tri3f *&ttris, unsigned int &numNTri, tri3f *&ntris)
{
	vector<tri3f> triset, ntriset, ttriset;
	vector<vec2f> texset;
	vector<vec3f> nrmset;
	vector<vec3f> vtxset;

	FILE *fp = fopen(path, "rt");
	if (fp == NULL) return false;

	char buf[1024];
	while (fgets(buf, 1024, fp)) {
		if (buf[0] == 'v' && buf[1] == ' ') {
			double x, y, z;
			sscanf(buf + 2, "%lf%lf%lf", &x, &y, &z);

			vtxset.push_back(vec3f(x, y, z)*scale + shift);
		}
		else if (buf[0] == 'v' && buf[1] == 'n' && buf[2] == ' ') {
			double x, y, z;
			sscanf(buf + 3, "%lf%lf%lf", &x, &y, &z);

			nrmset.push_back(vec3f(x, y, z));
		}
		else if (buf[0] == 'v' && buf[1] == 't' && buf[2] == ' ') {
			double x, y;
			sscanf(buf + 3, "%lf%lf", &x, &y);

			texset.push_back(vec2f(x, y));
		}
		else
			if (buf[0] == 'f' && buf[1] == ' ') {
				int id0, id1, id2, id3;
				int tid0, tid1, tid2, tid3;
				int nid0, nid1, nid2, nid3;

				bool quad = false;

				sscanf(buf + 2, "%d/%d/%d", &id0, &tid0, &nid0);
				char *nxt = strchr(buf + 2, ' ');

				sscanf(nxt + 1, "%d/%d/%d", &id1, &tid1, &nid1);
				nxt = strchr(nxt + 1, ' ');
				sscanf(nxt + 1, "%d/%d/%d", &id2, &tid2, &nid2);

				nxt = strchr(nxt + 1, ' ');
				if (nxt != NULL && nxt[1] >= '0' && nxt[1] <= '9') {// quad
					if (sscanf(nxt + 1, "%d/%d/%d", &id3, &tid3, &nid3))
						quad = true;
				}

				id0--, id1--, id2--, id3--;
				nid0--, nid1--, nid2--, nid3--;
				tid0--, tid1--, tid2--, tid3--;

				triset.push_back(tri3f(id0, id1, id2));
				ttriset.push_back(tri3f(tid0, tid1, tid2));
				ntriset.push_back(tri3f(nid0, nid1, nid2));

				if (quad) {
					triset.push_back(tri3f(id0, id2, id3));
					ttriset.push_back(tri3f(tid0, tid2, tid3));
					ntriset.push_back(tri3f(nid0, nid2, nid3));
				}
			}
	}
	fclose(fp);

	if (triset.size() == 0 || vtxset.size() == 0)
		return false;

	numVtx = vtxset.size();
	vtxs = new vec3f[numVtx];
	for (unsigned int i = 0; i<numVtx; i++)
		vtxs[i] = vtxset[i];

	numTri = triset.size();
	tris = new tri3f[numTri];
	for (unsigned int i = 0; i<numTri; i++)
		tris[i] = triset[i];

	numNTri = ntriset.size();
	ntris = new tri3f[numNTri];
	for (unsigned int i = 0; i < numNTri; i++)
		ntris[i] = ntriset[i];

	numTTri = ttriset.size();
	ttris = new tri3f[numTTri];
	for (unsigned int i = 0; i < numTTri; i++)
		ttris[i] = ttriset[i];

	numNrm = nrmset.size();
	if (numNrm) {
		nrms = new vec3f[numNrm];
		for (unsigned int i = 0; i < numNrm; i++)
			nrms[i] = nrmset[i];
	}
	else
		nrms = NULL;

	numTex = texset.size();
	if (numTex) {
		texs = new vec2f[numTex];
		for (unsigned int i = 0; i < numTex; i++)
			texs[i] = texset[i];
	}
	else
		texs = NULL;

	return true;
}

objModel::objModel(char *path, float scale)
{
	unsigned int numVtx = 0, numTri = 0;
	unsigned int numNrm = 0, numTex = 0;
	vec3f *vtxs = NULL;
	tri3f *tris = NULL;
	vec3f *nrms = NULL;
	vec2f *texs = NULL;
	unsigned int numTTri = 0, numNTri = 0;
	tri3f *ttris = NULL, *ntris = NULL;

	if (readobjfile(path, numVtx, numTri, numNrm, numTex, tris, vtxs, nrms, texs, scale, vec3f(), numTTri, ttris, numNTri, ntris)) {
		_num_vertex = numVtx;
		_num_face = numTri;
		_num_ttri = numTTri;
		_num_ntri = numNTri;

		_vertices = vtxs;
		_nrms = nrms;
		_texs = texs;
		_tris = tris;
		_ttris = ttris;
		_ntris = ntris;
	}
	else {
		_num_vertex = 0;
		_num_face = 0;
		_vertices = 0;
		_num_ttri = 0;
		_num_ntri = 0;

		_ttris = 0;
		_ntris = 0;
		_nrms = 0;
		_texs = 0;
		_tris = 0;
	}

}
