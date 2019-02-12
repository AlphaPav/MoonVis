// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.

#include "../MoonVis/stdafx.h"
#include <nvImage.h>
#include <fstream>
#include "terrain.h"
#include "camera.h"
#include "../MoonVis/Animation.h" 
# pragma warning(disable : 4996) 


#ifndef M_PI
	#define M_PI 3.14159265f
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


char* depth_tex_filename;
char* inf_filename;
wchar_t* explorer_filename;
wchar_t* explorer_animation_filename;

bool isLoaded;

extern std::vector<camera> cameraList;

std::vector<bool> vec_is_vis;

extern void set_pt2(nv::vec3f& dst, nv::vec3f& src);

Terrain::Terrain()
: tex(0)
, heights(NULL)
, normals(NULL)
, actExp(NULL)
, bAnimation(false)
, bPauseAnimation(false)
, fpsAnm(0)
, iAnmType(-1)
, anms(NULL)
{
	TailFlame::InitData("../../media/textures/SMOKE.png");
}

Terrain::~Terrain()
{
	if(tex)
		glDeleteTextures(1, &tex);
	if(heights)
		delete [] heights;
	if(normals)
		delete [] normals;

	for(unsigned int i=0; i<entities.size(); i++)
		delete entities[i];

	if (glIsList(terrain_list))
	{
		glDeleteLists(terrain_list, 1);
	}

	if (NULL != actExp)
	{
		delete actExp;
	}
}

bool Terrain::LoadInfoFromFile(const char *fname)
{
	FILE *fp = fopen(fname, "rt");
	if (fp == NULL) return false;

	char buffer[512];
	while (fgets(buffer, 512, fp)) {
		if (strstr(buffer, "[anchor]")) { // reading anchor info
			fgets(buffer, 512, fp);
			sscanf(buffer, "%g%g%g", &info.posx, &info.posy, &info.dir);
		}
		if (strstr(buffer, "[terrain]")) { // reading terrain info
			fgets(buffer, 512, fp);
			sscanf(buffer, "%g", &info.height_scale);
		}
		if (strstr(buffer, "[rocks]")) { // reading rock info
			fgets(buffer, 512, fp);
			int num=0;
			sscanf(buffer, "%d", &num);
			for (int i=0; i<num; i++) {
				RockInfo rock;
				fgets(buffer, 512, fp);
				sscanf(buffer, "%d", &rock.id);
				fgets(buffer, 512, fp);
				sscanf(buffer, "%g", &rock.size);
				fgets(buffer, 512, fp);
				sscanf(buffer, "%g%g", &rock.posx, &rock.posy);
				fgets(buffer, 512, fp);
				sscanf(buffer, "%g", &rock.dir);

				info.rocks.push_back(rock);
			}
		}
	}
	fclose(fp);
	return true;
}

float lowest, highest;
GLuint ttex=0;

