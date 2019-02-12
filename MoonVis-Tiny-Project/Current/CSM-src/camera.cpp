// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.

#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_LEAN_AND_MEAN

#include "../MoonVis/stdafx.h"

#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <windows.h>

#include "camera.h"

#include <vector>
#include "main.h"

#include "../avi/AVIGenerator.h"
#include "../MoonVis/RenderTextureFBO.h"

#include <fstream>
#include <sstream>

#define BUF_SIZE 512
static char szTempPath[BUF_SIZE];

//=================================================
HBITMAP cameraBMP[512];
//=================================================

int cameraID = 0;
std::vector<camera> cameraList;

CAVIGenerator avi_gen;
int id_recording = -1;

#define DOF_NEAR  7.f
#define DOF_FAR    10.f

camera::camera()
: _mdl_id(-1)
{
	_neard = _fard = _fov = _ratio = 0.f;
	_xres = _yres = 0;
	_freq = 30;
	_id = cameraID+1;
	_name[0] = 0;
	_fbos[0] = _fbos[1] = _fbos[2] = NULL;
	_mtf = 1.f;
	_led = 0.f;

	_dn = DOF_NEAR;
	_df = DOF_FAR;
	_bdof = false;
	_bdepth = false;

	_bvis = false;
	_length = 1.f;
}

camera::camera(nv::vec3f pos, nv::vec3f view)
: _mdl_id(-1)
{
	_init_pos = _pos = pos;
	_init_view = _view = view;
	
	nv::vec3f tmp;
	float ret = dot(view, nv::vec3f(0.f, 1.f, 0.f));
	if (std::abs(ret) == 1.f)
	{
		tmp = cross(view,  nv::vec3f(1.f, 0.f, 0.f));
	}
	else
	{
		tmp = cross(view,  nv::vec3f(0.f, 1.f, 0.f));
	}

	_up = cross(tmp, view);
	_init_up = _up = nv::normalize(_up);

	_neard = 0.001f;
	_fard = FAR_DIST;
	_fov = 74 / 57.2957795f; // 45.0 / 57.2957795f;
	_ratio = (double)width/(double)height;

	_xres = 1280;// 640;
	_yres = 1024;// 360;
	_freq = 30;
	_id = cameraID+1;
	_fbos[0] = _fbos[1] = _fbos[2] = NULL;
	_mtf = 1.f;
	_led = 0.f;

	_dn = DOF_NEAR;
	_df = DOF_FAR;
	_bdof = false;
	_bdepth = false;

	_bvis = false;
	_length = 1.f;
}

camera::camera(
			   float pos[3], float view[3],
			   float neard, float fard,
			   float fov, float ratio, 
			   int xres, int yres, 
			   int freq, float mtf, float led
			   )
			   : _pos(pos), _view(view),
			   _init_pos(pos), _init_view(view),
			   _neard(neard), _fard(fard),
			   _fov(fov), _ratio(ratio),
			   _xres(xres), _yres(yres),
			   _freq(freq),
			   _id(cameraID+1),
			   _mdl_id(-1),
			   _mtf(mtf),
			   _led(led)
{
	nv::vec3f tmp = cross(_view, nv::vec3f(0.f, 1.f, 0.f));
	_up = cross(tmp, _view);
	_init_up = _up = nv::normalize(_up);
	_fbos[0] = _fbos[1] = _fbos[2] = NULL;

	_dn = DOF_NEAR;
	_df = DOF_FAR;
	_bdof = false;
	_bdepth = false;

	_bvis = false;
	_length = 1.f;
}


static nv::vec3f  s_pos, s_view;

void pushCamera()
{
	s_pos = cam_pos;
	s_view = cam_view;
}

void popCamera()
{
	cam_pos[0] = s_pos.x, cam_pos[1] = s_pos.y, cam_pos[2] = s_pos.z;
	cam_view[0] = s_view.x, cam_view[1] = s_view.y, cam_view[2] = s_view.z;
}

extern int cameraID;
void addCamera()
{
	cameraList.push_back(camera(nv::vec3f(cam_pos), nv::vec3f(cam_view)));
	++cameraID;
	CString name_cstr;
	name_cstr.Format(_T("Па»ъ-%d"), cameraID-1);
	char *name = NULL;
	WideCharToMultiByte_M(name_cstr, &name);
	cameraList[cameraID-1].setName(name);
	delete[] name;
}

int getCameraNum()
{
	return cameraList.size();
}

int getCameraID(int i)
{
	return cameraList[i].getID();
}

void getCameraSize(int i, int &w, int &h)
{
	w = cameraList[i].getXRes();
	h = cameraList[i].getYRes();
}

