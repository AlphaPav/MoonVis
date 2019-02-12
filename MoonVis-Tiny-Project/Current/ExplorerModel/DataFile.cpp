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
#include <windows.h>
#include "DataFile.h"
#include "model.h"
#include <algorithm>
#include <vector>
#include <queue>

#define BUFSIZE 512
static TCHAR   szTempPath[BUFSIZE];   
static TCHAR   szTempFile[BUFSIZE];   

#include "zlib/zlib.h"
extern int zlib_def(FILE *source, FILE *dest, int level);
extern int zlib_inf(FILE *source, FILE *dest);

#include <gl/gl.h>
#include "glx.h"

extern std::vector<bool> vec_is_vis;

bool
DataFileInfo::WriteHead(FILE *fp, DataFileTypes ftype)
{
	switch (ftype) {
		case F_FZZ:
			fwrite("FZZ", sizeof(char), 3, fp);
			break;

		case F_FCB: {
			fwrite("FCB", sizeof(char), 3, fp);
					} break;

		case F_FCZ: {
			fwrite("FCZ", sizeof(char), 3, fp);
					} break;

		default:
			assert(0);
			return false;
	}

	return true;
}

bool
DataFileInfo::CheckHead(FILE *fp)
{
	char magic[4];

	int num = fread(magic, sizeof(char), 3, fp);
	if (num < 3) return false;
	magic[3] = 0;

	if (_type == F_FCB && !strcmp("FCB", magic))
		return true;

	if (_type == F_FCZ && !strcmp("FCZ", magic))
		return true;

	if (_type == F_FZZ && !strcmp("FZZ", magic))
		return true;

	return false;
}

bool
DataFileInfo::CheckHead(wistream &is)
{
	TCHAR magic[4];

	is >> magic;
	if (_type == F_FCT && !wcscmp(L"FCT", magic))
		return true;

	return false;
}

void
FCTInfo::Load(FILE *fp)
{
// 	matrix4f dummy;
// 
// 	ModelInfo *mdl = new ModelInfo(fp, dummy);
// 	_mdls.push_back(mdl);
// 	_box = mdl->GetBox();

	size_t num;
	fread(&num, sizeof(size_t), 1, fp);

	for (size_t i=0; i<num; i++) {
		bool empty;
		fread(&empty, sizeof(bool), 1, fp);
		if (!empty) {
			_mdls.push_back(new ModelInfo(fp));
		} else
			_mdls.push_back(NULL);
	}

	_box.Load(fp);
	_center.Load(fp);
	fread(&_sc, sizeof(double), 1, fp);
}

extern void SetCenter(vec3f &c);
extern void SetScale(double sc);

