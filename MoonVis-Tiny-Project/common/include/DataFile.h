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

// Author: Tang, Min tang_m@zju.edu.cn

#pragma once
#include <istream>
using namespace std;
#include "vec3f.h"
#include "box.h"
#include "line.h"
#include "matrix.h"
#include <string>



typedef enum {
	F_NULL = -1,
	F_FCT = 0,
	F_FCB,
	F_FCZ,
	F_ACT,
	F_ACB,
	F_ACZ,
	F_FZZ
} DataFileTypes;

class ModelInfo;
struct itemID;
struct MatInfo;

class DataFileInfo {
protected:
	DataFileTypes _type;
	float				 _version;

	bool CheckHead(FILE *);
	bool CheckHead(wistream &);

	bool WriteHead(FILE *, DataFileTypes);

public:
	DataFileInfo() : _type(F_NULL), _version(-1.f) {
		NULL;
	}

/*	virtual void loadT(wistream &is);
	virtual void saveT(wostream &os);
	virtual void loadB(FILE *);
	virtual void saveB(FILE *);
	virtual bool saveZ(FILE *);
	virtual bool loadZ(FILE *);*/
};

#include "AsmName.h"
class ModelPtrArray : public vector<ModelInfo *> {};
class AssembleNodeArray : public vector <AssembleNode *> {};


class AssembleNode {
	BOX _box;
	bool _flag;
	unsigned int _sid;
	unsigned int _id;
	wstring _name;
	matrix4f _trf;

	AssembleNodeArray _child;

public:
	AssembleNode() {
		_flag = true; // visible
		NULL;
	}

	AssembleNode(FILE *fp);
	AssembleNode(FILE *fp, AssembleNodeArray &array);

	static AssembleNode *Load(FILE *fp, char flag, int level = 0);

	~AssembleNode();

	void AddChild(AssembleNode *node);
	void GetNames(AsmNameArray &);
	void Rescan(ModelPtrArray &, matrix4f &);


	void DisplayFace(MatInfo *, ModelPtrArray &, bool bAnimate);
	void DisplayFace(ModelPtrArray &, bool bAnimate);
	void DisplayEdge(ModelPtrArray &, bool);
	void DisplayVtx(ModelPtrArray &, bool);
	BOX getBox() { return _box; }
	void GetChildren(const ModelPtrArray &, ModelPtrArray &);

	void toggleVisible() {_flag = !_flag;}
	bool getVisible() { return _flag; }
	void setChildVisible(bool);
	void setVisible(bool flag) { _flag = flag; }

	void getMdlIds(vector<unsigned int>& mdlIds) const;
};

class AssembleTree {
public:
	AssembleNode *_root;

	AssembleTree() {
		_root = NULL;
	}
	~AssembleTree();

	void load(FILE *fp);
	void loadBin(FILE *fp) {
		_root = new AssembleNode(fp);
	}

	bool empty() { return _root == NULL; }

	void rescan(ModelPtrArray &);
	//==
	void displayFace(MatInfo *, ModelPtrArray &, bool);
	void displayFace(ModelPtrArray &, bool bAnimate);
	void displayEdge(ModelPtrArray &, bool);
	void displayVtx(ModelPtrArray &, bool);
	BOX getBox();
	AssembleNode *root() { return _root; }

	void Load(FILE *fp, AssembleNodeArray &array) {
		bool flag;
		fread(&flag, sizeof(bool), 1, fp);

		if (flag)
			_root = new AssembleNode(fp, array);
		else
			_root = NULL;
	}
};
//#include "hit.h"

class FCTInfo : public DataFileInfo {
protected:
	ModelPtrArray _mdls;
	BOX _box;
	vec3f _center;
	double _sc;

public:
	FCTInfo() : _center(0.f, 0.f, 0.f), _sc(1.0) {
		NULL;
	}

	virtual ~FCTInfo();

	void Load(FILE *);
	bool Save(FILE *fp, DataFileTypes mode);

	const ModelPtrArray &GetModels() { return _mdls; }
	void GetNames(AsmNameArray &);
	virtual BOX GetBox() const {	return _box; }