bool Terrain::Load()
{
	if (!LoadInfoFromFile(inf_filename))
		return false;

	nv::Image iTex;
	nv::Image dTex;

	printf("loading terrain...\n");
	char tex_path[512];
	strcpy(tex_path, depth_tex_filename);
	char* idx = strrchr(tex_path, '\\');
	strcpy(idx+1, "moon-tex.dds");
	if (!iTex.loadImageFromFile(tex_path))
	{
		return false;
	}

	if (!dTex.loadImageFromFile(depth_tex_filename))
		return false;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glCompressedTexImage2DARB( GL_TEXTURE_2D, 0, iTex.getInternalFormat(), iTex.getWidth(), iTex.getHeight(), 0, iTex.getImageSize(), iTex.getLevel(0));

	ttex = tex;

	height = dTex.getHeight();
	width = dTex.getWidth();

	int size = height * width;

	heights = new float [size * sizeof(float)];

	GLubyte *src = (GLubyte*)dTex.getLevel(0);
	// added by TangMin
	int stride = 3;
	if (dTex.getFormat() == 6409)
		stride = 1;
	else if (dTex.getFormat() == 6407)
		stride = 3;

// 	for(int i=0; i<size; i++)
// 		heights[i] = (float)src[stride*i] * SCALE;
	//-- 逆y轴
	lowest = 10000.f;
	highest = 0.f;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			heights[x + y*width] = (float)src[stride*(x + (height-y-1)*width)] * SCALE_TER_H;
			lowest = min(lowest, heights[x + y*width]);
			highest = max(highest, heights[x + y*width]);
		}
	}
	
	//-- gen a flat zone to land the explorer
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int dx = x - info.posx - 512;
			int dy = y - info.posy - 512;
			if (dx*dx + dy*dy <= 7*7)
			{
				heights[x + y*width] = GetPosHeight(info.posx, info.posy);
			}
		}
	}

	normals = new float [3 * size * sizeof(float)];

	for(int z=1; z<height-1; z++) {
		for(int x=1; x<width-1; x++) {
			float dyx =  heights[x+1 + z*width]
						-heights[x-1 + z*width];
			float dyz =  heights[x + (z+1)*width]
						-heights[x + (z-1)*width];

			nv::vec3f vx(SCALE_TER_W,   dyx, 0.0f); //-- 为什么x,z取1而不是2？
			nv::vec3f vz(0.0f, -dyz,   -SCALE_TER_W);
			nv::vec3f v = normalize(cross(vx, vz));
			normals[3*(x + z*width)    ] = v.x;
			normals[3*(x + z*width) + 1] = v.y;
			normals[3*(x + z*width) + 2] = v.z;
		}
	}

	if (!LoadRocks()) {
		printf("Couldn't load rocks\n");
		exit(0);
	}

	if (!LoadExplorer()) {
		printf("Couldn't find explorer.obj.\n");
		exit(0);
	}

	if (!isLoaded)
	{
		TransformCameras();
	}

	if (!LoadExplorerAnimation())
	{
		AfxMessageBox(_T("load anm error"));
	}

	MakeTerrain();

	return true;
}

extern void updateCameraViews();

bool Terrain::UpdateAnimation(float dt)
{
	float dt_l = dt;
	if (0 != fpsAnm)
	{
		dt_l = 1.0f / (float)fpsAnm;
	}

	bool isEnd = anms->update(dt_l);

	updateCameraListAnm(actExp, inverse(matExp));

	updateCameraViews();

	return isEnd;
}

void Terrain::ToggleAnimation()
{
	bAnimation = !bAnimation;
// 
// 	if (!bAnimation)
// 	{
// 		resetCameraListAnm();
// 		resetAnimation();
// 		updateCameraViews(); //-- update a frame after terminating animation
// 	}
}

bool Terrain::IsAnimating()
{
	return bAnimation;
}

void Terrain::TogglePauseAnimation()
{
	bPauseAnimation = !bPauseAnimation;
}

bool Terrain::IsAnimationPaused()
{
	return bPauseAnimation;
}

void Terrain::StartAnimation()
{
	anms->start();
}

void Terrain::EndAnimation()
{
	anms->setEnd();
}

void Terrain::ResetAnimation()
{
	resetCameraListAnm();
	StartAnimation();
	updateCameraViews(); //-- update a frame after terminating animation
}

void Terrain::Draw(GLfloat *lightDir, GLfloat *camPos)
{
	Draw(-10000.0f, lightDir, camPos); 
}

extern bool show_camera;

void Terrain::Draw(float minCamZ, GLfloat *lightDir, GLfloat *camPos)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	DrawExplorer();

	beginProgram(true);
	DrawRocks();
	endProgram();

	glPopMatrix();


	glActiveTexture(GL_TEXTURE1);
	beginProgram(true);
	glCallList(terrain_list);
	endProgram();

	glMatrixMode(GL_MODELVIEW);
	glActiveTexture(GL_TEXTURE0);
}

void Terrain::Draw2()
{
	if (IsAnimating())
	{
		glPushMatrix();
		glMultMatrixf(matExp.get_value());
		anms->draw();
		glPopMatrix();
	}

	if (show_camera)
	{
		DrawCameras();
	}
}

