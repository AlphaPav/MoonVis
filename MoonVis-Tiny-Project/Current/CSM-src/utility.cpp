// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.

#include <cmath>
#define NV_REPORT_COMPILE_ERRORS
#include <nvShaderUtils.h>
#include "main.h"
#include "terrain.h"

float cam_pos[3] = {-120.706*SCALE_TER_W, 32.4241*SCALE_TER_W, -114.225*SCALE_TER_W};
float cam_view[3] = {0.976459, -0.15252, -0.152531};

//-- 36, 15 degree
GLfloat light_dir[4] = {-0.62831852f, 0.2617993875f, 1.0f , 0.0f};

const float sensitivity = 0.005;
const float walk_speed = 0.5 * SCALE_TER_W;

static int old_x, old_y;
int half_width;
int half_height;

extern bool g_is_use_aggregating;

void normalize(float *v)
{
	float magnitude = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= magnitude;
	v[1] /= magnitude;
	v[2] /= magnitude;
}

void rotate_view(float *view, float angle, float x, float y, float z)
{
	float new_x;
	float new_y;
	float new_z;

	float c = cos(angle);
	float s = sin(angle);

	new_x  = (x*x*(1-c) + c)	* view[0];
	new_x += (x*y*(1-c) - z*s)	* view[1];
	new_x += (x*z*(1-c) + y*s)	* view[2];
	
	new_y  = (y*x*(1-c) + z*s)	* view[0];
	new_y += (y*y*(1-c) + c)	* view[1];
	new_y += (y*z*(1-c) - x*s)	* view[2];

	new_z  = (x*z*(1-c) - y*s)	* view[0];
	new_z += (y*z*(1-c) + x*s)	* view[1];
	new_z += (z*z*(1-c) + c)	* view[2];

	view[0] = new_x;
	view[1] = new_y;
	view[2] = new_z;

	normalize(view);
}

void motion(int x, int y)
{
	float rot_x, rot_y;
	float rot_axis[3];

	x -= half_width;
	y -= half_height;

	rot_x = -(float)(x - old_x) * sensitivity;
	rot_y = -(float)(y - old_y) * sensitivity;

	old_x = x;
	old_y = y;

	if(GetAsyncKeyState(VK_SHIFT))
	{
		light_dir[1] += rot_y;
		if(light_dir[1] < 0.2f)
			light_dir[1] = 0.2f;
		normalize(light_dir);
		rotate_view(light_dir, -rot_x, cam_view[0], cam_view[1], cam_view[2]);
	}else
	{
		rotate_view(cam_view, rot_x, 0.0f, 1.0f, 0.0f);

		rot_axis[0] = -cam_view[2];
		rot_axis[1] = 0.0f;
		rot_axis[2] = cam_view[0];

		normalize(rot_axis);

		rotate_view(cam_view, rot_y, rot_axis[0], rot_axis[1], rot_axis[2]);
	}

}

#ifndef FLT_MIN
#define FLT_MIN (1.0e-10F)
#endif

inline vec3f getUp(vec3f &view)
{
	float ret = view.dot(vec3f(0, 1, 0));
	if (std::abs(std::abs(ret)-1.f) < FLT_MIN)
	{
		return vec3f(1.f, 0.f, 0.f);
	}
	else
	{
		return vec3f(0.f, 1.f, 0.f);
	}
}

void camLook()
{
	vec3f view = vec3f(cam_view);
	vec3f up = getUp(view);

	gluLookAt(
		cam_pos[0], cam_pos[1], cam_pos[2],
		cam_pos[0] + cam_view[0], cam_pos[1] + cam_view[1], cam_pos[2] + cam_view[2],
		up.x, up.y, up.z);
}

void updateKeys()
{
	if(GetAsyncKeyState('W')){
		cam_pos[0] += cam_view[0] * walk_speed;
		cam_pos[1] += cam_view[1] * walk_speed;
		cam_pos[2] += cam_view[2] * walk_speed;
	}
	if(GetAsyncKeyState('S')){
		cam_pos[0] -= cam_view[0] * walk_speed;
		cam_pos[1] -= cam_view[1] * walk_speed;
		cam_pos[2] -= cam_view[2] * walk_speed;
	}
	if(GetAsyncKeyState('A')){
		cam_pos[0] += cam_view[2] * walk_speed;
			
		cam_pos[2] -= cam_view[0] * walk_speed;
	}
	if(GetAsyncKeyState('D')){
		cam_pos[0] -= cam_view[2] * walk_speed;
			
		cam_pos[2] += cam_view[0] * walk_speed;
	}

	if(GetAsyncKeyState(VK_SPACE)){
		cam_pos[1] += walk_speed;
	}
}

