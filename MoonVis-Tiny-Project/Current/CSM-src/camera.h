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

// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.

#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "gl/GL.h"
#include <nvMath.h>
#include "DataFile.h"
#include "Model.h"

#define FILM_DIAGONAL 36
#define RAD_UNIT 0.017453292519943
class RenderTexture;

class camera
{
protected:
	nv::vec3f _pos;
	nv::vec3f _init_pos;
	nv::vec3f _view;
	nv::vec3f _init_view;
	nv::vec3f _up;
	nv::vec3f _init_up;
	float _neard, _fard;
	float _fov, _ratio;
	int _xres, _yres;
	int _freq;
	int _id;
	char _name[512];
	int _mdl_id;
	float _mtf;
	float _led;

	float _dn, _df; // for DOF
	int _bdepth, _bdof;

	bool _bvis; // visible or not
	float _length;

	RenderTexture *_fbos[3];

public:
	camera();
	camera(nv::vec3f pos, nv::vec3f view);
	camera(
		float pos[3], float view[3], 
		float neard, float fard, 
		float fov, float ratio,
		int xres, int yres,
		int freq, float mtf = 1.f, float led = 0.f
		);

	int getID() const { return _id; }

	void setMdlId(int mdl_id) { _mdl_id = mdl_id; }
	int getMdlId() const { return _mdl_id; }

	void getInitPos(float p[3]) const {
		p[0] = _init_pos.x;
		p[1] = _init_pos.y;
		p[2] = _init_pos.z;
	}

	void getInitView(float v[3]) const {
		v[0] = _init_view.x;
		v[1] = _init_view.y;
		v[2] = _init_view.z;
	}

	void getPos(float p[3]) const {
		p[0] = _pos.x;
		p[1] = _pos.y;
		p[2] = _pos.z;
	}

	void getView(float v[3]) const {
		v[0] = _view.x;
		v[1] = _view.y;
		v[2] = _view.z;
	}

	int getXRes() const { return _xres; }
	int getYRes() const { return _yres; }
	float getNear() const { return _neard; }
	float getFar() const { return _fard; }
	float getFov() const { return _fov; }
	float getRatio() const {return _ratio; }
	int getFreq() const { return _freq; }

	float getRectFov() const {
		float ret = atan(_yres/sqrt(float(_xres*_xres+_yres*_yres))*tan(_fov*0.5f));
		return ret * 2.f;
	}

	void setPos(float p[3]) {
		_pos.x = p[0];
		_pos.y = p[1];
		_pos.z = p[2];
	}

	void setView(float v[3]) {
		_view.x = v[0];
		_view.y = v[1];
		_view.z = v[2];
	}

	void setName(char name[])
	{
		strcpy(_name, name);
	}

	char* getName()
	{
		return _name;
	}

	void translate(nv::vec3f trans){
		_pos += trans;
		_init_pos = _pos;
	}

	void scale(float factor)
	{
		_pos *= factor;
		_init_pos = _pos;
	}

	void setVisible(bool v) { _bvis = v; }
	bool getVisible() { return _bvis; }
	void setLength(float v) { _length = v; }
	float getLength() { return _length; }

	void setXRes(int xres) { _xres = xres; }
	void setYRes(int yres) { _yres = yres; }
	void setNear(float v) { _neard = v; }
	void setFar(float v) { _fard = v; }
	void setFov(float v) { _fov = v; }
	void setRatio(float v) { _ratio = v; }
	void setFreq(int v) {_freq = v; }

	float getMTF() { return _mtf; }
	void setMTF(float v) {_mtf = v; }
	float getLED() { return _led; }
	void setLED(float v) {_led = v; }
	void getDOF(float &n, float &f, int &depth, int &dof) {
		n = _dn; f = _df; 
		depth = _bdepth, dof = _bdof;
	}
	void setDOF(float n, float f, int depth, int dof) {
		_dn = n; _df = f;
		_bdepth = depth; _bdof = dof;
	}

	RenderTexture *getFBO(int i) { return _fbos[i]; }
	void setFBO(int i, RenderTexture *fbo) { _fbos[i] = fbo; }

	void visualize();

	friend void transCameraList(const nv::matrix4f& mat);
	friend void updateCameraListAnm(ACTInfo* act, const nv::matrix4f& mat);
	friend void resetCameraListAnm();
};

//角度转焦距
inline	float Angle2Focus(float angle) {
	return FILM_DIAGONAL/2/tan(angle*RAD_UNIT/2);
}

//焦距转角度
inline	float Focus2Angle(float focus) {
	return atan(FILM_DIAGONAL/focus/2.0f)*2/RAD_UNIT;
}

void addCamera();
int getCameraNum();
void applyCamera(int i);
void removeCamera(int id);
void pushCamera();
void popCamera();
int getCameraID(int i);
void getCameraSize(int i, int &w, int &h);
char* getCameraName(int i);

void getCamera(int i, camera &);
void putCamera(int i, camera &);

void renderToBMP(int i, const char* fname);
void renderToTempBMP(int i);

int beginRecording(int i, const wchar_t* fname);
bool endRecording();