void Terrain::DrawCoarse()
{
	float half_width = 0.5f*(float)width*SCALE_TER_W;
	float half_height = 0.5f*(float)height*SCALE_TER_W;

	const float inv_height = 1.0f / ((float)height*SCALE_TER_W);
	const float inv_width = 1.0f / ((float)width*SCALE_TER_W);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-half_width, 0, -half_height);

	glBindTexture(GL_TEXTURE_2D, tex);
	
	for(int z=1; z<height-2; z+=8)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int x=1; x<width-1; x+=8)
		{
			float fx = (float)x*SCALE_TER_W;
			float fz = (float)z*SCALE_TER_W;
			glTexCoord2f( fx*inv_width, fz*inv_height );
			glNormal3fv(&normals[3*(x + z*width)]);
			glVertex3f( fx, heights[x + z*width], fz );
			glTexCoord2f( fx*inv_width, (fz+8.0f*SCALE_TER_W)*inv_height );
			glNormal3fv(&normals[3*(x + (z+8)*width)]);
			glVertex3f( fx, heights[x + (z+8)*width], fz+8.0f*SCALE_TER_W );
		}
		glEnd();
	}
	glPopMatrix();
}

void Terrain::MakeTerrain()
{
	printf("building terrain...\n");
	float half_width = 0.5f*(float)width*SCALE_TER_W;
	float half_height = 0.5f*(float)height*SCALE_TER_W;

	const float inv_height = 1.0f / ((float)height * SCALE_TER_W);
	const float inv_width = 1.0f / ((float)width * SCALE_TER_W);

	terrain_list = glGenLists(1);
	glNewList(terrain_list, GL_COMPILE);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-half_width, 0, -half_height);

	glBindTexture(GL_TEXTURE_2D, tex);
	
	float high_ever = (lowest+highest)/2.f;

	//-- terrain from height map
	for(int z=1; z<height-2; z++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int x=1; x<width-1; x++)
		{
			float fx = (float)x * SCALE_TER_W;
			float fz = (float)z * SCALE_TER_W;

			glTexCoord2f( fx*inv_width*5, fz*inv_height*5 );
			glNormal3fv(&normals[3*(x + z*width)]);
			glVertex3f( fx, heights[x + z*width], fz );

			glTexCoord2f( fx*inv_width*5, (fz+SCALE_TER_W)*inv_height*5 );
			glNormal3fv(&normals[3*(x + (z+1)*width)]);
			glVertex3f( fx, heights[x + (z+1)*width], fz+SCALE_TER_W );
		}
		glEnd();
	}

	glPopMatrix();

	glEndList();
}

//-- 图像空间上插值小数坐标点对应的高度
float Terrain::GetPosHeight(float fx, float fz) const
{
	float half_width = width/2;
	float half_height = height/2;
	fx += half_width;
	fz += half_height;

	// 在三角形内对高度值做线性插值
	int x = (int)fx;
	int z = (int)fz;
	float fracx = fx - (float)x;
	float fracz = fz - (float)z;

	float h = 0.0f;

	// 在下三角形中
	if (fracx + fracz < 1.0f)
	{
		float h0 = heights[x + z*width];
		float h1 = heights[x+1 + z*width];
		float h2 = heights[x + (z+1)*width];

		float h02 = (1.0f - fracz)*h0 + fracz*h2;
		float h12 = (1.0f - fracz)*h1 + fracz*h2;
		h = ((1.0f - fracz - fracx)*h02 + fracx*h12) / (1.0f - fracz);
	}
	// 在上三角形中
	else
	{
		float h0 = heights[x+1 + (z+1)*width];
		float h1 = heights[x+1 + z*width];
		float h2 = heights[x + (z+1)*width];

		float h01 = fracz*h0 + (1.0f - fracz)*h1;
		float h21 = fracz*h2 + (1.0f - fracz)*h1;
		h = ((fracz - (1.0f - fracx))*h01 + (1.0f - fracx)*h21) / fracz;
	}

	return h;
}

