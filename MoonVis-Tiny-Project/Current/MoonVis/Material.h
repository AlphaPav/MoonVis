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
#include <stdio.h>	
#include <string>
#include <windows.h>        // Windows header file.
#include <GL/glew.h>
#include <gl/gl.h>          // OpenGl include.
#include <gl/glu.h>         // OpenGl utility include.
#include <TCHAR.h>

#define BITMAP_ID 0x4D42	// the universal bitmap ID

typedef unsigned long COLORREF;
typedef unsigned int  GLuint;

struct MatItem
{
	COLORREF diffuse;
	COLORREF ambitient;
	COLORREF specular;
	float	 shininess;
	char	 texPath[1024];
	char	 norPath[1024];
	char matName[50];
	GLuint	 textureID;
	GLuint	 normalMapID;
	unsigned char*	 textureData;
	unsigned char*	 normalMapData;
	//add by lzh 2015/3/27
	int smooth;
	int transparent;
	int reflection;
	int refraction;
	int radation;

	//end by lzh 2015/3/27
};

struct MatInfo
{
	COLORREF diffuse;
	COLORREF ambitient;
	COLORREF specular;
	float	 shininess;
	char	 texPath[1024];
	char	 norPath[1024];
	char matName[50];//此处我们默认名字不会超过50个字符。

	//add by lzh 2015/3/27
	int smooth;
	int transparent;
	int reflection;
	int refraction;
	int radation;

	//end by lzh 2015/3/27
};

class CMaterial
{
public:
	//binary save/load
	CMaterial();
	CMaterial(FILE *fp);
	CMaterial(MatInfo*, int);
	void Load(FILE *fp);
	void Save(FILE *fp);
	void GenerateTexture();                // Generate textures in OpenGL.
	void GenerateMaterialInfo();
	int GetMaterialNum();
	void GenerateTextureID();
	//add by lzh 2014/10/11
	bool isloadMat();//added by lzh in 2014.9.13
	void Load(FILE *fp, wchar_t *const path);
	void Save(FILE *fp, wchar_t *const path);
	//end add by lzh 2014/10/11

	int textureMode;	//0 for non texture, 1 for texture mapping, 2 for normal mapping
	GLuint* textureID;
	GLuint* normalMapID;
	MatInfo* matr;

	~CMaterial(void);

private:
	MatItem* _mat;							
	int		 num;							//材质个数	
	unsigned char *LoadBitmap(char *filename,
		BITMAPINFOHEADER *bitmapInfoHeader); // Load a bitmap image.
};

