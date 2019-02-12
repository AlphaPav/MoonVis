// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.

#pragma once

#include "main.h"
#include <vector>
//#include "Model.h"
#include <nvModel.h>
#include <nvImage.h>
#include <glx.h>
#include "DataFile.h"
#include "../Particles/TailFlame.h"
#include "../Particles/soil.h"

class CMaterial;
class AnimationInfo;

//const char TERRAIN_TEX_FILENAME[] = "../../media/textures/gcanyon.dds";
const char TERRAIN_TEX_FILENAME[] = "../../media/textures/moon-tex1.dds";

//const char DEPTH_TEX_FILENAME[] = "../../media/textures/gcanyond.png";
//const char DEPTH_TEX_FILENAME[] = "../../media/textures/moon-heightmap.png";
const char DEPTH_TEX_FILENAME[] = "../../media/textures/001.png";
const char INF_FILENAME[] = "../../media/textures/001.inf";

const char ROCK_OBJ_FILENAME[] = "../../media/rocks/%02d/model.obj";
const char ROCK_TEX_FILENAME[] = "../../media/rocks/%02d/texture.png";

const char EXPLORER_FILENAME[] = "../../media/models/apollo.obj";


class RockInfo {
public:
	int id;
	float size;
	float posx, posy, dir;
};

class TerrainInfo {
public:
	float posx, posy, dir;
	float height_scale;
	std::vector<RockInfo> rocks;
};

class objDisplayInfo {
public:
	//objModel	*model;
	nv::Model *model;
	nv::Image *tex;

	GLuint	vboId;
	GLuint	eboId;
	GLuint texId;
};

class TailFlameAnm : public TailFlame
{
	//int _id; // the id whose holdering model to which the tailflame inhere
	ModelInfo *_mdl; // the model to which the tailflame inhere
	nv::vec3f _pos;
	nv::vec3f _pos_anm;
	nv::vec3f _dir_jet;
	nv::vec3f _dir_jet_anm;
	float _vel_jet;
	float _vel_anm;
	nv::matrix4f _mat;

public:
	TailFlameAnm(ModelInfo* mdl, const nv::vec3f& pos, const nv::vec3f& dir_jet, float vel_jet, float h, float w, int num = 128)
		: _mdl(mdl)
		, _pos(pos)
		, _pos_anm(nv::vec3f(0.f, 0.f, 0.f))
		, _dir_jet(dir_jet)
		, _dir_jet_anm(_dir_jet)
		, _vel_jet(vel_jet)
		, _vel_anm(0.f)
		, TailFlame(h, w, num)
	{
	}

	TailFlameAnm(const TailFlameAnm& rhd)
		: _mdl(rhd._mdl)
		, _pos(rhd._pos)
		, _pos_anm(nv::vec3f(0.f, 0.f, 0.f))
		, _dir_jet(rhd._dir_jet)
		, _dir_jet_anm(rhd._dir_jet_anm)
		, _vel_jet(rhd._vel_jet)
		, _vel_anm(0.f)
		, TailFlame(rhd._height, rhd._width, rhd._num_smoke)
	{
	}

	ModelInfo* GetOwner() { return _mdl; }
	nv::vec3f GetPos() { return _pos; }

	void Reset()
	{
		_pos_anm = nv::vec3f(0.f, 0.f, 0.f);
		_dir_jet_anm = _dir_jet;
		_vel_anm = 0.f;
		TailFlame::Reset();
	}