void applyCamera(int i)
{
	 cameraList[i].getPos(cam_pos);
	 cameraList[i].getView(cam_view);
}

void removeCamera(int id)
{
	for (std::vector<camera>::iterator it=cameraList.begin();
		it != cameraList.end(); it++)
	{
		if ((*it).getID() == id) {
			cameraList.erase(it);
			return;
		}
	}
}

char* getCameraName(int i)
{
	return cameraList[i].getName();
}

void getCamera(int i, camera &c)
{
	c = cameraList[i];
}

void putCamera(int i, camera & c)
{
	cameraList[i] = c;
}

GLbyte *SaveAsBinary(int i, GLuint fb) 
{ 
	GLint viewPort[4]; 
	glGetIntegerv(GL_VIEWPORT,viewPort);

	unsigned long imageSize=((viewPort[2]+((4-(viewPort[2]%4))%4))*viewPort[3]*3)+2; 
	GLbyte *rgbData=(GLbyte*)malloc(imageSize); 

	glPixelStorei(GL_PACK_ALIGNMENT,4); 
	glPixelStorei(GL_PACK_ROW_LENGTH,0); 
	glPixelStorei(GL_PACK_SKIP_ROWS,0); 
	glPixelStorei(GL_PACK_SKIP_PIXELS,0); 
	glPixelStorei(GL_PACK_SWAP_BYTES,1); 

	GLenum lastBuffer; 
	glGetIntegerv(GL_READ_BUFFER,(GLint*)&lastBuffer); 

	glReadBuffer(fb); 
	glReadPixels(0,0,viewPort[2],viewPort[3],GL_BGR,GL_UNSIGNED_BYTE,rgbData); 
	rgbData[imageSize-1]=0; 
	rgbData[imageSize-2]=0; 

	glReadBuffer(lastBuffer); 

	return rgbData;
} 

void SaveAsBMP(int i, GLuint fb, const char *fileName) 
{ 
	unsigned long imageSize; 
	GLbyte *data=NULL; 
	GLint viewPort[4]; 
	GLenum lastBuffer; 
	BITMAPFILEHEADER bmfh; 
	BITMAPINFOHEADER bmih; 
	bmfh.bfType='MB'; 
	bmfh.bfReserved1=0; 
	bmfh.bfReserved2=0; 
	bmfh.bfOffBits=54; 
	glGetIntegerv(GL_VIEWPORT,viewPort); 
	imageSize=((viewPort[2]+((4-(viewPort[2]%4))%4))*viewPort[3]*3)+2; 
	bmfh.bfSize=imageSize+sizeof(bmfh)+sizeof(bmih); 
	data=(GLbyte*)malloc(imageSize); 
	glPixelStorei(GL_PACK_ALIGNMENT,4); 
	glPixelStorei(GL_PACK_ROW_LENGTH,0); 
	glPixelStorei(GL_PACK_SKIP_ROWS,0); 
	glPixelStorei(GL_PACK_SKIP_PIXELS,0); 
	glPixelStorei(GL_PACK_SWAP_BYTES,1); 
	glGetIntegerv(GL_READ_BUFFER,(GLint*)&lastBuffer); 

	glReadBuffer(fb); 
	glReadPixels(0,0,viewPort[2],viewPort[3],GL_BGR,GL_UNSIGNED_BYTE,data); 
	data[imageSize-1]=0; 
	data[imageSize-2]=0; 

	if (i == id_recording)
	{
		avi_gen.AddFrame((BYTE*)data);
	}

	glReadBuffer(lastBuffer); 
	bmih.biSize=40; 
	bmih.biWidth=viewPort[2]; 
	bmih.biHeight=viewPort[3]; 
	bmih.biPlanes=1; 
	bmih.biBitCount=24; 
	bmih.biCompression=0; 
	bmih.biSizeImage=imageSize; 
	bmih.biXPelsPerMeter=45089; 
	bmih.biYPelsPerMeter=45089; 
	bmih.biClrUsed=0; 
	bmih.biClrImportant=0; 

	if (fileName) {
		if (0) {
			FILE *file = fopen(fileName, "wb");
			fwrite(&bmfh, sizeof(bmfh), 1, file);
			fwrite(&bmih, sizeof(bmih), 1, file);
			fwrite(data, imageSize, 1, file);
			fclose(file);
		}
		else {
			std::ofstream ofs(fileName, ios::binary);
			ofs.write((char *)&bmfh, sizeof(bmfh));
			ofs.write((char *)&bmih, sizeof(bmih));
			ofs.write((char *)data, imageSize);
			ofs.close();
		}
	}
	else {
		if (cameraBMP[i] != NULL)
			::DeleteObject(cameraBMP[i]);


#if 0
		cameraBMP[i] = CreateBitmap(viewPort[2], viewPort[3], 1, 24, data);
#else
		GetTempPathA(BUF_SIZE, szTempPath);
		char path[512];
		sprintf(path, "%s//preview%02d.bmp", szTempPath, i);
		GetTempFileNameA(szTempPath, "BMP", 0, path);

		FILE *file = fopen(path, "wb");
		fwrite(&bmfh, sizeof(bmfh), 1, file);
		fwrite(&bmih, sizeof(bmih), 1, file);
		fwrite(data, imageSize, 1, file);
		fclose(file);

		cameraBMP[i] = (HBITMAP)::LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
#endif
		assert(cameraBMP[i] != NULL);
	}

	free(data); 
} 

