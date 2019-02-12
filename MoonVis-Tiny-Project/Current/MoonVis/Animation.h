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

#include <vector>
#include <fstream>
#include "line.h"
#include "matrix.h"

#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

class ACTInfo;
class DumpSoilAnm;
class DigSoilAnm;

typedef std::vector<unsigned int> VI;

// #ifndef MOON_VIS
// #define MOON_VIS
// #endif

/************************************************************************/
/* 动画依赖关系树                                                       */
/************************************************************************/
class AnmDepItem{
public:
	typedef std::vector<AnmDepItem*> PtrVec;
	typedef PtrVec::iterator Iter;
	typedef PtrVec::const_iterator ConstIter;

	unsigned int _asmId;
	VI _mdlIds;
	PtrVec _children;

	AnmDepItem()
		: _asmId(-1)
	{
	}
	AnmDepItem(unsigned int asmId, VI mdlIds = VI())
		: _asmId(asmId)
		, _mdlIds(mdlIds)
	{
	}
	AnmDepItem(wistream& is);
	~AnmDepItem()
	{
		clear();
	}

	//-- I/O
	friend wostream& operator << (wostream& os, const AnmDepItem& rhd);

	void delChild(const AnmDepItem* c);
	void addChild(AnmDepItem* c);
	bool includeAsm(unsigned int id);
	AnmDepItem* getItem(unsigned int asmId);
	unsigned int getParMdlId(unsigned int asmId);
	void apply(ACTInfo* pAct, const matrix4f& trf) const;
	void reset(ACTInfo* pAct) const;
	const VI& getMdlIds() const { return _mdlIds; }

	void clear()
	{
		for (PtrVec::iterator itr = _children.begin(); itr != _children.end(); itr++)
		{
			if (NULL != *itr)
			{
				delete *itr;
			}
		}
	}
};
typedef std::vector<AnmDepItem> AnmDepItemVec;

class AnmDepInfo : public std::vector<AnmDepItem*> 
{
public:
	AnmDepInfo();
	AnmDepInfo(wistream& is);
	~AnmDepInfo();

	//-- I/O
	friend wostream& operator << (wostream& os, const AnmDepInfo& rhd);

	//-- interface
	bool includeAsm(unsigned int id);
	unsigned int getParMdlId(unsigned int asmId);
	AnmDepItem* getItem(unsigned int asmId);
	void deleteItem(AnmDepItem* pAnmDep);
	void apply(ACTInfo* pAct) const;
	void reset(ACTInfo* pAct) const;
};

/************************************************************************/
/* 旋转轴                                                               */
/************************************************************************/
class AnmRotAxis
{
public:
	unsigned int _asmId;
	VI _mdlIds;
	line _ln;

public:
	AnmRotAxis(unsigned int asmId, const VI& mdlIds, const line& ln)
		: _asmId(asmId)
		, _mdlIds(mdlIds)
		, _ln(ln)
	{
	}
	AnmRotAxis(wistream& is);

	//-- I/O
	friend wostream& operator << (wostream& os, const AnmRotAxis& rhd);
};
typedef AnmRotAxis* AnmRotAxisPtr;
typedef vector<AnmRotAxisPtr> AnmRotAxisPtrVec;

class AnmRotAxisInfo : public AnmRotAxisPtrVec
{
public:
	AnmRotAxisInfo();
	AnmRotAxisInfo(wistream& is);

	//-- I/O
	friend wostream& operator << (wostream& os, const AnmRotAxisInfo& rhd);

	~AnmRotAxisInfo()
	{
		for (iterator itr = begin(); itr != end(); itr++)
		{
			if (NULL != *itr)
			{
				delete *itr;
			}
		}
	}
	bool includeAsm(unsigned int id);
	line getAxis(unsigned int id);
	void deleteItem(unsigned int index);
};
/************************************************************************/
/* 动画相关behaviors                                                    */
/************************************************************************/
class AnimationItem;
//-- apply behavior
class AnmApplyBehavior
{
protected:
	AnimationItem* _pOwner;
public:
	AnmApplyBehavior(AnimationItem* pOwner)
		: _pOwner(pOwner)
	{

	}
	virtual void apply(ACTInfo* pAct) = 0;
};

class AnmApplyTrst : public AnmApplyBehavior
{
public:
	AnmApplyTrst(AnimationItem* pOwner)
		: AnmApplyBehavior(pOwner)
	{

	}
	virtual void apply(ACTInfo* pAct);
};