bool Terrain::LoadExplorer()
{
	setlocale(LC_ALL,   "chs");

	CString filepath(explorer_filename);
	int pos = filepath.ReverseFind(_T('\\'));
	CString filename = filepath.Right(filepath.GetLength()-pos-1);

	LPWSTR oldDir = new WCHAR[512];
	GetCurrentDirectory(512, oldDir);
	if (pos != -1) {// others will failed when open from recent list
		CString dirname = filepath.Left(pos);
		SetCurrentDirectory(dirname);
	}

	pos = filename.ReverseFind(_T('.'));
	CString extendname = filename.Right(filename.GetLength()-pos-1);

	if (!extendname.CompareNoCase(_T("wrl")))
	{
		FILE *fp = _wfopen(explorer_filename, L"rb");

		DataFileTypes ftype = F_FCT;
		if (wcsstr(explorer_filename, _T("_asm.wrl")))
			ftype = F_ACT;

		if (NULL != actExp)
		{
			delete actExp;
		}
		actExp = new ACTInfo(fp, ftype);
	} 
	else if (!extendname.CompareNoCase(_T("bin")))
	{
		FILE *fp = _wfopen(explorer_filename, L"rb");
		actExp = new ACTInfo(fp);
		fclose(fp);
	}
	
	SetCurrentDirectory(oldDir);

	CString visPath = CString(explorer_filename);
	int idx = visPath.ReverseFind(_T('.'));
	visPath = visPath.Left(idx+1);
	visPath += _T("vis");
	std::wifstream is(visPath);
	if (!is.fail())
	{
		int count;
		is>>count;
		vec_is_vis.clear();
		for (int i = 0; i < count; i++)
		{
			bool b;
			is>>b;
			vec_is_vis.push_back(b);
		}
	}

	AdhereExpToTer();

	return true;
}

static nv::matrix4f getRotMatByAxis(nv::vec3f axis_org, nv::vec3f axis_dir, float angle)
{
	nv::matrix4f mat1, mat2, mat3;
	matrix4f mat_t;
	mat1.set_translate(-axis_org);

	vec3f dir(axis_dir.get_value());
	mat_t.set_rotate(dir, angle);
	mat2.set_value(const_cast<float*>(mat_t.get_value()));

	mat3.set_translate(axis_org);
	return mat3 * mat2 * mat1;
}

//-- 使用时要保证逆时针转时leg往上抬，顺时针时往下
nv::matrix4f Terrain::_recurRotateByAxis(nv::vec3f axis_org, nv::vec3f axis_dir, nv::vec3f leg, float lower, float higher, float tolerance, int count)
{
	float angle = (lower + higher)/2;
	nv::matrix4f mat = getRotMatByAxis(axis_org, axis_dir, angle);

	if (1 == count)
	{
		return mat;
	}

	nv::vec4f v4 = mat * nv::vec4f(leg, 1.0f);
	nv::vec3f leg_t = transHomoToNon(v4);

	float h = GetPointHeight(leg_t);
	if (std::abs(h) < tolerance)
	{
		return mat;
	}
	else if (h > tolerance)
	{
		return _recurRotateByAxis(axis_org, axis_dir, leg, lower, angle, tolerance, count-1);
	}
	else
	{
		return _recurRotateByAxis(axis_org, axis_dir, leg, angle, higher, tolerance, count-1);
	}
}

//-- test dig soil
static int g_lId = 0;
//-- test end

nv::matrix4f Terrain::getTransThreeLeg(nv::vec3f vm, nv::vec3f v0, nv::vec3f v1, nv::vec3f v2, nv::vec3f v3)
{
	nv::vec3f v[] = {v0, v1, v2, v3};
	nv::vec3f v_h[4], vm_h;
	nv::matrix4f mat;

	//-- 目前暂时只让最低的点落地
	{
		int lowestId = 0;
		for (int i = 1; i < 4; i++)
		{
			if (GetPointHeight(v[i]) < GetPointHeight(v[lowestId]))
			{
				lowestId = i;
			}
		}
		nv::vec3f trans(0.f, -GetPointHeight(v[lowestId]), 0.f);
		mat.set_translate(trans);

		//-- test dig soil
		g_lId = lowestId;
		//-- test end

	}

	
	return mat; //-- 暂时不采用后面的策略

	//-- 第一个脚着地
	nv::vec3f trans(0.f, -GetPointHeight(v[0]), 0.f);
	mat.set_translate(trans);
	nv::vec4f v4f;
	//-- 变换
	for (int i = 0; i < 4; i++)
	{
		v4f = mat * nv::vec4f(v[i], 1.f);
		v_h[i] = transHomoToNon(v4f);
	}
	v4f = mat * nv::vec4f(vm, 1.f);
	vm_h = transHomoToNon(v4f);

	//-- 第二个脚着地
	nv::vec3f dir = nv::normalize(v_h[0] - vm_h);
	nv::matrix4f mat1 = _recurRotateByAxis(vm_h, dir, v_h[1], -3.14f/3.f, 3.14f/3.f, 0.01f, 30);
	v4f = mat1 * nv::vec4f(v_h[1], 1.f);
	nv::vec3f v_temp = transHomoToNon(v4f);
	float h1 = GetPointHeight(v_temp);
	nv::matrix4f mat2 = _recurRotateByAxis(vm_h, -dir, v_h[1], -3.14f/3.f, 3.14f/3.f, 0.01f, 30);
	v4f = mat2 * nv::vec4f(v_h[1], 1.f);
	v_temp = transHomoToNon(v4f);
	float h2 = GetPointHeight(v_temp);
	if (std::abs(h1) < std::abs(h2))
	{
		mat = mat1 * mat;
	} 
	else
	{
		mat = mat2 * mat;
	}
	//-- 变换
	for (int i = 0; i < 4; i++)
	{
		v4f = mat * nv::vec4f(v[i], 1.f);
		v_h[i] = transHomoToNon(v4f);
	}

	//-- 第三个脚着地
	dir = nv::normalize(v_h[0] - v_h[1]);
	mat1 = _recurRotateByAxis(v_h[0], dir, v_h[2], -3.14f/3.f, 3.14f/3.f, 0.01f, 30);
	v4f = mat1 * nv::vec4f(v_h[2], 1.f);
	v_temp = transHomoToNon(v4f);
	h1 = GetPointHeight(v_temp);
	mat2 = _recurRotateByAxis(vm_h, -dir, v_h[2], -3.14f/3.f, 3.14f/3.f, 0.01f, 30);
	v4f = mat2 * nv::vec4f(v_h[2], 1.f);
	v_temp = transHomoToNon(v4f);
	h2 = GetPointHeight(v_temp);
	if (std::abs(h1) < std::abs(h2))
	{
		mat = mat1 * mat;
	} 
	else
	{
		mat = mat2 * mat;
	}

	return mat;
}