FCTInfo::~FCTInfo()
{
	for (ModelPtrArray::iterator it=_mdls.begin(); it!=_mdls.end(); it++)
		delete (*it);

	_mdls.clear();
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

void
FCTInfo::DisplayEdge(bool)
{
}

void
FCTInfo::DisplayVtx(bool)
{
}

bool
FCTInfo::PointInShadow(vec3f &pt, vec3f &sun, unsigned int pid)
{
	vec3f pnt = pt;

	for (ModelPtrArray::iterator it=_mdls.begin(); it!=_mdls.end(); it++) {
		if ((*it) == NULL)
			continue;

		if (!vec_is_vis.empty() && !vec_is_vis[it - _mdls.begin()])
			continue;

		if ((*it)->GetID() == pid)
			continue;

		if ((*it)->PointInShadow(pnt, sun))
			return true;
	}

	return false;
}

void
FCTInfo::DisplayFace(bool bAnimate)
{
	int count = 0;
	for (ModelPtrArray::iterator it=_mdls.begin(); it!=_mdls.end(); it++) {
		if ((*it) == NULL)
		{
			continue;
		}
		if (!vec_is_vis.empty() && !vec_is_vis[it - _mdls.begin()])
		{
			continue;
		}

		/*
		float *mat = predefined_mat[(count++)%15];

		glxSetMaterial(mat[0], mat[1], mat[2], 0.f);
		glxEnableMaterial();
*/

		(*it)->DisplayFace(bAnimate, false);
	}

}

void
FCTInfo::DisplayFace(bool bAnimate, MatInfo *matInfo, unsigned int *textureID, unsigned int *normalMapID)
{
	int count = 0;
	for (ModelPtrArray::iterator it = _mdls.begin(); it != _mdls.end(); it++) {
		if ((*it) == NULL)
		{
			continue;
		}
		if (!vec_is_vis.empty() && !vec_is_vis[it - _mdls.begin()])
		{
			continue;
		}

		float *mat = predefined_mat[(count++)%15];
		glxSetMaterial(mat[0], mat[1], mat[2], 0.f);
		glxEnableMaterial();

		ModelInfo *mdl = *it;

		if (mdl->IsUniformColor()) {
			FaceInfo *fac = mdl->GetFace0();
			if (fac) {
				COLORREF color;
				fac->GetColor(color);//这color貌似没有用上！
								//*************************************************
								//在下面的代码中使用material的属性
				GET_GLERROR
					
				if (color != 0) {
					glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
					GET_GLERROR

					glxSetMat(matInfo, textureID, normalMapID, color);
					GET_GLERROR

					glxEnableMat();
				}

				GET_GLERROR

				bool has_tex = (color == 0) ? false : (textureID[color - 1] == 0 ? false : true);
				mdl->DisplayFace(bAnimate, has_tex);
				GET_GLERROR

				if (color != 0) {
					glPopAttrib();
				}
			}
		}
		else {
			GET_GLERROR
				mdl->DisplayFace(matInfo, bAnimate);
			GET_GLERROR
		}
	}
}

void
FCTInfo::DisplayAFace(itemID &id)
{
	_mdls[id._pid]->DisplayFace(false, id._id, false);
}

/*
void
FCTInfo::DisplayItem(enumItemType ty, itemID &id)
{
	_mdls[id._pid]->DisplayFace(false, id._id);
}
*/

void
FCTInfo::GetNames(AsmNameArray &names)
{
	int idx=0;
	TCHAR buffer[128];

	for (ModelPtrArray::iterator it=_mdls.begin(); it!=_mdls.end(); it++)
	{
		wsprintf(buffer, L"部件 #%d", idx++);
		names.push_back(AsmName(wstring(buffer), NULL, 0));
	}
}

void
FCTInfo::GetModelInfo(int &fNum, int &vNum, int &tNum)
{
	for (ModelPtrArray::iterator it=_mdls.begin(); it!=_mdls.end(); it++)
		(*it)->GetModelInfo(fNum, vNum, tNum);
}

bool
FCTInfo::AddAnimateRotate(unsigned int id, line &ln, float angle)
{
	matrix4f t1, t2, t3;
	t1.set_translate(-ln._org);
	t2.set_rotate(ln._dir, angle);
	t3.set_translate(ln._org);

	matrix4f trf = t3*t2*t1;

	_mdls[id]->SetTransf(trf);

	return true;
}

void
FCTInfo::SetAnimate(unsigned int id, matrix4f &trf)
{
	_mdls[id]->SetTransf(trf);
}

matrix4f
FCTInfo::GetAnimate( unsigned int id )
{
	return _mdls[id]->GetTransf();
}

void FCTInfo::SetLocalAnimate( unsigned int id, matrix4f &trf )
{
	_mdls[id]->SetTransfLocal(trf);
}

matrix4f FCTInfo::GetLocalAnimate( unsigned int id )
{
	return _mdls[id]->GetTransfLocal();
}

void 
FCTInfo::SetLocalAnimateBase( unsigned int id, matrix4f &trf )
{
	_mdls[id]->SetTransfLocalBase(trf);
}

matrix4f FCTInfo::GetLocalAnimateBase( unsigned int id )
{
	return _mdls[id]->GetTransfLocalBase();
}

void
FCTInfo::SetLocalAnimateOnBase( unsigned int id, matrix4f &trf )
{
	_mdls[id]->SetTransfLocal(trf * _mdls[id]->GetTransfLocalBase());
}

void 
FCTInfo::AddAnimatePre( unsigned int id, matrix4f &trf )
{
	_mdls[id]->SetTransf(trf * _mdls[id]->GetTransfLocal());
}

bool
FCTInfo::AddAnimateTranslate(unsigned int id, vec3f &offset)
{
	matrix4f t;
	t.set_translate(offset);

	ModelInfo *ptr = _mdls[id];
	if (NULL != ptr)
	{
		ptr->SetTransf(t);
	}
	return true;
}

bool
FCTInfo::Save(FILE *fp, DataFileTypes mode)
{
	return false;
}

void FCTInfo::ResetAnimations()
{
	for (int i = 0; i < _mdls.size(); i++)
	{
		_mdls[i]->SetTransf(matrix4f());
	}
}

float
FCTInfo::GetShadowRatio(itemID &id, vec3f &sun)
{
	return _mdls[id._pid]->GetShadowRatio(id._id, sun, this, id._pid);
}

float
FCTInfo::GetFaceArea(itemID &id)
{
	if (NULL == _mdls[id._pid])
		return 0.f;

	SetScale(_sc);
	return _mdls[id._pid]->GetFaceArea(id._id);
}

bool
FCTInfo::GetFacePlane(itemID &id, vec3f &org, vec3f &norm)
{
	if (NULL == _mdls[id._pid])
		return false;

	return _mdls[id._pid]->GetFacePlane(id._id, org, norm);
}

ACTInfo::ACTInfo(wistream &is, DataFileTypes ftype, bool autoFit)
{
	assert(ftype == F_ACT || ftype == F_FCT);
	//Load(is, ftype, autoFit);
}

ACTInfo::ACTInfo(FILE *fp, DataFileTypes ftype)
{
	if (ftype == F_ACT) {
		LoadAsm(fp);
	} else
		Load(fp);
}

void
ACTInfo::GetNames(AsmNameArray &lst)
{
	if (_tree.empty())
		FCTInfo::GetNames(lst);
	else {
		_tree._root->GetNames(lst);
	}
}

BOX
ACTInfo::GetBox()
{
	if (_tree.empty())
		return FCTInfo::GetBox();
	else {
		return _tree.getBox();
	}
}

AssembleNode *
ACTInfo::GetRootNode()
{
	return _tree.root();
}

class less_vec3f
{
public:
	bool operator()(const vec3f& lhd, const vec3f& rhd)
	{
		return lhd.y < rhd.y;
	}
};

std::vector<vec3f>
ACTInfo::GetFourLeg()
{
	std::vector<vec3f> vec;

	std::priority_queue<vec3f, std::vector<vec3f>, less_vec3f> q;

	int qn = 0;
	int i;
	for (i = 0; i < _mdls.size(); i++)
	{
		if (!vec_is_vis.empty() && !vec_is_vis[i])
		{
			continue;
		}
		q.push(_mdls[i]->GetLowestVertex());
		if (++qn == 4)
		{
			break;
		}
	}
	if (qn < 4)
	{
		return vec;
	}

	for (; i < _mdls.size(); i++)
	{
		if (!vec_is_vis.empty() && !vec_is_vis[i])
		{
			continue;
		}
		vec3f v = _mdls[i]->GetLowestVertex();
		if (v.y < q.top().y)
		{
			q.pop();
			q.push(v);
		}
	}

	while (!q.empty())
	{
		vec.push_back(q.top());
		q.pop();
	}

	return vec;
}

wstring
ACTInfo::GetRootName()
{
	if (_tree.empty())
		return L"零件";
	else {
		return L"装配体";
	}
}

void
ACTInfo::GetNames(AssembleNode *node, AsmNameArray &lst)
{
	node->GetNames(lst);
}

void
ACTInfo::LoadAsm(FILE *fp)
{
	_tree.load(fp);
	_tree.rescan(_mdls);
}

void
ACTInfo::DisplayAFace(itemID &id)
{
	FCTInfo::DisplayAFace(id);
}

void
ACTInfo::DisplayFace(bool bAnimate)
{
	FCTInfo::DisplayFace(bAnimate);
}

void
ACTInfo::DisplayMatFace(MatInfo *mat, unsigned int *textureID, unsigned int *normalMapID, bool bAnimate)
{
	if (_tree.empty() || mat == NULL)
		FCTInfo::DisplayFace(bAnimate);
	else 
		FCTInfo::DisplayFace(bAnimate, mat, textureID, normalMapID);
		//_tree.displayFace(mat, _mdls, bAnimate);
}

void
ACTInfo::DisplayEdge(bool bAnimate)
{
	if (_tree.empty())
		FCTInfo::DisplayEdge(bAnimate);
	else
		_tree.displayEdge(_mdls, bAnimate);
}

void
ACTInfo::DisplayVtx(bool bAnimate)
{
	if (_tree.empty())
		FCTInfo::DisplayVtx(bAnimate);
	else
		_tree.displayVtx(_mdls, bAnimate);
}

// C字符转宽字符
std::wstring ctow(const char* src){
	std::vector<wchar_t> dest(512);
	int i = mbstowcs(&dest[0], src, strlen(src));
	return std::wstring(&dest[0]);
}

AssembleNode::AssembleNode(FILE *fp, AssembleNodeArray &array)
{
	_box.Load(fp);
	fread(&_flag, sizeof(bool), 1, fp);
	fread(&_sid, sizeof(unsigned int), 1, fp);

	unsigned int len;
	fread(&len, sizeof(unsigned int), 1, fp);
	wchar_t *tmp = new wchar_t[len+1];
	fread(tmp, sizeof(wchar_t), len, fp);
	tmp[len] = 0;
	_name = wstring(tmp);
	delete [] tmp;

	_trf.Load(fp);

	_id = array.size();
	array.push_back(this);

	size_t num;
	fread(&num, sizeof(size_t), 1, fp);
	for (size_t i=0; i<num; i++) {
		_child.push_back(new AssembleNode(fp, array));
	}
}

//-- 读二进制
AssembleNode::AssembleNode(FILE *fp)
{
	_box.Load(fp);
	fread(&_flag, sizeof(bool), 1, fp);
	fread(&_sid, sizeof(unsigned int), 1, fp);

	unsigned int len;
	fread(&len, sizeof(unsigned int), 1, fp);
	wchar_t *tmp = new wchar_t[len+1];
	fread(tmp, sizeof(wchar_t), len, fp);
	tmp[len] = 0;
	_name = wstring(tmp);
	delete [] tmp;

	_trf.Load(fp);

	size_t num;
	fread(&num, sizeof(size_t), 1, fp);
	for (size_t i=0; i<num; i++) {
		_child.push_back(new AssembleNode(fp));
	}
}

AssembleNode *
AssembleNode::Load(FILE *fp, char flag, int level/* = 0*/)
{
	char buffer[1024];
	char name[512];

	AssembleNode *node = new AssembleNode();
	node->_name = wstring(L"root");
	node->_sid = 0;
	
	vector<matrix4f> currentTrf;
	while (fgets(buffer, 1024, fp) != NULL) {
		char *idx = strstr(buffer, "url \"");
		if (idx != NULL) {
			idx += strlen("url \"");
			sscanf(idx, "%s", name);

			if (NULL != (idx = strstr(name, "_prt.wrl"))) {
				idx += strlen("_prt.wrl");
				*idx = 0;

				AssembleNode *child = new AssembleNode();
				child->_name = ctow(name);
				child->_sid = -1;

				for (int i=0; i<currentTrf.size(); i++)
					child->_trf *= currentTrf[currentTrf.size()-1-i];;

				node->AddChild(child);
				currentTrf.clear();
			}
			if (NULL != (idx = strstr(name, "_asm.wrl"))) {
				idx += strlen("_asm.wrl");
				*idx = 0;

				FILE *fpNew = fopen(name, "rt");
				if (fpNew == NULL)
					continue;

				AssembleNode *child = Load(fpNew, 0, level+1);
				fclose(fpNew);

				for (int i=0; i<currentTrf.size(); i++)
					child->_trf *= currentTrf[currentTrf.size()-1-i];;

				node->AddChild(child);
				currentTrf.clear();
			}
		} 

		idx = strstr(buffer, "rotation ");
		if (idx != NULL) {
			float x, y, z, r;
			sscanf(idx+strlen("rotation "), "%g%g%g%g", &x, &y, &z, &r);
			matrix4f rot;
			rot.set_rotate(vec3f(x, y, z), r);
			currentTrf.push_back( rot);
		}

		idx = strstr(buffer, "translation ");
		if (idx != NULL) {
			float x, y, z;
			sscanf(idx+strlen("translation "), "%g%g%g", &x, &y, &z);
			matrix4f move;
			move.set_translate(vec3f(x, y, z)*VTX_SCALE);
			currentTrf.push_back(move);
		}

		
	}

	return node;
}

AssembleNode::~AssembleNode()
{
	for (AssembleNodeArray::iterator it=_child.begin(); it!=_child.end(); it++)
		delete *it;
}

void
AssembleNode::AddChild(AssembleNode *node)
{
	_child.push_back(node);
}

void
AssembleNode::GetNames(AsmNameArray &lst)
{
	for (AssembleNodeArray::iterator it=_child.begin(); it!=_child.end(); it++) {
		lst.push_back(AsmName((*it)->_name, *it, (*it)->_child.size()));
	}
}

static bool ReplaceExt(wstring &name)
{
	wstring::size_type pos = name.rfind(wstring(L".PRT"));
	if (pos == wstring::npos) {
		pos = name.rfind(wstring(L".prt"));
		if (pos == wstring::npos)
			return false;
	}

	name.replace(pos, wstring(L".PRT").size(), wstring(L".fcz"));
	return true;
}

void
AssembleNode::Rescan(ModelPtrArray &mdls, matrix4f &trf)
{
	_box.empty();
	for (AssembleNodeArray::iterator it=_child.begin(); it!=_child.end(); it++) {
		if ((*it)->_child.size() > 0) {
			(*it)->Rescan(mdls, trf*(*it)->_trf);
		}
		else {
			(*it)->_sid = mdls.size();
			wstring name = (*it)->_name;
//			ReplaceExt(name);

			FILE *fp = _wfopen(name.c_str(), L"rt");
			if (NULL == fp)
			{
				mdls.push_back(NULL);
			} 
			else
			{
				ModelInfo *model = new ModelInfo(fp, trf*(*it)->_trf);
				fclose(fp);

				model->SetID(mdls.size());

				mdls.push_back(model);
				(*it)->_box = model->GetBox();
			}
		}
		_box += (*it)->_box;
	}
}

void
AssembleNode::GetChildren(const ModelPtrArray &all, ModelPtrArray &mdls)
{
	if (_child.size() > 0)
		for (AssembleNodeArray::iterator it=_child.begin(); it!=_child.end(); it++)
			(*it)->GetChildren(all, mdls);
	else
		mdls.push_back(all[_sid]);
}

void 
AssembleNode::DisplayFace(MatInfo *matr, ModelPtrArray &mdls, bool bAnimate)
{
	for (AssembleNodeArray::iterator it=_child.begin(); it!=_child.end(); it++) {
		if ((*it)->_flag == 0) //invisible
			continue;

		if ((*it)->_child.size() > 0) { // draw sub parts
			(*it)->DisplayFace(matr, mdls, bAnimate);
			continue;
		} 

		unsigned int id = (*it)->_sid;
		if (NULL == mdls[id]) // empty part
			continue;

		float *mat = predefined_mat[id%15];
		glxSetMaterial(mat[0], mat[1], mat[2], 0.f);
		glxEnableMaterial();

		mdls[id]->DisplayFace(matr, bAnimate);
		//mdls[id]->DisplayFace(bAnimate);
	}
}

void
AssembleNode::DisplayFace(ModelPtrArray &mdls, bool bAnimate)
{
	for (AssembleNodeArray::iterator it=_child.begin(); it!=_child.end(); it++) {
		if ((*it)->_flag == 0) //invisible
			continue;

		if ((*it)->_child.size() > 0) {
			(*it)->DisplayFace(mdls, bAnimate);
		} else {
			unsigned int id = (*it)->_sid;

			if (NULL == mdls[id])
			{
				continue;
			}

			float *mat = predefined_mat[id%15];
			glxSetMaterial(mat[0], mat[1], mat[2], 0.f);
			glxEnableMaterial();
			mdls[id]->DisplayFace(bAnimate, false);
		}
	}
}

void
AssembleNode::DisplayEdge(ModelPtrArray &mdls, bool bAnimate)
{
}

void
AssembleNode::DisplayVtx(ModelPtrArray &mdls, bool bAnimate)
{
}

void
AssembleNode::setChildVisible(bool flag)
{
	for (AssembleNodeArray::iterator it=_child.begin(); it!=_child.end(); it++) {
		(*it)->setVisible(flag);
	}
}

void AssembleNode::getMdlIds( vector<unsigned int>& mdlIds ) const
{
	if (_child.size() == 0)
	{
		mdlIds.push_back(_sid);
	}
	for (AssembleNodeArray::const_iterator itr = _child.begin(); itr != _child.end(); itr++)
	{
		(*itr)->getMdlIds(mdlIds);
	}
}

AssembleTree::~AssembleTree()
{
	if (_root)
		delete _root;
}

void
AssembleTree::load(FILE *fp)
{
	_root = AssembleNode::Load(fp, 0);
}

void
AssembleTree::rescan(ModelPtrArray &mdls)
{
	matrix4f trf;
	_root->Rescan(mdls, trf);
}

void
AssembleTree::displayFace(MatInfo *matr, ModelPtrArray &mdls, bool bAnimate)
{
	_root->DisplayFace(matr, mdls, bAnimate);
}

void
AssembleTree::displayFace(ModelPtrArray &mdls, bool bAnimate)
{
	_root->DisplayFace(mdls, bAnimate);
}

void
AssembleTree::displayEdge(ModelPtrArray &mdls, bool bAnimate)
{
	_root->DisplayEdge(mdls, bAnimate);
}

void
AssembleTree::displayVtx(ModelPtrArray &mdls, bool bAnimate)
{
	_root->DisplayVtx(mdls, bAnimate);
}

BOX
AssembleTree::getBox()
{
	return _root->getBox();
}