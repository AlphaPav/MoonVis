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
#include "stdafx.h"
#include <windows.h>//#include <WinDef.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "glx.h"
#include "Material.h"

static float    ambient[4]={0.0,0.0,0.5,1.0};
static float	diffuse[4] = {0.0, 0.0, 0.5, 1.0};
static float    specular[4] = {1.0, 1.0, 1.0, 1.0};
static float    shininess[] = {0.0};
static int		location = 0;

void glxInit()
{
	glewInit();
}

void  glxSetDiffuse(float r, float g, float b, float a)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = a;
}

void glxSetAmbitient(float r, float g, float b, float a)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
	ambient[3] = a;
}

void glxSetSpecular(float r, float g, float b, float a)
{
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
	specular[3] = a;
}

void glxSetShininess(float a)
{
	shininess[0] = a;
}

void  glxSetMaterial(float r, float g, float b, float a)
{
	 ambient[0] = r;
	 ambient[1] = g;
	 ambient[2] = b;
	 ambient[3] = a;
}

void	glxEnableMaterial()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}

GLboolean glxEnableLighting(GLboolean bEnable)
{
	GLboolean bLighting;
	glGetBooleanv(GL_LIGHTING, &bLighting);
	(bEnable? glEnable : glDisable)(GL_LIGHTING);
	return bLighting;
}

GLboolean glxEnableDepthTest(GLboolean bEnable)
{
	GLboolean bDepthOld;
	glGetBooleanv(GL_DEPTH_TEST, &bDepthOld);
	(bEnable? glEnable : glDisable)(GL_DEPTH_TEST);
	return bDepthOld;
}

COLORREF glxSetColor(COLORREF color)
{
	RGB_ByteColor ByteColor;
	RGBtoByte(color, ByteColor);

	double colorOld[4];
	memset(colorOld,0,4*sizeof(double));
	glGetDoublev(GL_CURRENT_COLOR, colorOld);

	glColor4ubv(ByteColor);

	return RGB(colorOld[0]*255, colorOld[1]*255, colorOld[2]*255 ) | ((int)(colorOld[2]*255))<<24 ;
}

float glxSetWidth(float width)
{
	GLfloat	rWidthOld;
	glGetFloatv(GL_LINE_WIDTH, &rWidthOld);

	glLineWidth(width);

	return rWidthOld;
}

float glxSetPointSize(float size)
{
	GLfloat	rSizeOld;
	glGetFloatv(GL_POINT_SIZE, &rSizeOld);

	glPointSize(size);

	return rSizeOld;
}

void
glxBegin2D(int win_w, int win_h)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glViewport(0, 0, win_w, win_h);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, win_w, 0.0, win_h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
}

void
glxEnd2D()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

void glxRenderText( float x, float y, void *glutFont, char *string )
{
	char *c = '\0';
	glRasterPos2f( x, y );

	for( c = string; *c != '\0'; ++c )
		glutBitmapCharacter( glutFont, *c );
}

#include "vec3f.h"

int max(double len1, double len2) {
	if (len1 > len2)
		return len1;
	else
		return len2;
}

void glxSecionPlaneWithLines(vec3f pos[])
{
	int i;

	glBegin(GL_LINE_LOOP);
	for(i=0; i<4; i++)
	{
		glVertex3fv(pos[i].v);
	}
	glEnd();

	vec3f vec[4];
	for(int j = 0; j<3; j++)
	{
		vec[j] = pos[j+1] - pos[j];
	}
	vec[3] = pos[0] - pos[3];

	double len1 = vec[0].length();
	double len2 = vec[1].length();
//	len1 *=2;
//	len2 *=2;
	int numCut = max(len1, len2); ///设置剖面线数目
	float ratio = 1.f/numCut;

	int k;
	for( k= 0; k<4; k++)
	{
		vec[k] = vec[k] * ratio;
	}

	/************vec[0]**************
	*   pos0-----pos11---->pos1
	*	 ^					|
	*	 |					|
	*	pos12			   pos21    vec[1]
	*	 |					|
	*    |					v			
	*   pos3<----pos22-----pos2
	*************vec[2]***************/

	vec3f pos11, pos12;
	vec3f pos21, pos22;

	float width = glxSetWidth(1.0);
	GLboolean bEnableDepth = glxEnableDepthTest(TRUE);
	glBegin(GL_LINES);
	for(k= 0; k< numCut; k++)
	{
		pos11 = pos[0] + vec[0]*k;
		pos12 = pos[0] - vec[3]*k;
		glVertex3fv(pos11.v);
		glVertex3fv(pos12.v);

		pos21 = pos[1] +vec[1]*k;
		pos22 = pos[3] - vec[2]*k;
		glVertex3fv(pos21.v);
		glVertex3fv(pos22.v);
	}
	glEnd();

	glxSetWidth(width);
	glxEnableDepthTest(bEnableDepth);
}


void glxSetMat(MatInfo *matr, GLuint *textureID, GLuint *normalID, unsigned long color)
{
	if (textureID != NULL) {
		//TangMin: useless, will be done by shader
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, normalID[color-1]);

		//TangMin: texutre0 has been used for shadow map?
		//TangMin： different from FViewer
		//glActiveTexture(GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID[color-1]);
	}

	glxSetAmbitient(GetRValue(matr[color-1].ambitient) / 255.f,
		GetGValue(matr[color-1].ambitient) / 255.f,
		GetBValue(matr[color-1].ambitient) / 255.f,
		0);
	glxSetDiffuse(GetRValue(matr[color-1].diffuse) / 255.f,
		GetGValue(matr[color-1].diffuse) / 255.f,
		GetBValue(matr[color-1].diffuse) / 255.f,
		0);
	glxSetSpecular(GetRValue(matr[color-1].specular) / 255.f,
		GetGValue(matr[color-1].specular) / 255.f,
		GetBValue(matr[color-1].specular) / 255.f,
		0);
	glxSetShininess(matr[color-1].shininess);
}

void glxEnableMat()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	//====
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	GET_GLERROR
}