void mouse(int button, int state, int x, int y)
{
	old_x = x - half_width;
	old_y = y - half_height;
    //glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}

void keys(unsigned char c, int x, int y)
{
	switch (c) {
/*		case 27: //escape key
		case 'q':
		case 'Q':
			exit(0);
			break;*/
		case '1':
			cur_num_splits = 1;
			break;
		case '2':
			cur_num_splits = 2;
			break;
		case '3':
			cur_num_splits = 3;
			break;
		case '4':
			cur_num_splits = 4;
			break;
		case 192: //'`'
			toggleShowDepthMap();
			break;

		case '0':
			shadow_type--;
			if (shadow_type < 0)
				shadow_type = 8;
			break;

		case 'o':
		case 'O':
			g_is_use_aggregating = !g_is_use_aggregating;
			break;

		case 'p':
		case 'P':
			printf("cam pos: %f %f %f\n", cam_pos[0], cam_pos[1], cam_pos[2]);
			break;
		case '+':
			split_weight += 0.05f;
			printf( "split_weight changed to %f\n", split_weight);
			break;
		case '-':
			split_weight -= 0.05f;
			printf( "split_weight changed to %f\n", split_weight);
			break;
	}

/*
	if (GetAsyncKeyState('q') || GetAsyncKeyState('Q'))
	{
		exit(0);
	}
	else if (GetAsyncKeyState('1'))
	{
		cur_num_splits = 1;
	}
	else if (GetAsyncKeyState('2'))
	{
		cur_num_splits = 2;
	}
	else if (GetAsyncKeyState('3'))
	{
		cur_num_splits = 3;
	}
	else if (GetAsyncKeyState('4'))
	{
		cur_num_splits = 4;
	}
	else if (GetAsyncKeyState('`'))
	{
		show_depth_tex = !show_depth_tex;
	}
	else if (GetAsyncKeyState('0'))
	{
		shadow_type--;
		if (shadow_type < 0)
			shadow_type = 8;
	}
	else if (GetAsyncKeyState('p') || GetAsyncKeyState('P'))
	{
		printf("cam pos: %f %f %f\n", cam_pos[0], cam_pos[1], cam_pos[2]);
	}
	else if (GetAsyncKeyState('+'))
	{
		split_weight += 0.05f;
		printf( "split_weight changed to %f\n", split_weight);
	}
	else if (GetAsyncKeyState('-'))
	{
		split_weight -= 0.05f;
		printf( "split_weight changed to %f\n", split_weight);
	}
	*/
}

void reshapeExt(int w, int h, float angle)
{
	width = w;
	height = h;
	fov = angle;

	half_height = height/2;
	half_width = width/2;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle, (double)width/(double)height, 0.1f*SCALE_TER_W, FAR_DIST);

	glViewport(0, 0, width, height);
	for(int i=0; i<MAX_SPLITS; i++)
	{
		f[i].fov = fov/57.2957795f;
		f[i].ratio = (double)width/(double)height;
	}
}

void reshape(int w, int h)
{
	reshapeExt(w, h, 45.f);
}

void regenerateDepthTex(GLuint depth_size)
{
	glDeleteTextures(1, &depth_tex_ar);
	glGenTextures(1, &depth_tex_ar);

	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, depth_tex_ar);
	glTexImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, GL_DEPTH_COMPONENT24, depth_size, depth_size, MAX_SPLITS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}

void menu(int m)
{
	switch (m) {
		case 'q':
			exit(0);
			break;
		case '1':
			cur_num_splits = 1;
			break;
		case '2':
			cur_num_splits = 2;
			break;
		case '3':
			cur_num_splits = 3;
			break;
		case '4':
			cur_num_splits = 4;
			break;
		case '`':
			toggleShowDepthMap();
			break;
		case 0:
			shadow_type = 0;
			break;
		case 1:
			shadow_type = 1;
			break;
		case 2:
			shadow_type = 2;
			break;
		case 3:
			shadow_type = 3;
			break;
        case 4:
			shadow_type = 4;
			break;
        case 5:
			shadow_type = 5;
			break;
        case 6:
			shadow_type = 6;
			break;
        case 7:
			shadow_type = 7;
			break;
        case 8:
			shadow_type = 8;
			break;
		case 10:
			depth_size = 512;
			regenerateDepthTex(512);
			break;
		case 11:
			depth_size = 1024;
			regenerateDepthTex(1024);
			break;
		case 12:
			depth_size = 2048;
			regenerateDepthTex(2048);
			break;
		case 13:
			depth_size = 4096;
			regenerateDepthTex(4096);
			break;
	}
	glutPostRedisplay();
}

void cameraInverse(float dst[16], float src[16])
{
	dst[0] = src[0];
	dst[1] = src[4];
	dst[2] = src[8];
	dst[3] = 0.0f;
	dst[4] = src[1];
	dst[5] = src[5];
	dst[6]  = src[9];
	dst[7] = 0.0f;
	dst[8] = src[2];
	dst[9] = src[6];
	dst[10] = src[10];
	dst[11] = 0.0f;
	dst[12] = -(src[12] * src[0]) - (src[13] * src[1]) - (src[14] * src[2]);
	dst[13] = -(src[12] * src[4]) - (src[13] * src[5]) - (src[14] * src[6]);
	dst[14] = -(src[12] * src[8]) - (src[13] * src[9]) - (src[14] * src[10]);
	dst[15] = 1.0f;
}

nv::vec3f getCameraPostion()
{
	return nv::vec3f(cam_pos[0], cam_pos[1], cam_pos[2]);
}

GLuint createShaders(char* vert, char* frag)
{
	GLuint v, f;

    if(! (v = nv::CompileGLSLShaderFromFile(GL_VERTEX_SHADER, vert)))
        v = nv::CompileGLSLShaderFromFile(GL_VERTEX_SHADER, &vert[3]); //skip the first three chars to deal with path differences

    if(! (f = nv::CompileGLSLShaderFromFile(GL_FRAGMENT_SHADER, frag)))
        f = nv::CompileGLSLShaderFromFile(GL_FRAGMENT_SHADER, &frag[3]); //skip the first three chars to deal with path differences

	return nv::LinkGLSLProgram(v, f);
}

void CheckFramebufferStatus()
{
    int status;
    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            printf("Unsupported framebuffer format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, missing attachment\n");
            break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, incomplete attachment\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Framebuffer incomplete, attached images must have same dimensions\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Framebuffer incomplete, attached images must have same format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Framebuffer incomplete, missing draw buffer\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Framebuffer incomplete, missing read buffer\n");
            break;
        default:
            exit(0);
    }
}