//-- end behavior
class AnmEndBehavior
{
protected:
	AnimationItem* _pOwner;
public:
	AnmEndBehavior(AnimationItem* pOwner)
		: _pOwner(pOwner)
	{

	}
	virtual void end(ACTInfo* pAct) = 0;
};

class AnmEndTrsf : public AnmEndBehavior
{
public:
	AnmEndTrsf(AnimationItem* pOwner)
		: AnmEndBehavior(pOwner)
	{

	}
	virtual void end(ACTInfo* pAct);
};

/************************************************************************/
/* 动画单元基类                                                         */
/************************************************************************/
class AnimationItem
{
public:
	enum Type
	{
		TYPE_INVALID,

		TRANSLATE,
		ROTATE,
		SCREW,
		VERCONS,
		GLOW,
		DUMPSOIL,
		DRILLEARTH,
		FOLLOW,
		PROJECTILE,

		TYPE_END
	};

protected:
	//-- properties
	Type _type;
	unsigned int _asmId;
	VI _mdlIds;
	float _duration;

	//-- status
	bool _active;
	bool _end;
	float _playedTime;

	//-- behaviors
	AnmApplyBehavior* _pApply;
	AnmEndBehavior* _pEnd;

protected:
	virtual void makeItVirtual() = 0;

public:
	//-- constructors & destructor
	AnimationItem(Type type, unsigned int asmId = -1, const VI& mdlIds = VI(), float duration = 60.f);
	AnimationItem(Type type, wistream& is);

	//-- copy
	virtual AnimationItem* copy() const = 0;

	//-- I/O
	virtual void save(wostream& os) const;
	friend wostream& operator << (wostream& os, const AnimationItem* rhd);

	//-- interface
	Type getType() const { return _type; }
	unsigned int getAsmId() const { return _asmId; }
	VI getMdlIds() const { return _mdlIds; }
	float getDuration() const { return _duration; }

	void setDuration(float duration) { _duration = duration; }

	bool isActive() const { return _active; }
	bool isEnd() const { return _end; }

	virtual void start(ACTInfo* pAct = NULL, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL);
	virtual void end(ACTInfo* pAct);
	float timeOn(float dt);
	virtual bool update(float dt);
	virtual matrix4f getMatrix() const;
	virtual void apply(ACTInfo* pAct) const;
	virtual void draw(ACTInfo* pAct = NULL);

	//-- friends
	friend class AnmApplyBehavior;
	friend class AnmApplyTrst;
	friend class AnmEndBehavior;
	friend class AnmEndTrsf;
};
typedef vector<AnimationItem*> AnmItemPtrVec;

/************************************************************************/
/* 平移动画                                                             */
/************************************************************************/
class AnmTransItem : public AnimationItem
{
private:
	//-- data
	vec3f _dir;
	float _init_vel;
	float _acc;
	float _limit;

	//-- status
	float _displace;
	float _vel;

protected:
	void makeItVirtual() {}

public:
	//-- constructors & destructor
	//AnmTransItem();
	AnmTransItem(wistream& is);
	AnmTransItem(		
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		const vec3f& dir = vec3f(),
		float init_vel = 0.0f,
		float acc = 2.11f,
		float limit = 0.f
		);
	AnmTransItem(const AnmTransItem& rhd);

	//-- I/O
	virtual void save(wostream& os) const;
	friend wostream& operator << (wostream& os, const AnmTransItem& rhd);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- interface
	const vec3f& getDir() const { return _dir; }
	float getVel() const { return _init_vel; }
	float getAcc() const { return _acc; }
	float getLimit() const { return _limit; }

	void setDir(const vec3f& dir) { _dir = dir; }
	void setVel(float vel) { _init_vel = vel; }
	void setAcc(float acc) { _acc = acc; }
	void setLimit(float limit) { _limit = limit; }

	virtual void start(ACTInfo* pAct = NULL, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL);
	virtual bool update(float dt);
	virtual matrix4f getMatrix() const;

	//-- friends
	friend class AnmApplyBehavior;
	friend class AnmApplyTrst;
	friend class AnmEndBehavior;
	friend class AnmEndTrsf;
};

/************************************************************************/
/* 平抛动画                                                             */
/************************************************************************/
class AnmProjectileItem : public AnimationItem
{
private:
	//-- data
	vec3f _dir1;
	float _init_vel1;
	float _acc1;
	float _limit1;
	vec3f _dir2;
	float _init_vel2;
	float _acc2;
	float _limit2;