//-- 在探测器有4个支架的前提下求能使其与地面贴合的变换矩阵
void Terrain::AdhereExpToTer()
{
	nv::matrix4f mat, mat_t, mat_s;
	mat_t.set_translate(nv::vec3f(info.posx*SCALE_TER_W, GetPosHeight(info.posx, info.posy)+RADIUS, info.posy*SCALE_TER_W));
	mat_s.set_scale(SCALE_MDL);
	mat = mat_t*mat_s;

	const int legs_num = 4;
	std::vector<vec3f> legs_t = actExp->GetFourLeg();
	if (legs_t.size() != legs_num)
	{
		matExp = mat;
		return;
	}

	nv::vec3f legs[legs_num];
	memcpy(legs, &legs_t[0], legs_num*sizeof(nv::vec3f));

	nv::vec3f legs_h[4];
	nv::vec4f v4f;
	for (int i = 0; i < 4; i++)
	{
		v4f = mat * nv::vec4f(legs[i], 1.f);
		legs_h[i] = transHomoToNon(v4f);
	}
	v4f = mat * nv::vec4f(nv::vec3f(0.f, 0.f, 0.f), 1.f);
	nv::vec3f vmid = transHomoToNon(v4f);
	mat = getTransThreeLeg(vmid, legs_h[0], legs_h[1], legs_h[2], legs_h[3]) * mat;

	matExp = mat;
}