	void Step(const nv::matrix4f mat, float dt)
	{
		nv::vec4f pos04 = inverse(mat) * nv::vec4f(0.f, 0.f, 0.f, 1.f);
		nv::vec3f pos03(pos04.x/pos04.w, pos04.y/pos04.w, pos04.z/pos04.w);
		nv::vec4f dir4 = inverse(mat) * nv::vec4f(_dir_jet, 1.f);
		_dir_jet_anm = nv::vec3f(dir4.x/dir4.w, dir4.y/dir4.w, dir4.z/dir4.w);
		_dir_jet_anm -= pos03;

		nv::quaternionf quat(nv::vec3f(0.f, 0.f, 1.f), _dir_jet_anm);

		nv::matrix4f mat_r;
		quat.get_value(mat_r);
		nv::vec4f pos4 = inverse(mat_r) * mat * mat_r * nv::vec4f(0.f, 0.f, 0.f, 1.f);
		nv::vec3f pos(pos4.x/pos4.w, pos4.y/pos4.w, pos4.z/pos4.w);

		TailFlame::Step(nv::vec3f(0.f, 0.f, 0.f), 0.25f, dt);
		_mat = mat;

		_vel_anm = length(pos - _pos_anm) / dt;
		_pos_anm = pos;
	}

	void Draw()
	{
		nv::quaternionf quat(nv::vec3f(0.f, 0.f, 1.f), _dir_jet);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(_mat.get_value());
		glTranslatef(_pos.x, _pos.y, _pos.z);
		nv::applyRotation(quat);
		float scale = 1.f / SCALE_MDL;
		glScalef(scale, scale, scale);

		GLfloat buffer[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, buffer);
		nv::matrix4f mat_v;
		for (int i = 0; i < 4; i++)
		{
			mat_v.set_column(i, &buffer[4*i]);
		}
		mat_v = inverse(mat_v);
		nv::vec4f cam_pos4 = mat_v * nv::vec4f(0.f, 0.f, 0.f, 1.f);
		nv::vec3f cam_pos(cam_pos4.x/cam_pos4.w, cam_pos4.y/cam_pos4.w, cam_pos4.z/cam_pos4.w);

		TailFlame::Draw(cam_pos);
		glPopMatrix();
	}
};

class DumpSoilAnm : public DumpSoil
{
	ModelInfo *_mdl; // the model to which the dumpsoil animation adhere
	nv::vec3f _pos;
	nv::matrix4f _mat;

public:
	DumpSoilAnm(ModelInfo* mdl, const nv::vec3f& pos, float _threshold_dis, float _radius, float _soil_radius = 0.02f)
		: _mdl(mdl)
		, _pos(pos)
		, DumpSoil(_threshold_dis, _radius, _soil_radius)
	{
	}

	DumpSoilAnm(const DumpSoilAnm& rhd)
		: _mdl(rhd._mdl)
		, _pos(rhd._pos)
		, DumpSoil(rhd.threshold_dis, rhd.radius, rhd.soil_radius)
	{
	}

	ModelInfo* GetOwner() { return _mdl; }
	nv::vec3f GetPos() { return _pos; }

	void Reset()
	{
		DumpSoil::reset();
	}

	void Step(const nv::matrix4f mat, float dt)
	{
		update(dt);
		_mat = mat;
	}

	void Draw()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(_mat.get_value());
		glTranslatef(_pos.x, _pos.y, _pos.z);
 		float scale = 1.f / SCALE_MDL;
 		glScalef(scale, scale, scale);

		DumpSoil::draw();

		glPopMatrix();
	}
};

class DigSoilAnm : public DigSoil
{
	ModelInfo *_mdl; // the model to which the dumpsoil animation adhere
	nv::vec3f _pos;
	nv::matrix4f _mat;

public:
	DigSoilAnm(ModelInfo* mdl, const nv::vec3f& pos, float _radius, float width, float _hwRatio, float _soilRadius, float _deltaRadius)
		: _mdl(mdl)
		, _pos(pos)
		, DigSoil(_radius, width, _hwRatio, _soilRadius, _deltaRadius)
	{
	}

	DigSoilAnm(const DigSoilAnm& rhd)
		: _mdl(rhd._mdl)
		, _pos(rhd._pos)
		, DigSoil(rhd)
	{
	}

	ModelInfo* GetOwner() { return _mdl; }
	nv::vec3f GetPos() { return _pos; }