	//-- status
	float _displace1;
	float _vel1;
	float _displace2;
	float _vel2;

protected:
	void makeItVirtual() {}

public:
	//-- constructors & destructor
	AnmProjectileItem(wistream& is);
	AnmProjectileItem(		
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		const vec3f& dir1 = vec3f(),
		float init_vel1 = 1.0f,
		float acc1 = 0.0f,
		float limit1 = 0.f,
		const vec3f& dir2 = vec3f(),
		float init_vel2 = 0.0f,
		float acc2 = 2.11f,
		float limit2 = 0.f
		);
	AnmProjectileItem(const AnmProjectileItem& rhd);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- I/O
	virtual void save(wostream& os) const;
	friend wostream& operator << (wostream& os, const AnmProjectileItem& rhd);

	//-- interface
	const vec3f& getDir1() const { return _dir1; }
	float getVel1() const { return _init_vel1; }
	float getAcc1() const { return _acc1; }
	float getLimit1() const { return _limit1; }
	const vec3f& getDir2() const { return _dir2; }
	float getVel2() const { return _init_vel2; }
	float getAcc2() const { return _acc2; }
	float getLimit2() const { return _limit2; }

	void setDir1(const vec3f& dir) { _dir1 = dir; }
	void setVel1(float vel) { _init_vel1 = vel; }
	void setAcc1(float acc) { _acc1 = acc; }
	void setLimit1(float limit) { _limit1 = limit; }
	void setDir2(const vec3f& dir) { _dir2 = dir; }
	void setVel2(float vel) { _init_vel2 = vel; }
	void setAcc2(float acc) { _acc2 = acc; }
	void setLimit2(float limit) { _limit2 = limit; }

	virtual void start(ACTInfo* pAct = NULL, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL);
	virtual bool update(float dt);
	virtual matrix4f getMatrix() const;

	//-- friends
	friend class AnmApplyBehavior;
	friend class AnmApplyTrst;
	friend class AnmEndBehavior;
	friend class AnmEndTrsf;
};

/************************************************************************/
/* 旋转动画                                                             */
/************************************************************************/
class AnmRotItem : public AnimationItem
{
private:
	//-- data
	line _axis;
	float _ang_vel;
	float _limit;

	//-- status
	float _angle;

protected:
	void makeItVirtual() {}

public:
	//-- constructors & destructor
	AnmRotItem(
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		const line& axis = line(),
		float ang_vel = 30.0f,
		float limit = 0.f
		);
	AnmRotItem(wistream& is);
	AnmRotItem(const AnmRotItem& rhd);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- I/O
	virtual void save(wostream& os) const;
	friend wostream& operator << (wostream& os, const AnmRotItem& rhd);
	

	//-- interface
	const line& getAxis() const { return _axis; }
	float getAngVel() const { return _ang_vel; }
	float getLimit() const { return _limit; }

	void setAxis(const line& axis) { _axis = axis; }
	void setAngVel(float vel) { _ang_vel = vel; }
	void setLimit(float limit) { _limit = limit; }

	virtual void start(ACTInfo* pAct = NULL, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL);
	virtual bool update(float dt);
	virtual matrix4f getMatrix() const;

	//-- friends
	friend class AnmApplyBehavior;
	friend class AnmApplyTrst;
	friend class AnmEndBehavior;
	friend class AnmEndTrsf;
};

/************************************************************************/
/* 螺旋动画                                                             */
/************************************************************************/
class AnmScrewItem : public AnimationItem
{
private:
	//-- data
	line _axis;
	float _ang_vel;
	float _vel;
	float _limit;

	//-- status
	float _angle;
	float _displace;

protected:
	void makeItVirtual() {}

public:
	//-- constructors & destructor
	AnmScrewItem(
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		const line& axis = line(),
		float ang_vel = 30.0f,
		float limit = 0.f
		);
	AnmScrewItem(wistream& is);
	AnmScrewItem(const AnmScrewItem& rhd);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- I/O
	virtual void save(wostream& os) const;
	friend wostream& operator << (wostream& os, const AnmScrewItem& rhd);

	//-- interface
	const line& getAxis() const { return _axis; }
	float getVel() const { return _vel; }
	float getAngVel() const { return _ang_vel; }
	float getLimit() const { return _limit; }

	void setAxis(const line& axis) { _axis = axis; }
	void setVel(float vel) { _vel = vel; }
	void setAngVel(float vel) { _ang_vel = vel; }
	void setLimit(float limit) { _limit = limit; }