void Terrain::DrawExplorer()
{
	glPushMatrix();

	glMultMatrixf(matExp.get_value());

	//glRotatef(info.dir/180*M_PI, 0.f, 1.f, 0.f);
	actExp->DisplayFace(true/*bAnimation*/);


	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

bool Terrain::LoadExplorerAnimation()
{
	std::wifstream is(explorer_animation_filename);
	if (is.fail())
	{
		return false;
	}

	if (NULL != anms)
	{
		delete anms;
	}
	anms = new AnimationInfo(is);
	anms->setAct(actExp);

	return true;
}

bool Terrain::LoadDumpSoil()
{
	char* old_local = _strdup(setlocale(LC_CTYPE,NULL));
	setlocale( LC_CTYPE, "chs");

	CString path(explorer_filename);
	path = path.Left(path.GetLength() - 3);
	path += _T("mrk");

	TRY
	{
		CStdioFile fin(path, CFile::modeRead);

		CString buff;
		char *cbuff = NULL;
		fin.ReadString(buff);
		WideCharToMultiByte_M(buff, &cbuff);
		int n;
		sscanf(cbuff, "%d", &n);
		delete[] cbuff;

		for (int i = 0; i < n; i++)
		{
			buff.Empty();
			CString buff2;
			fin.ReadString(buff2);
			fin.ReadString(buff2);
			buff += buff2;
			buff += CString(_T(" "));
			fin.ReadString(buff2);
			buff += buff2;
			WideCharToMultiByte_M(buff, &cbuff);
			nv::vec3f pos, dir;
			sscanf(cbuff, "%f, %f, %f %f, %f, %f", &pos.x, &pos.y, &pos.z, &dir.x, &dir.y, &dir.z);
			delete[] cbuff;

			CString cam_name;
			fin.ReadString(cam_name);

			fin.ReadString(buff);
			WideCharToMultiByte_M(buff, &cbuff);
			int mdl_id, type;
			sscanf(cbuff, "%d, %d", &mdl_id, &type);
			delete[] cbuff;
			if (0 != type) //-- not tailflame
			{
				continue;
			}

			nv::vec3f pos_t, dir_t;
			set_pt2(pos_t, pos);
			set_pt2(dir_t, dir);
		}
	}
	CATCH (CFileException, e)
	{
		setlocale( LC_CTYPE, old_local);
		return false;
	}
	END_CATCH

	setlocale( LC_CTYPE, old_local);
	return true;
}

void Terrain::TransformCameras()
{
	transCameraList(matExp);
// 	float x = info.posx;
// 	float z = info.posy;
// 	float y = GetPosHeight(x, z);
// 
// 	for (int i = 0; i < cameraList.size(); i++)
// 	{
// 		cameraList[i].scale(SCALE_MDL);
// 		cameraList[i].translate(nv::vec3f(x, y+RADIUS, z));
// 	}
}

static void toScreen(nv::vec4f& pos, nv::matrix4f& mat, int width, int height)
{
	pos = mat * pos;
	pos = nv::vec4f(pos.x/pos.w, pos.y/pos.w, 0.0f, 0.0f);
	pos.x = (pos.x * width + width) / 2;
	pos.y = (pos.y * height+ height) / 2;
}

extern std::vector<camera> cameraList;
void Terrain::DrawCameras()
{
	GLfloat entries[16];

	glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)entries);
	nv::matrix4f mat_p;
	for (int i = 0; i < 4; i++)
	{
		mat_p.set_column(i, entries+i*4);
	}

	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)entries);
	nv::matrix4f mat_v;
	for (int i = 0; i < 4; i++)
	{
		mat_v.set_column(i, entries+i*4);
	}
	nv::matrix4f mat = mat_p * mat_v;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);
	GLfloat w_v = viewport[2] - viewport[0];
	GLfloat h_v = viewport[3] - viewport[1];

	bool mode = glxEnableLighting(false);
	float width = glxSetWidth(2.0);
	COLORREF color = glxSetColor(RGB(255, 255, 255));
	bool mode_depth = glxEnableDepthTest(false);

	glxBegin2D(w_v, h_v);
	
	int num = getCameraNum();
	for (int i = 0; i < num; i++)
	{
		camera &m_camera = cameraList[i];

		float p_t[3], v_t[3];
		m_camera.getPos(p_t);
		m_camera.getView(v_t);

		nv::vec4f p1(p_t, 1.0f);
		nv::vec4f v(v_t, 0.0f);
		nv::vec4f p2 = p1 + v * 10.f;
		toScreen(p1, mat, w_v, h_v);
		toScreen(p2, mat, w_v, h_v);
		nv::vec4f dir_a = p2 - p1; //- 轴向
		dir_a = nv::normalize(dir_a);
		nv::vec4f dir_t(-dir_a.y, dir_a.x, 0.0f, 0.0f); //- 切向
		p2 = p1 + dir_a * 40.f;

		nv::vec4f ap_c = p1 + dir_a*10.f;
		nv::vec4f ap_l = ap_c + dir_t*6.f;
		nv::vec4f ap_r = ap_c - dir_t*6.f;

		nv::vec4f bp_c = p2 - dir_a*10.f;
		nv::vec4f bp_l = bp_c + dir_t*6.f;
		nv::vec4f bp_r = bp_c - dir_t*6.f;

#define WC(pt) (pt.x, pt.y)

		glBegin(GL_LINES);
		glVertex2f WC(p1);
		glVertex2f WC(p2);
		glEnd();

		glBegin(GL_TRIANGLES);
		//	glVertex2f WC(pt1);
		//	glVertex2f WC(ap_l);
		//	glVertex2f WC(ap_r);
		glVertex2f WC(p2);
		glVertex2f WC(bp_r);
		glVertex2f WC(bp_l);
		glEnd();
	}

	glxEnd2D();

	glxEnableDepthTest(mode_depth);
	glxEnableLighting(mode);
	glxSetColor(color);
	glxSetWidth(width);
}

