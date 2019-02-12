// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.

#ifndef _MAIN_H_
#define _MAIN_H_

#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <windows.h>

#include <nvMath.h>

struct frustum
{
	float neard;
	float fard;
	float fov;
	float ratio;
	nv::vec3f point[8];
};

#define SCALE_TER_W 1.0f
#define SCALE_TER_H (SCALE_TER_W * 0.2f) 
//#define SCALE_MDL 0.02f

#ifdef FOR_CAR


#ifdef FOR_3DS_COMPARE
#define SCALE_MDL (SCALE_TER_W*0.1f)
#else
#define SCALE_MDL (SCALE_TER_W*1.f)
#endif

#else
#define SCALE_MDL (SCALE_TER_W*0.02f)
#endif

#define RADIUS 4.f

#define FAR_DIST (500.0f*SCALE_TER_W)
#define MAX_SPLITS 4
#define NUM_OBJECTS 4
#define LIGHT_FOV 45.0

class RenderTexture;

int initGL(int, int);
void initScene();
void display();
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle();
void keys(unsigned char c, int x, int y);
void reshape(int w, int h);
void menu(int m);
void updateKeys();
void camLook();
void cameraInverse(float dst[16], float src[16]);
nv::vec3f getCameraPostion();
GLuint createShaders(char* vert, char* frag);
void CheckFramebufferStatus();
void beginToonProgram();
void beginGoochProgram();

void beginProgram(bool tex_enable, bool bump_enable=false, bool grid_enable = false, float r = 1.f, float g = 1.f, float b = 0.f, float shiness=30.f);
void endProgram();
void reshapeExt(int w, int h, float angle);
void displayView(RenderTexture *);

//AnimationInfo* getAnmInfo();
void reloadAnimation();
bool updateAnimation(float dt);
void toggleAnimation();
bool isAnimating();
void togglePauseAnimation();
bool isAnimationPaused();
void resetAnimation();
void endAnimation();
void flushAnimation();

void setAnmFrameRate(int fps);

extern GLuint depth_tex_ar;;

extern float cam_pos[3];
extern float cam_view[3];
extern GLfloat light_dir[4];
extern int depth_size;
extern GLuint depth_tex_ar;

extern int width;
extern int height;
extern float fov;

extern int cur_num_splits;
extern bool show_depth_tex;
extern int shadow_type;
extern frustum f[MAX_SPLITS];

extern float split_weight;
extern float xgrid, ygrid;

void toggleShowFog();
void toggleShowWireframe();
void toggleShowCamera();
void toggleShowDepthMap();
void toggleShowWings();
void toggleShowFPS();
void toggleShowGrid();

bool getShowFog();
bool getShowWireframe();
bool getShowCamera();
bool getShowDepthMap();
bool getShowWings();
bool getShowFPS();
bool getShowGrid();

void computeShadowRatio(float &ratio, float &angle);

#endif