void getFBOs(int i, int w, int h, 
	RenderTexture *o1, RenderTexture *o2, RenderTexture *o3,
	RenderTexture *&n1, RenderTexture *&n2, RenderTexture *&n3)
{
	if (o1 == NULL ||  
		o1->GetWidth() != w || o1->GetHeight() != h) {
		
		if (o1) delete o1;
		if (o2) delete o2;
		if (o3) delete o3;

		n1 = new RenderTexture(w, h, GL_TEXTURE_2D);
		n1->InitColor_Tex(0, GL_RGB16F_ARB);
		n1->InitColor_Tex(1, GL_RGB16F_ARB);
		n1->InitColor_Tex(2, GL_RGB16F_ARB);
		n1->InitDepth_Tex();

		n2 = new RenderTexture(w, h, GL_TEXTURE_2D);
		n2->InitColor_Tex(0, GL_RGB16F_ARB);

		n3 = new RenderTexture(w, h, GL_TEXTURE_2D);
		n3->InitColor_Tex(0, GL_RGB16F_ARB);
	} else {
		n1 = o1;
		n2 = o2;
		n3 = o3;
	}
}

static int w=0, h=0;
extern bool show_camera;
extern GLuint mtf_prog;
extern GLuint dof_prog;
extern GLuint ttex;