bool Terrain::LoadRocks()
{
	for (size_t i=0; i<info.rocks.size(); i++) {
		DisplayInfo dsp;
		dsp.model = new nv::Model;

		char buffer[512];
		sprintf(buffer, ROCK_OBJ_FILENAME, info.rocks[i].id);

		if (!dsp.model->loadModelFromFile(&buffer[0])) {
			if (!dsp.model->loadModelFromFile(&buffer[3]))
				return false;
		}
		//rescale it...
		dsp.model->rescale(3.f);
		dsp.model->compileModel();

		int totalVertexSize = dsp.model->getCompiledVertexCount() *  dsp.model->getCompiledVertexSize() * sizeof(GLfloat);
		int totalIndexSize = dsp.model->getCompiledIndexCount() * sizeof(GLuint);

		glGenBuffers(1, &dsp.vboId);
		glBindBuffer(GL_ARRAY_BUFFER, dsp.vboId);
		glBufferData(GL_ARRAY_BUFFER, totalVertexSize, dsp.model->getCompiledVertices(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &dsp.eboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dsp.eboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndexSize, dsp.model->getCompiledIndices(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		dsp.tex = new nv::Image;
		sprintf(buffer, ROCK_TEX_FILENAME, info.rocks[i].id);

		if (!dsp.tex->loadImageFromFile(&buffer[0])) {
			if (!dsp.tex->loadImageFromFile(&buffer[3]))
				return false;
		}

		glGenTextures(1, &dsp.texId);
		glBindTexture(GL_TEXTURE_2D, dsp.texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, dsp.tex->getInternalFormat(), dsp.tex->getWidth(), dsp.tex->getHeight(), 0, dsp.tex->getFormat(), dsp.tex->getType(), dsp.tex->getLevel(0));
		glBindTexture(GL_TEXTURE_2D, 0);

		dspRocks.push_back(dsp);
	}

	return true;
}

void Terrain::DrawRocks()
{
	for (size_t i=0; i<dspRocks.size(); i++) {
		glPushMatrix();

		DisplayInfo &dsp = dspRocks[i];

// 		float x = width/2+info.rocks[i].posx;
// 		float z = height/2-info.rocks[i].posy;
// 		float y = heights[int(x+z*width)];
// 
		float x = info.rocks[i].posx;
		float z = info.rocks[i].posy;
		float y = GetPosHeight(x, z);
 		glTranslatef(x*SCALE_TER_W, y, z*SCALE_TER_W);

		glRotatef(info.rocks[i].dir/180*M_PI, 0.f, 1.f, 0.f);
		float s = info.rocks[i].size;
		glScalef(s*SCALE_TER_W, s*SCALE_TER_W, s*SCALE_TER_W);


		glBindBuffer(GL_ARRAY_BUFFER,  dsp.vboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  dsp.eboId);
		int stride = dsp.model->getCompiledVertexSize() * sizeof(GLfloat);
		int normalOffset = dsp.model->getCompiledNormalOffset() * sizeof(GLfloat);
		int texCoordOffset = dsp.model->getCompiledTexCoordOffset() * sizeof(GL_FLOAT);

		glVertexPointer(dsp.model->getPositionSize(), GL_FLOAT, stride, NULL);
		glNormalPointer(GL_FLOAT, stride, (GLubyte *)NULL + normalOffset);
		glTexCoordPointer(dsp.model->getTexCoordSize(), GL_FLOAT, stride, (GLubyte*)NULL + texCoordOffset);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, dsp.texId);

		//glColor3f(0.917647f, 0.776471f, 0.576471f);
		glColor3f(1.f, 1.f, 1.f);
		glDrawElements(GL_TRIANGLES, dsp.model->getCompiledIndexCount(), GL_UNSIGNED_INT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
		glColor3f(1.0f, 1.0f, 1.0f);
		glPopMatrix();
	}
}