	virtual void start(ACTInfo* pAct = NULL, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL);
	virtual bool update(float dt);
	virtual matrix4f getMatrix() const;
	
	//-- friends
	friend class AnmApplyBehavior;
	friend class AnmApplyTrst;
	friend class AnmEndBehavior;
	friend class AnmEndTrsf;
};

/************************************************************************/
/* 倒土动画                                                             */
/************************************************************************/
class AnmDumpSoilItem : public AnimationItem
{
private:
	//-- data
	line _axis;
	float _length;
	float _radius;
	float _soil_radius;
	unsigned long _color;

	DumpSoilAnm* _pDumpSoil;

protected:
	void makeItVirtual() {}

public:
	//-- constructors & destructor
	AnmDumpSoilItem(
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		const line& axis = line(),
		float length = 1.f,
		float radius = 0.01f,
		float soil_radius = 0.005f,
		unsigned long color = RGB(128, 128, 128)
		);
	AnmDumpSoilItem(wistream& is);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- I/O
	virtual void save(wostream& os) const;

	//-- interface
	const line& getAxis() const { return _axis; }
	float getLength() const { return _length; }
	float getRadius() const { return _radius; }
	float getSoilRadius() const { return _soil_radius; }
	unsigned long getColor() const { return _color; }

	void setAxis(const line& ln) { _axis = ln; }
	void setLength(float length) { _length = length; }
	void setRadius(float radius) { _radius = radius; }
	void setSoilRadius(float soil_radius) { _soil_radius = soil_radius; }
	void setColor(unsigned long color) { _color = color; }

	virtual void start(ACTInfo* pAct = NULL, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL);
	virtual bool update(float dt);
	virtual void draw(ACTInfo* pAct);
};

/************************************************************************/
/* 钻土土堆动画                                                         */
/************************************************************************/
class AnmDrillEarthItem : public AnimationItem
{
private:
	//-- data
	line _axis;
	float _radius;
	float _outer_radius;
	float _height;
	float _soil_radius;
	float _acc_rate;
	unsigned long _color;

	//-- copy
	virtual AnimationItem* copy() const;

	//-- state
	DigSoilAnm* _pDigSoil;
	vec3f _pos;
	vec3f _dir;

protected:
	void makeItVirtual() {}

public:
	//-- constructors & destructor
	AnmDrillEarthItem(
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		const line& ln = line(),
		float radius = 0.05f,
		float outer_radius = 0.1f,
		float height = 0.05f,
		float soil_radius = 0.005f,
		float acc_rate = 0.005f,
		unsigned long color = RGB(128, 128, 128)
		);
		
	AnmDrillEarthItem(wistream& is);


	//-- I/O
	virtual void save(wostream& os) const;

	//-- interface
	const line& getAxis() const { return _axis; }
	float getRadius() const { return _radius; }
	float getOuterRadius() const { return _outer_radius; }
	float getHeight() const { return _height; }
	float getSoilRadius() const { return _soil_radius; }
	float getAccRate() const { return _acc_rate; }
	unsigned long getColor() const { return _color; }

	void setAxis(const line& axis) { _axis = axis; }
	void setRadius(float radius) { _radius = radius; }
	void setOuterRadius(float outer_radius) { _outer_radius = outer_radius; }
	void setHeight(float height) { _height = height; }
	void setSoilRadius(float soil_radius) { _soil_radius = soil_radius; }
	void setAccRate(float acc_rate) { _acc_rate = acc_rate; }
	void setColor(unsigned long color) { _color = color; }

	virtual void start(ACTInfo* pAct = NULL, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL );
	virtual bool update(float dt);
	virtual void draw(ACTInfo* pAct = NULL);
};

/************************************************************************/
/* 竖直约束动画                                                         */
/************************************************************************/
class AnmVerConsItem : public AnimationItem
{
private:
	//-- data

	//-- state
	line _axis;
	matrix4f _trsfInit;
	unsigned int _asmIdPar; //-- 上一截杆
	VI _mdlIdsPar;
	line _axisPar;
	matrix4f _trsfParInit;

protected:
	void makeItVirtual() {}

public:
	//-- constructors & destructor
	AnmVerConsItem(
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f
		);

	AnmVerConsItem(wistream& is);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- I/O
	virtual void save(wostream& os) const;

	//-- interface
	virtual void start(ACTInfo* pAct, AnmDepInfo* pDeps, AnmRotAxisInfo* pAxis);
	virtual void end(ACTInfo* pAct);
	virtual void apply(ACTInfo* pAct);
};