static void DrawQuad(int cx, int cy)
{
	glBegin(GL_QUADS);
	glColor3f(1.f, 1.f, 1.f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);

	glColor3f(1.f, 0.f, 1.f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(cx, 0.0f);

	glColor3f(1.f, 1.f, 0.f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(cx, cy);

	glColor3f(1.f, 0.f, 0.f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0.f, cy);
	glEnd();
}

void renderToBMP(int i, const char* fname)
{
	pushCamera();

	int w=width, h=height;

	camera &c = cameraList[i];
	int cx = c.getXRes(), cy = c.getYRes();
	reshapeExt(cx, cy, c.getRectFov()*57.2957795f);
	applyCamera(i);

	RenderTexture *fbo1=NULL, *fbo2=NULL, *fbo3=NULL;
	getFBOs(i, cx, cy, c.getFBO(0), c.getFBO(1), c.getFBO(2), fbo1, fbo2, fbo3);

	c.setFBO(0, fbo1);
	c.setFBO(1, fbo2);
	c.setFBO(2, fbo3);

#if 1
	bool _show_camera = show_camera;
	show_camera = false;
	displayView(fbo1);
	show_camera = _show_camera;

	fbo2->Activate();

	glxBegin2D(cx, cy);
	
	glActiveTexture(GL_TEXTURE0);
	fbo1->Bind(0);

	glActiveTexture(GL_TEXTURE1);
	fbo1->Bind(1);

	glActiveTexture(GL_TEXTURE2);
	fbo1->Bind(2);

	glUseProgram(mtf_prog);
	glUniform1i(glGetUniformLocation(mtf_prog,"ctex"), 0);
	glUniform1i(glGetUniformLocation(mtf_prog,"dtex"), 1);
	glUniform1i(glGetUniformLocation(mtf_prog,"ntex"), 2);

	glUniform1f(glGetUniformLocation(mtf_prog, "mtf"), c.getMTF()); // mtf value
	glUniform1f(glGetUniformLocation(mtf_prog, "led"), c.getLED()); // led value

	DrawQuad(cx, cy);

	glUseProgram(0);
	fbo2->Deactivate();

	fbo3->Activate();

	glActiveTexture(GL_TEXTURE0);
	fbo2->Bind(0);

	glActiveTexture(GL_TEXTURE1);
	fbo1->Bind(1);

	glActiveTexture(GL_TEXTURE2);
	fbo1->Bind(2);

	glUseProgram(dof_prog);
	glUniform1i(glGetUniformLocation(dof_prog,"ctex"), 0);
	glUniform1i(glGetUniformLocation(dof_prog,"dtex"), 1);
	glUniform1i(glGetUniformLocation(dof_prog,"ntex"), 2);
	glUniform2f(glGetUniformLocation(dof_prog, "offset"), 1.f/cx, 1.f/cy);

	float dfar, dnear;
	int bdepth, bdof;
	c.getDOF(dnear, dfar, bdepth, bdof);

	glUniform1f(glGetUniformLocation(dof_prog, "focusDistance"), dnear); // led value
	glUniform1f(glGetUniformLocation(dof_prog, "focusLength"), dfar-dnear); // led value
	glUniform1f(glGetUniformLocation(dof_prog, "blurIntensity"), 0.75f); // led value
	glUniform1i(glGetUniformLocation(dof_prog, "visDOF"), bdof); // enable DOF ?
	glUniform1i(glGetUniformLocation(dof_prog, "visDepth"), bdepth); // enable Depth?

	DrawQuad(cx, cy);

	SaveAsBMP(i, GL_BACK, fname);
	glxEnd2D();
	glUseProgram(0);
	fbo3->Deactivate();
	reshape(w, h);
	popCamera();
#else
	
	fbo2->Activate();
	bool _show_camera = show_camera;
	show_camera = false;
	displayView(fbo1);
	show_camera = _show_camera;
	fbo2->Deactivate();
	//reshape(w, h);
	popCamera();
#endif
}

void renderToTempBMP(int i)
{
	renderToBMP(i, NULL);
}

int beginRecording(int i, const wchar_t* fname)
{
	if (-1 != id_recording)
	{
		return false;
	}

	camera &cam = cameraList[i];

	// set fps
	avi_gen.SetRate(cam.getFreq());

	// give info about bitmap
	avi_gen.SetBitmapHeader(cam.getXRes(), cam.getYRes());		

	// set filename, extension ".avi" is appended if necessary
	avi_gen.SetFileName(fname);

	HRESULT hr=avi_gen.InitEngine();
	if (!FAILED(hr))
	{
		id_recording = i;
		setAnmFrameRate(cam.getFreq());
	}
	return id_recording;
}

bool endRecording()
{
	avi_gen.ReleaseEngine();
	id_recording = -1;
	setAnmFrameRate(0);
	return true;
}

void transCameraList(const nv::matrix4f& mat)
{
	for (size_t i = 0; i < cameraList.size(); i++)
	{
		camera &cam = cameraList[i];

		nv::matrix4f mat_v = mat;

		nv::vec4f v4f = mat_v * nv::vec4f(0.f, 0.f, 0.f, 1.f);
		nv::vec3f origin = transHomoToNon(v4f);

		v4f = mat_v * nv::vec4f(cam._init_pos, 1.f);
		cam._init_pos = cam._pos = transHomoToNon(v4f);

		v4f = mat_v * nv::vec4f(cam._init_view, 1.f);
		cam._view = transHomoToNon(v4f);
		cam._view -= origin;
		cam._init_view = cam._view = nv::normalize(cam._view);

		v4f = mat_v * nv::vec4f(cam._init_up, 1.f);
		cam._up = transHomoToNon(v4f);
		cam._up -= origin;
		cam._init_up = cam._up = nv::normalize(cam._up);
	}
}

void updateCameraListAnm(ACTInfo* act, const nv::matrix4f& mat)
{
	for (size_t i = 0; i < cameraList.size(); i++)
	{
		camera &cam = cameraList[i];

		if (-1 == cam._mdl_id)
		{
			continue;
		}

		matrix4f mat_t = act->GetModelPtr(cam._mdl_id)->GetTransf();
		nv::matrix4f mat_v;
		mat_v.set_value(const_cast<float*>(mat_t.get_value())); //-- use const_cast deal to unreasonable design in nv::matrix
		mat_v = inverse(mat) * mat_v * mat;

		nv::vec4f v4f = mat_v * nv::vec4f(0.f, 0.f, 0.f, 1.f);
		nv::vec3f origin = transHomoToNon(v4f);

		v4f = mat_v * nv::vec4f(cam._init_pos, 1.f);
		cam._pos = transHomoToNon(v4f);

		v4f = mat_v * nv::vec4f(cam._init_view, 1.f);
		cam._view = transHomoToNon(v4f);
		cam._view -= origin;
		cam._view = nv::normalize(cam._view);

		v4f = mat_v * nv::vec4f(cam._init_up, 1.f);
		cam._up = transHomoToNon(v4f);
		cam._up -= origin;
		cam._up = nv::normalize(cam._up);
	}
}

void resetCameraListAnm()
{
	for (size_t i = 0; i < cameraList.size(); i++)
	{
		camera &cam = cameraList[i];
		cam._pos = cam._init_pos;
		cam._view = cam._init_view;
		cam._up = cam._init_up;
	}
}
