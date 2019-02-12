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

typedef GLubyte	RGB_ByteColor[4];
class vec3f;
struct MatInfo;

#define ByteToRGB(Bytecolor, colorRef)	(colorRef = RGB(Bytecolor[0], Bytecolor[1], Bytecolor[2]))
#define RGBtoByte(colorRef, ByteColor)	{														\
											ByteColor[0] = (GLubyte)(colorRef&0xff);			\
											ByteColor[1] = (GLubyte)((colorRef>>8)&0xff);		\
											ByteColor[2] = (GLubyte)((colorRef>>16)&0xff);		\
											ByteColor[3] = (GLubyte)((colorRef>>24)&0xff);		\
										}

extern void glxEnableMaterial();
extern void glxSetMaterial(float r, float g, float b, float a);

extern GLboolean	glxEnableLighting(GLboolean bEnable = TRUE);
extern GLboolean	glxEnableDepthTest(GLboolean bEnable = TRUE);

extern COLORREF glxSetColor(COLORREF color);

extern float glxSetWidth(float width);
extern float glxSetPointSize(float size);

extern void glxBegin2D(int win_w, int win_h);
extern void glxEnd2D();
extern void glxRenderText( float x, float y, void *glutFont, char *string );
extern void glxSecionPlaneWithLines(vec3f pos[]);

extern void glxSetMat(MatInfo *, GLuint *, GLuint *, unsigned long);
extern void glxEnableMat();
extern void glxSetLoc(int x);
extern int  glxGetLoc();


extern void tm_print(char *msg);
#ifdef _DEBUG
#if 0
#define GET_GLERROR()                                          \
{                                                                 \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR) {                                     \
    fprintf(stderr, "[file %s, line %d] GL Error: %s\n",                \
    __FILE__, __LINE__, gluErrorString(err));                     \
    fflush(stderr);                                               \
    }                                                             \
}
#endif

#define GET_GLERROR                                          \
{                                                                 \
	char outstring[1024];\
	GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR) {                                     \
    sprintf(outstring, "[file %s, line %d] GL Error: %s\n",                \
    __FILE__, __LINE__, gluErrorString(err));                     \
    tm_print(outstring);                                           \
    }                                                             \
}
#else
#define GET_GLERROR
#endif