/************************************************************************/
/* 发光动画                                                             */
/************************************************************************/
class AnmGlowItem : public AnimationItem
{
private:
	//-- data
	unsigned long _color;

protected:
	void makeItVirtual() {}
	
public:
	//-- consturctors & destructor
	AnmGlowItem(
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		unsigned long color = RGB(128, 128, 128)
		);
	AnmGlowItem(wistream& is);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- I/O
	virtual void save(wostream& os) const;

	//-- interface
	unsigned long getColor() const { return _color; }

	void setColor(unsigned long color) { _color = color; }
};

/************************************************************************/
/* 跟随动画                                                             */
/************************************************************************/
class AnmFollowItem : public AnimationItem
{
private:
	//-- data
	unsigned int _targetMdlId; // 所跟随的目标模型id

	//-- state
	matrix4f _matInit_inv; // matInit是动画开始时目标已有的变换，这一部分是动画主体不需要的，要消去

protected:
	void makeItVirtual() {}

public:
	//-- constuctors & destructor
	AnmFollowItem(
		unsigned int asmId = -1,
		const VI& mdlIds = VI(),
		float duration = 60.f,
		unsigned int targetMdlId = -1
		);
	AnmFollowItem(wistream& is);

	//-- copy
	virtual AnimationItem* copy() const;

	//-- I/O
	virtual void save(wostream& os) const;

	//-- interface
	virtual void start(ACTInfo* pAct, AnmDepInfo* pDeps = NULL, AnmRotAxisInfo* pAxis = NULL);
	virtual void apply(ACTInfo* pAct);

	//-- friends
	friend class AnmEndBehavior;
};

/************************************************************************/
/* 动画集                                                               */
/************************************************************************/
class AnimationInfo;
class AnimationSet : public AnmItemPtrVec
{
protected:
	//-- status
	bool _active;
	bool _end;

public:
	//-- constructors & destructor
	AnimationSet()
		: _active(false)
		, _end(false)
	{
	}
	AnimationSet(wistream& is);
	AnimationSet(const AnimationSet& rhd);


	~AnimationSet()
	{
		for (iterator itr = begin(); itr != end(); itr++)
		{
			delete *itr;
		}
	}

	//-- I/O
	void save(wostream& os) const;
	friend wostream& operator << (wostream& os, const AnimationSet& rhd);
	
	//-- interface
	bool includeAsm(unsigned int id);
	bool update(AnimationInfo* pAnms, ACTInfo* pAct, AnmDepInfo* pDeps, float dt, bool updateSoil = true);
	void start(ACTInfo* pAct, AnmDepInfo* pDeps, AnmRotAxisInfo* pAxis);
	void setEnd(ACTInfo* pAct, AnimationInfo* pAnms);
	void delItem(AnimationItem* pItem);
	void draw(ACTInfo* pAct);
};
typedef AnimationSet* AnimationSetPtr;
typedef std::vector<AnimationSetPtr> AnmSetPtrVec;

/************************************************************************/
/* 动画信息                                                             */
/************************************************************************/
class AnimationInfo : public AnmSetPtrVec
{
protected:
	ACTInfo* _pAct;
	AnmDepInfo _anmDeps;
	AnmRotAxisInfo _anmRotAxises;
	iterator _curItr;

	vector<AnmDrillEarthItem*> _drillEarthAnms;

public:
	AnimationInfo();
	AnimationInfo(wistream& is);
	~AnimationInfo()
	{
		for (iterator itr = begin(); itr != end(); itr++)
		{
			if (NULL != *itr)
			{
				delete *itr;
			}
		}
	}

	//-- I/O
	void save(wostream& os) const;
	friend wostream& operator << (wostream& os, const AnimationInfo& rhd);

	//-- interface
	//void getIt() { return _curItr; }
	AnmDepInfo* getDepInfo() { return &_anmDeps; }
	AnmRotAxisInfo* getRotAxisInfo() { return &_anmRotAxises; }
	void setAct(ACTInfo* pAct) { _pAct= pAct; }
	bool update(float dt);
	void updateOneByOne();
	void start();
	void setEnd();
	void delAnmSet(AnimationSetPtr pAnmSet);
	void playToSet(AnimationSetPtr pAnmSet);
	void draw();
	void addDigSoil(AnmDrillEarthItem* pItem) { _drillEarthAnms.push_back(pItem); }
};