	void GetModelInfo(int &fNum, int &vNum, int &tNum);
	ModelInfo* GetModelPtr(int i) { return _mdls[i]; }
	unsigned int GetModelCount() const { return _mdls.size(); }

	bool AddAnimateRotate(unsigned int, line &, float);
	bool AddAnimateTranslate(unsigned int, vec3f &);
	void SetAnimate(unsigned int id, matrix4f &);
	matrix4f GetAnimate(unsigned int id);
	void SetLocalAnimate(unsigned int id, matrix4f &);
	matrix4f GetLocalAnimate(unsigned int id);
	void SetLocalAnimateBase(unsigned int id, matrix4f &);
	matrix4f GetLocalAnimateBase(unsigned int id);
	void SetLocalAnimateOnBase(unsigned int id, matrix4f &);
	void AddAnimatePre(unsigned int id, matrix4f &);
	void ResetAnimations();

	bool GetFacePlane(itemID &, vec3f &org, vec3f &norm);
	float GetShadowRatio(itemID &, vec3f &sun);
	float GetFaceArea(itemID &);
	bool PointInShadow(vec3f &pt, vec3f &sun, unsigned int pid);

	void DisplayFace(bool, MatInfo *);
	void DisplayFace(bool bAnimate, MatInfo *matInfo, unsigned int *textureID, unsigned int *normalMapID);

	virtual void DisplayFace(bool);
	virtual void DisplayEdge(bool);
	virtual void DisplayVtx(bool);
	virtual void DisplayAFace(itemID&);
	//void DisplayItem(enumItemType, itemID&);

};

extern void tm_print(char *);

class ACTInfo : public FCTInfo {
	AssembleTree	_tree;
	AssembleNodeArray _array;

public:
	ACTInfo(wistream &, DataFileTypes, bool);
	ACTInfo(FILE *, DataFileTypes);
	ACTInfo(FILE *fp) {
		FCTInfo::Load(fp);
		//_tree.loadBin(fp);
		//AfxMessageBox(L"Here!");

		tm_print("ACTInfo enter ...\n");
		_tree.Load(fp, _array);
	}

	void LoadAsm(FILE *);
	void GetNames(AsmNameArray &);
	void GetNames(AssembleNode *, AsmNameArray &);
	wstring GetRootName();
	AssembleNode *GetRootNode();
	AssembleNode *GetNode(int id) { return _array[id]; }
	AssembleNode *GetNodeOfMdl(int id);

	std::vector<vec3f> GetFourLeg();

	//void DisplayMatFace(MatInfo *, bool bAnimate);
	virtual BOX GetBox();
	virtual void DisplayFace(bool);
	void DisplayMatFace(MatInfo * mat, unsigned int * textureID, unsigned int * normalMapID, bool bAnimate);
	virtual void DisplayEdge(bool);
	virtual void DisplayVtx(bool);
	virtual void DisplayAFace(itemID&);
};

class AsmNameArray;

struct itemID{
	unsigned int _pid;
	unsigned int _id;

	itemID() { Clear(); }
	itemID(unsigned pid, unsigned id) {
		_pid = pid;
		_id = id;
	}

	void Save(FILE *fp) {
		fwrite(&_pid, sizeof(unsigned int), 1, fp);
		fwrite(&_id, sizeof(unsigned int), 1, fp);
	}

	void Load(FILE *fp) {
		fread(&_pid, sizeof(unsigned int), 1, fp);
		fread(&_id, sizeof(unsigned int), 1, fp);
	}

	void Load(wistream &is) {
		is >> _pid;
		is >> _id;
	}

	friend wostream & operator << (wostream &os, itemID &);

	void Clear() {
		_pid = -1;
		_id = -1;
	}

	bool isEmpty() {
		return _pid == -1 && _id == -1;
	}

	bool operator == (const itemID &other) {
		return _pid == other._pid && _id == other._id;
	}

	bool operator != (const itemID &other) {
		return _pid != other._pid || _id != other._id;
	}
};

inline wostream &operator << (wostream &os, itemID &item)
{
	os << item._pid << endl;
	os << item._id << endl;
	return os;
}