	void Reset()
	{
		DigSoil::reset();
	}

	void Step(const nv::matrix4f mat, float dt)
	{
		update(dt);
		_mat = mat;
	}

	void Draw()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(_mat.get_value());
		glTranslatef(_pos.x, _pos.y, _pos.z);
		float scale = 1.f / SCALE_MDL;
		glScalef(scale, scale, scale);

		DigSoil::draw();

		glPopMatrix();
	}
};

class Terrain
{
public:
	Terrain();
	~Terrain();
	bool	Load();
	bool LoadInfoFromFile(const char *);

	void	Draw(GLfloat *lightDir, GLfloat *camPos);
	void	Draw(float minCamZ, GLfloat *lightDir, GLfloat *camPos);
	void	Draw2();
	void	DrawCoarse();
	int		getDim(){ return (width>height)?width:height;	}

	//AnimationInfo*	GetAnmInfo() { return anms; }
	void	GetRotateAxis(line &axis1, line &axis2, line &axis3, line &axis4);
	void	GetRotateAng(float &theta, float &alpha, float &beta, float &gama, float & theta1);
	void	SetRotateAng(float &theta, float &alpha, float &beta, float &gama, float & theta1);
	void	GetRotateVel(float &v1, float &v2, float &v3, float &v4, float &v5);
	void	SetRotateVel(float &v1, float &v2, float &v3, float &v4, float &v5);
	void	ToggleAnimation();
	bool	IsAnimating();
	void	TogglePauseAnimation();
	bool	IsAnimationPaused();
	bool	UpdateAnimation(float dt);
	void	StartAnimation();
	void	EndAnimation();
	void	ResetAnimation();
	void	SetAnmFrameRate(int fps) { fpsAnm = fps; }
	bool	LoadExplorerAnimation();
	void DrawRocks();

	void computeShadowRatio(float &r, float &a);

	nv::matrix4f	getMatexp();

private:
	void	MakeTerrain();
	float	GetPosHeight(float fx, float fz) const;
	float	GetPointHeight(const nv::vec3f& p) const
	{
		return p.y - GetPosHeight(p.x/SCALE_TER_W, p.z/SCALE_TER_W);
	}

	bool	LoadExplorer();
	nv::matrix4f	_recurRotateByAxis(nv::vec3f axis_org, nv::vec3f axis_dir, nv::vec3f leg, float lower, float higher, float tolerance, int count);
	nv::matrix4f	getTransThreeLeg(nv::vec3f vm, nv::vec3f v0, nv::vec3f v1, nv::vec3f v2, nv::vec3f v3);
	void	AdhereExpToTer();
	void	DrawExplorer();
	bool	LoadDumpSoil();

	void TransformCameras();
	void DrawCamerasOld();
	void DrawCameras();
	void DrawWings();

	bool LoadRocks(char *);


	void UpdateDumpSoil(float dt);
	void DrawDumpSoil();

	void UpdateDigSoil(float dt);
	void DrawDigSoil();

	GLuint	tex;
	float	*heights;
	float	*normals;
	std::vector<nv::vec3f *> entities;

	int		height;
	int		width;

	ACTInfo* actExp;
	nv::matrix4f matExp; //-- transformation matrix for explorer

	AnimationInfo* anms;
	int anmsetIdCur; //-- id of current animation set to play in anmsetsExp

	bool bAnimation;
	bool bPauseAnimation;
	int iAnmType;
	int fpsAnm;

	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	CMaterial *_material;
	bool LoadMaterialInfo();

	/////////////////////////////////////////////////////////////

	std::vector<objDisplayInfo> dspRocks;

	TerrainInfo info;

	GLuint	terrain_list;
};

void	DelPointAt(int i);
void	SetPointValue(int i, float x, float y, float z);
void	SetPointNormalValue(int i, float x, float y, float z);
void	ClearPointInfo();
bool	LoadPointInfo();

