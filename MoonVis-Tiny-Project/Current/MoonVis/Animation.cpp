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
#include "Animation.h"
#include "algorithm"
#include "DataFile.h"
#include "../Particles/SoilAnm.h"
#include <cmath>

void AnmDepItem::delChild( const AnmDepItem* c )
{
	Iter itr = std::find(_children.begin(), _children.end(), c);
	if (itr != _children.end())
	{
		_children.erase(itr);
	}
}

void AnmDepItem::addChild( AnmDepItem* c )
{
	_children.push_back(c);
}

bool AnmDepItem::includeAsm( unsigned int id )
{
	if (id == _asmId)
	{
		return true;
	}
	for (Iter itr = _children.begin(); itr != _children.end(); itr++)
	{
		if ((*itr)->includeAsm(id))
		{
			return true;
		}
	}
	return false;
}

AnmDepItem* AnmDepItem::getItem( unsigned int asmId )
{
	if (asmId == _asmId)
	{
		return this;
	}
	for (Iter itr = _children.begin(); itr != _children.end(); itr++)
	{
		AnmDepItem* p;
		if ((p = (*itr)->getItem(asmId)) != NULL)
		{
			return p;
		}
	}
	return NULL;
}

unsigned int AnmDepItem::getParMdlId( unsigned int asmId )
{
	for (Iter itr = _children.begin(); itr != _children.end(); itr++)
	{
		unsigned int par;
		if (asmId == (*itr)->_asmId)
		{
			return _asmId;
		}
		else if ((par = (*itr)->getParMdlId(asmId)) != -1)
		{
			return par;
		}
	}
	return -1;
}

wostream& operator<<( wostream& os, const AnmDepItem& rhd )
{
	os<<rhd._asmId<<endl;

	os<<rhd._mdlIds.size()<<endl;
	for (VI::const_iterator itr = rhd._mdlIds.begin(); itr != rhd._mdlIds.end(); itr++)
	{
		os<<*itr<<endl;
	}

	os<<rhd._children.size()<<endl;
	for (AnmDepItem::ConstIter itr = rhd._children.begin(); itr != rhd._children.end(); itr++)
	{
		os<<**itr<<endl;
	}

	return os;
}

AnmDepItem::AnmDepItem( wistream& is )
{
	is>>_asmId;
	int count;
	is>>count;
	for (int i = 0; i < count; i++)
	{
		int temp;
		is>>temp;
		_mdlIds.push_back(temp);
	}

	is>>count;
	for (int i = 0; i < count; i++)
	{
		_children.push_back(new AnmDepItem(is));
	}
}

void AnmDepItem::apply( ACTInfo* pAct, const matrix4f& trf ) const
{
	assert(!_mdlIds.empty());
	matrix4f mat = trf * pAct->GetLocalAnimate(_mdlIds[0]);
	for (VI::const_iterator itr = _mdlIds.begin(); itr != _mdlIds.end(); itr++)
	{
		pAct->SetAnimate(*itr, mat);
	}
	for (ConstIter itr = _children.begin(); itr != _children.end(); itr++)
	{
		(*itr)->apply(pAct, mat);
	}
}

void AnmDepItem::reset( ACTInfo* pAct ) const
{
	for (VI::const_iterator itr = _mdlIds.begin(); itr != _mdlIds.end(); itr++)
	{
		pAct->SetLocalAnimateOnBase(*itr, matrix4f());
	}
	for (ConstIter itr = _children.begin(); itr != _children.end(); itr++)
	{
		(*itr)->reset(pAct);
	}
}

bool AnmDepInfo::includeAsm( unsigned int id )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if ((*itr)->includeAsm(id))
		{
			return true;
		}
	}
	return false;
}

unsigned int AnmDepInfo::getParMdlId( unsigned int asmId )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		unsigned int par;
		if ((par = (*itr)->getParMdlId(asmId)) != -1)
		{
			return par;
		}
	}
	return -1;
}

void AnmDepInfo::deleteItem(AnmDepItem* pAnmDep)
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if (pAnmDep == *itr)
		{
			erase(itr);
			return;
		}
	}
}

AnmDepInfo::~AnmDepInfo()
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if (NULL == *itr)
		{
			delete *itr;
		}
	}
}

void AnmDepInfo::apply( ACTInfo* pAct ) const
{
	for (const_iterator itr = begin(); itr != end(); itr++)
	{
		(*itr)->apply(pAct, matrix4f());
	}
}

void AnmDepInfo::reset( ACTInfo* pAct ) const
{
	for (const_iterator itr = begin(); itr != end(); itr++)
	{
		(*itr)->reset(pAct);
	}
}

AnmDepInfo::AnmDepInfo()
: std::vector<AnmDepItem*>()
{

}

AnmDepInfo::AnmDepInfo( wistream& is )
: std::vector<AnmDepItem*>()
{
	int count;
	is>>count;
	for (int i = 0; i < count; i++)
	{
		push_back(new AnmDepItem(is));
	}
}

AnmDepItem* AnmDepInfo::getItem( unsigned int asmId )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		AnmDepItem* p = (*itr)->getItem(asmId);
		if (NULL != p)
		{
			return p;
		}
	}
	return NULL;
}

wostream& operator<<( wostream& os, const AnmDepInfo& rhd )
{
	os<<rhd.size()<<endl;
	for (AnmDepInfo::const_iterator itr = rhd.begin(); itr != rhd.end(); itr++)
	{
		os<<**itr<<endl;
	}

	return os;
}

/************************************************************************/
/* 旋转轴                                                               */
/************************************************************************/

AnmRotAxis::AnmRotAxis( wistream& is )
{
	is>>_asmId;
	int count;
	std::fstream file;
	file.open("../log.txt", std::ios::app);
	file << "asmId: " << _asmId << endl;
	is>>count;
	file << "rot cout: " << count << endl;
	for (int i = 0; i < count; i++)
	{
		int temp;
		is>>temp;
		_mdlIds.push_back(temp);
	}

	_ln.Load(is);
}

wostream& operator<<( wostream &os, const AnmRotAxis &rhd )
{
	os<<rhd._asmId<<endl;

	os<<rhd._mdlIds.size()<<endl;
	for (VI::const_iterator itr = rhd._mdlIds.begin(); itr != rhd._mdlIds.end(); itr++)
	{
		os<<*itr<<endl;
	}

	os<<rhd._ln<<endl;

	return os;
}

bool AnmRotAxisInfo::includeAsm( unsigned int id )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if (id == (*itr)->_asmId)
		{
			return true;
		}
	}
	return false;
}

void AnmRotAxisInfo::deleteItem( unsigned int index )
{
	assert(index < size());
	iterator itr = begin()+index;
	if (NULL != *itr)
	{
		delete *itr;
	}
	erase(itr);
}

line AnmRotAxisInfo::getAxis( unsigned int id )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if (id == (*itr)->_asmId)
		{
			return (*itr)->_ln;
		}
	}
	return line();
}

AnmRotAxisInfo::AnmRotAxisInfo()
: AnmRotAxisPtrVec()
{

}

AnmRotAxisInfo::AnmRotAxisInfo( wistream& is )
{
	int count;
	is>>count;
	std::fstream file;
	file.open("../log.txt", std::ios::app);
	file << "count: " << count << endl;
	for (int i = 0; i < count; i++)
	{
		push_back(new AnmRotAxis(is));
	}
}

wostream& operator<<( wostream& os, const AnmRotAxisInfo& rhd )
{
	os<<rhd.size()<<endl;
	for (AnmRotAxisInfo::const_iterator itr = rhd.begin(); itr != rhd.end(); itr++)
	{
		os<<**itr<<endl;
	}
	return os;
}
/************************************************************************/
/* 动画相关behaviors                                                    */
/************************************************************************/
void AnmApplyTrst::apply( ACTInfo* pAct )
{
	matrix4f trf = _pOwner->getMatrix();
	for (VI::const_iterator itr = _pOwner->_mdlIds.begin(); itr != _pOwner->_mdlIds.end(); itr++)
	{
		pAct->SetLocalAnimateOnBase(*itr, trf);
		pAct->SetAnimate(*itr, pAct->GetLocalAnimate(*itr));
	}
}

void AnmEndTrsf::end( ACTInfo* pAct )
{
	for (VI::const_iterator itr = _pOwner->_mdlIds.begin(); itr != _pOwner->_mdlIds.end(); itr++)
	{
		pAct->SetLocalAnimateBase(*itr, pAct->GetLocalAnimate(*itr));
	}
}

/************************************************************************/
/* 动画单元基类                                                         */
/************************************************************************/
matrix4f AnimationItem::getMatrix() const
{
	return matrix4f();
}

float AnimationItem::timeOn( float dt )
{
	if (!isActive() || isEnd())
	{
		return 0.f;
	}
	if (fabs(_duration) < FLT_MIN)
	{
		return dt;
	}

	if (_duration < _playedTime+dt)
	{
		_end = true;
		dt = _duration - _playedTime;
	}
	_playedTime += dt;

	return dt;
}

void AnimationItem::start(ACTInfo* pAct /* = NULL */, AnmDepInfo* pDeps /* = NULL */, AnmRotAxisInfo* pAxis /* = NULL */)
{
	_active = true;
	_playedTime = 0.f;
	_end = false;
}

void AnimationItem::end(ACTInfo* pAct)
{
	if (!isActive())
	{
		return;
	}

	_active = false;
	if (NULL != _pEnd)
	{
		_pEnd->end(pAct);
	}
}

bool AnimationItem::update( float dt )
{
	if (!isActive() || isEnd())
	{
		return true;
	}

	dt = timeOn(dt);

	return isEnd();
}

void AnimationItem::apply( ACTInfo* pAct ) const
{
	if (NULL != _pApply)
	{
		_pApply->apply(pAct);
	}
}

AnimationItem::AnimationItem( Type type, wistream& is )
: _type(type)
, _active(false)
, _end(false)
, _playedTime(0.f)
, _pApply(NULL)
, _pEnd(NULL)
{
	is>>_asmId;
	int count;
	is>>count;
	for (int i = 0; i < count; i++)
	{
		int temp;
		is>>temp;
		_mdlIds.push_back(temp);
	}
	is>>_duration;
}

AnimationItem::AnimationItem( Type type, unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 0.f*/ )
: _type(type)
, _asmId(asmId)
, _mdlIds(mdlIds)
, _duration(duration)
, _active(false)
, _end(false)
, _playedTime(0.f)
, _pApply(NULL)
, _pEnd(NULL)
{
}

void AnimationItem::save( wostream& os ) const
{
	os<<getType()<<endl;

	os<<_asmId<<endl;

	os<<_mdlIds.size()<<endl;
	for (VI::const_iterator itr = _mdlIds.begin(); itr != _mdlIds.end(); itr++)
	{
		os<<*itr<<endl;
	}

	os<<_duration<<endl;
}

void AnimationItem::draw(ACTInfo* pAct /* = NULL */)
{
	return;
}

wostream& operator<<( wostream& os, const AnimationItem* rhd )
{
	rhd->save(os);

	return os;
}


/************************************************************************/
/* 平移动画                                                             */
/************************************************************************/
// 计算平移变换矩阵
matrix4f AnmTransItem::getMatrix() const
{
	matrix4f mat;
	mat.set_translate(_dir*_displace*100);
	return mat;
}

//-- 返回true表示此步更新后动画终结
// 平移动画状态更新
bool AnmTransItem::update( float dt )
{
	if (!isActive() || isEnd())
	{
		return true;
	}

	dt = timeOn(dt);

	// 计算平移量
	_displace += _vel*dt + 0.5f * _acc*dt*dt;
	// 更新实时速度
	_vel += _acc*dt;

	// 限制位移量
	if (_limit > FLT_MIN && fabs(_displace) > _limit)
	{
		_displace = _limit*_displace/fabs(_displace);
		_end = true;
	}

	return isEnd();
}

void AnmTransItem::start(ACTInfo* pAct /* = NULL */, AnmDepInfo* pDeps /* = NULL */, AnmRotAxisInfo* pAxis /* = NULL */)
{
	AnimationItem::start();
	_displace = 0.f;
	_vel = _init_vel;
}


AnmTransItem::AnmTransItem( wistream& is )
: AnimationItem(TRANSLATE, is)
, _dir()
, _init_vel(0.f)
, _acc(0.f)
, _limit(0.f)
, _displace(0.f)
, _vel(0.f)
{
	is>>_dir;
#ifdef MOON_VIS
	vec3f tem = _dir;
	set_pt(_dir, tem);
#endif
	is>>_init_vel;
	is>>_acc;
	is>>_limit;

	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmTransItem::AnmTransItem( unsigned int asmId /*= -1*/, 
						   const VI& mdlIds /*= VI()*/, 
						   float duration /*= 0.f*/, 
						   const vec3f& dir /*= vec3f()*/, 
						   float init_vel /*= 0.0f*/, 
						   float acc /*= 2.11f*/, 
						   float limit /*= 0.f */ )
: AnimationItem(TRANSLATE, asmId, mdlIds, duration)
, _dir(dir)
, _init_vel(init_vel)
, _acc(acc)
, _limit(limit)
, _displace(0.f)
, _vel(0.f)
{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmTransItem::AnmTransItem( const AnmTransItem& rhd )
: AnimationItem(static_cast<const AnimationItem&>(rhd))
, _dir(rhd._dir)
, _init_vel(rhd._init_vel)
, _acc(rhd._acc)
, _limit(rhd._limit)
, _displace(rhd._displace)
, _vel(rhd._vel)
{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

void AnmTransItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_dir<<endl;
	os<<_init_vel<<endl;
	os<<_acc<<endl;
	os<<_limit<<endl;
}

AnimationItem* AnmTransItem::copy() const
{
	AnmTransItem* item = new AnmTransItem(*this);
	return static_cast<AnimationItem*>(item);
}

wostream& operator<<( wostream& os, const AnmTransItem& rhd )
{
	rhd.save(os);
	return os;
}

/************************************************************************/
/* 平抛动画                                                             */
/************************************************************************/
AnmProjectileItem::AnmProjectileItem( wistream& is )
: AnimationItem(PROJECTILE, is)
{
	is>>_dir1;
#ifdef MOON_VIS
	vec3f tem = _dir1;
	set_pt(_dir1, tem);
#endif
	is>>_init_vel1;
	is>>_acc1;
	is>>_limit1;
	is>>_dir2;
#ifdef MOON_VIS
	tem = _dir2;
	set_pt(_dir2, tem);
#endif
	is>>_init_vel2;
	is>>_acc2;
	is>>_limit2;

	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmProjectileItem::AnmProjectileItem( unsigned int asmId /*= -1*/, 
									 const VI& mdlIds /*= VI()*/, 
									 float duration /*= 60.f*/, 
									 const vec3f& dir1 /*= vec3f()*/, 
									 float init_vel1 /*= 1.0f*/, 
									 float acc1 /*= 0.0f*/, 
									 float limit1 /*= 0.f*/, 
									 const vec3f& dir2 /*= vec3f()*/, 
									 float init_vel2 /*= 0.0f*/, 
									 float acc2 /*= 2.11f*/, 
									 float limit2 /*= 0.f */ )
: AnimationItem(PROJECTILE, asmId, mdlIds, duration)
, _dir1(dir1)
, _init_vel1(init_vel1)
, _acc1(acc1)
, _limit1(limit1)
, _displace1(0.f)
, _vel1(0.f)
, _dir2(dir2)
, _init_vel2(init_vel2)
, _acc2(acc2)
, _limit2(limit2)
, _displace2(0.f)
, _vel2(0.f)

{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmProjectileItem::AnmProjectileItem( const AnmProjectileItem& rhd )
: AnimationItem(static_cast<const AnimationItem&>(rhd))
, _dir1(rhd._dir1)
, _init_vel1(rhd._init_vel1)
, _acc1(rhd._acc1)
, _limit1(rhd._limit1)
, _displace1(rhd._displace1)
, _vel1(rhd._vel1)
, _dir2(rhd._dir2)
, _init_vel2(rhd._init_vel2)
, _acc2(rhd._acc2)
, _limit2(rhd._limit2)
, _displace2(rhd._displace2)
, _vel2(rhd._vel2)
{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

void AnmProjectileItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_dir1<<endl;
	os<<_init_vel1<<endl;
	os<<_acc1<<endl;
	os<<_limit1<<endl;
	os<<_dir2<<endl;
	os<<_init_vel2<<endl;
	os<<_acc2<<endl;
	os<<_limit2<<endl;
}

void AnmProjectileItem::start( ACTInfo* pAct /*= NULL*/, AnmDepInfo* pDeps /*= NULL*/, AnmRotAxisInfo* pAxis /*= NULL*/ )
{
	AnimationItem::start();
	_displace1 = 0.f;
	_vel1 = _init_vel1;
	_displace2 = 0.f;
	_vel2 = _init_vel2;
}

bool AnmProjectileItem::update( float dt )
{
	if (!isActive() || isEnd())
	{
		return true;
	}

	dt = timeOn(dt);

	// 计算预测平移量
	float _displace1_t = _displace1 + _vel1*dt + 0.5f * _acc1*dt*dt;
	float _displace2_t = _displace2 + _vel2*dt + 0.5f * _acc2*dt*dt;
	
	float dt_new = dt;

	// 限制位移量
	if (_limit1 > FLT_MIN && fabs(_displace1_t) > _limit1)
	{
		_displace1_t = _limit1*_displace1_t/fabs(_displace1_t);
		float d = _displace1_t - _displace1;
		dt_new = std::max(0.f, 0.5f*std::sqrt(_vel1*_vel1+2*_acc1*d) - 0.5f*_vel1);
		_displace2_t = _displace2 + _vel2*dt_new + 0.5f * _acc2*dt_new*dt_new;
		_end = true;
	}
	else if (_limit2 > FLT_MIN && fabs(_displace2_t) > _limit2)
	{
		_displace2_t = _limit2*_displace2_t/fabs(_displace2_t);
		float d = _displace2_t - _displace2;
		dt_new = std::max(0.f, 0.5f*std::sqrt(_vel2*_vel2+2*_acc2*d) - 0.5f*_vel2);
		_displace1_t = _displace1 + _vel1*dt_new + 0.5f * _acc1*dt_new*dt_new;
		_end = true;
	}

	// 确定最终平移量
	_displace1 = _displace1_t;
	_displace2 = _displace2_t;
	
	// 更新实时速度
	_vel1 += _acc1*dt_new;
	_vel2 += _acc2*dt_new;

	return isEnd();
}

matrix4f AnmProjectileItem::getMatrix() const
{
	matrix4f mat1, mat2;
	mat1.set_translate(_dir1*_displace1*100);
	mat2.set_translate(_dir2*_displace2*100);
	return mat2 * mat1;
}

AnimationItem* AnmProjectileItem::copy() const
{
	AnmProjectileItem* item = new AnmProjectileItem(*this);
	return static_cast<AnimationItem*>(item);
}

wostream& operator<<( wostream& os, const AnmProjectileItem& rhd )
{
	rhd.save(os);
	return os;
}

/************************************************************************/
/* 旋转动画                                                             */
/************************************************************************/
void AnmRotItem::start(ACTInfo* pAct /* = NULL */, AnmDepInfo* pDeps /* = NULL */, AnmRotAxisInfo* pAxis /* = NULL */)
{
	AnimationItem::start();
	_angle = 0.f;
}

// 更新旋转运动状态
bool AnmRotItem::update( float dt )
{
	if (!isActive() || isEnd())
	{
		return true;
	}

	dt = timeOn(dt);

	// 计算旋转角度
	_angle += _ang_vel*dt;

	// 限制旋转角度
	if (_limit > FLT_MIN && fabs(_angle) > _limit)
	{
		_angle = _limit*_angle/fabs(_angle);
		_end = true;
	}

	return isEnd();
}

// 计算旋转变换矩阵
matrix4f AnmRotItem::getMatrix() const
{
	matrix4f t1, t2, t3;

	// 平移使旋转轴过原点
	t1.set_translate(-_axis._org);
	// 练轴旋转
	t2.set_rotate(_axis._dir, _angle*3.141593f/180.f);
	// 平移回初始位置
	t3.set_translate(_axis._org);

	// 合并矩阵
	return t3*t2*t1;
}

AnmRotItem::AnmRotItem( unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 0.f*/, const line& axis /*= line()*/, float ang_vel /*= 30.0f*/, float limit /*= 0.f */ )
: AnimationItem(ROTATE, asmId, mdlIds, duration)
, _axis(axis)
, _ang_vel(ang_vel)
, _limit(limit)
, _angle(0.f)
{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmRotItem::AnmRotItem( wistream& is )
: AnimationItem(ROTATE, is)
, _axis()
, _ang_vel(0.f)
, _limit(0.f)
, _angle(0.f)
{
	_axis.Load(is);
	is>>_ang_vel;
	is>>_limit;

	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmRotItem::AnmRotItem( const AnmRotItem& rhd )
: AnimationItem(static_cast<const AnmRotItem&>(rhd))
, _axis(rhd._axis)
, _ang_vel(rhd._ang_vel)
, _limit(rhd._limit)
, _angle(rhd._angle)
{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

void AnmRotItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_axis<<endl;
	os<<_ang_vel<<endl;
	os<<_limit<<endl<<endl;
}

AnimationItem* AnmRotItem::copy() const
{
	AnmRotItem* item = new AnmRotItem(*this);
	return static_cast<AnimationItem*>(item);
}

wostream& operator<<( wostream& os, const AnmRotItem& rhd )
{
	
	return os;
}

wostream& operator<<( wostream& os, const AnmScrewItem& rhd )
{
	rhd.save(os);
	return os;
}

/************************************************************************/
/* 螺旋动画                                                             */
/************************************************************************/
void AnmScrewItem::start(ACTInfo* pAct /* = NULL */, AnmDepInfo* pDeps /* = NULL */, AnmRotAxisInfo* pAxis /* = NULL */)
{
	AnimationItem::start();
	_angle = 0.f;
	_displace = 0.f;
}

// 更新螺旋运动状态
bool AnmScrewItem::update( float dt )
{
	if (!isActive() || isEnd())
	{
		return true;
	}

	dt = timeOn(dt);

	// 计算转动角度
	_angle += _ang_vel*dt;
	// 计算位移量
	_displace += _vel*dt;

	// 限制位移量
	if (_limit > FLT_MIN && fabs(_displace) > _limit)
	{
		_displace = _limit*_displace/fabs(_displace);
		_end = true;
	}

	return isEnd();
}

// 计算螺旋运动变换矩阵
matrix4f AnmScrewItem::getMatrix() const
{
	matrix4f t1, t2, t3, t4;

	// 绕任意轴旋转
	t1.set_translate(-_axis._org);
	t2.set_rotate(_axis._dir, _angle*3.141593f/180.f);
	t3.set_translate(_axis._org);

	// 平移
	t4.set_translate(_axis._dir*_displace*100);

	// 合并矩阵
	return t4*t3*t2*t1;
}

AnmScrewItem::AnmScrewItem( unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 0.f*/, const line& axis /*= line()*/, float ang_vel /*= 30.0f*/, float limit /*= 0.f */ )
: AnimationItem(SCREW, asmId, mdlIds, duration)
, _axis(axis)
, _ang_vel(30.f)
, _vel(0.1f)
, _limit(0.f)
, _angle(0.f)
, _displace(0.f)
{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmScrewItem::AnmScrewItem( wistream& is )
: AnimationItem(SCREW, is)
, _axis()
, _ang_vel(0.f)
, _vel(0.f)
, _limit(0.f)
, _angle(0.f)
, _displace(0.f)
{
	_axis.Load(is);
	is>>_ang_vel;
	is>>_vel;
	is>>_limit;

	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

AnmScrewItem::AnmScrewItem( const AnmScrewItem& rhd )
: AnimationItem(static_cast<const AnmScrewItem&>(rhd))
, _axis(rhd._axis)
, _ang_vel(rhd._ang_vel)
, _vel(rhd._vel)
, _limit(rhd._limit)
, _angle(rhd._angle)
, _displace(rhd._displace)
{
	_pApply = new AnmApplyTrst(this);
	_pEnd = new AnmEndTrsf(this);
}

void AnmScrewItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_axis<<endl;
	os<<_ang_vel<<endl;
	os<<_vel<<endl;
	os<<_limit<<endl<<endl;
}

AnimationItem* AnmScrewItem::copy() const
{
	AnmScrewItem* item = new AnmScrewItem(*this);
	return static_cast<AnimationItem*>(item);
}

AnmVerConsItem::AnmVerConsItem( unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 3600.f*/ )
: AnimationItem(VERCONS, asmId, mdlIds, duration)
{
_pEnd = new AnmEndTrsf(this);
}

AnmVerConsItem::AnmVerConsItem( wistream& is )
: AnimationItem(VERCONS, is)
{
_pEnd = new AnmEndTrsf(this);
}

void AnmVerConsItem::start( ACTInfo* pAct, AnmDepInfo* pDeps, AnmRotAxisInfo* pAxis )
{
	AnimationItem::start();
	assert(pAct != NULL && pDeps != NULL && pAxis != NULL);
	_axis = pAxis->getAxis(_asmId);
	_trsfInit = pAct->GetAnimate(_mdlIds[0]);
	
	_asmIdPar = pDeps->getParMdlId(_asmId);
	if (-1 == _asmIdPar)
	{
		return;
	}

	_axisPar = pAxis->getAxis(_asmIdPar);
	_mdlIdsPar = pDeps->getItem(_asmIdPar)->getMdlIds();
	_trsfParInit = pAct->GetAnimate(_mdlIdsPar[0]);
}

static matrix4f getTrsfRotByAxisDirToDir(const line& axis, const vec3f& src, const vec3f& dst)
{
	matrix4f t1, t2, t3;

	t1.set_translate(-axis._org);
	float cos_g = src.dot(dst);
	float grad = acos(cos_g);
	vec3f dir = src.cross(dst);
	if (dir.dot(axis._dir) < 0)
	{
		grad = -grad;
	}
	t2.set_rotate(axis._dir, grad);
	t3.set_translate(axis._org);

	return t3*t2*t1;
}

void AnmVerConsItem::apply( ACTInfo* pAct )
{
	//-- 当前变换矩阵
	matrix4f trsfPar = pAct->GetAnimate(_mdlIdsPar[0]);

	//-- 上一截杆初始状态
	vec3f O = _axisPar._org;
	vec3f temp_ = O - _axis._org;
	float d = temp_.dot(_axis._dir);
	vec3f T = _axis._org + _axis._dir*d;
	float r = (T-O).length();

	//-- 本动画初始时上一截杆
	vec3f O1;
	_trsfParInit.mult_matrix_vec(O, O1);
	vec3f T1;
	_trsfParInit.mult_matrix_vec(T, T1);

	//-- 当前帧调整前的上一截杆
	vec3f O2;
	trsfPar.mult_matrix_vec(O, O2);
	vec3f T2;
	trsfPar.mult_matrix_vec(T, T2);
	vec3f dirPar = T2 - O2;
	dirPar.normalize();

	//-- 竖直方向（原坐标系中是x向，moonvis坐标系中是y向）
	vec3f t  = vec3f(1.f, 0.f, 0.f);
	vec3f dirVer;
	set_pt(dirVer, t);

	vec3f T1O2 = O2 - T1;
	vec3f P = T1 + dirVer*(T1O2.dot(dirVer));
	d = (O2-P).length();
	int l;

	// 可以完全满足约束
	if (r > d)
	{
		l = sqrt(r*r - d*d);
	}
	// 只能心里满足约束
	else
	{
		l = 0;
	}

	vec3f PT1 = T1 - P;
	PT1.normalize();
	vec3f dirPar_t = P + (PT1)*l - O2;
	dirPar_t.normalize();
	vec3f T2_t = O2+dirPar_t*r;

	//-- 上一截杆在当前帧仍需要的旋转
	matrix4f trsfPar_n = getTrsfRotByAxisDirToDir(_axisPar, dirPar, dirPar_t);
	matrix4f trsfPar_t = trsfPar*trsfPar_n;
	for (VI::iterator itr = _mdlIdsPar.begin(); itr != _mdlIdsPar.end(); itr++)
	{
		pAct->SetAnimate(*itr, trsfPar_t);
		pAct->SetLocalAnimateOnBase(*itr, trsfPar_n);
	}

	//-- 本截只需给动画初始状态加一平移T_init(本截初始时T的位置)-->T2_t
	vec3f T_init;
	_trsfInit.mult_matrix_vec(T, T_init);
	matrix4f trsf_n;
	trsf_n.set_translate(T2_t - T_init);
	matrix4f trsf_t = trsf_n * _trsfInit;
	matrix4f trsf_local = trsfPar_t.inverse()*trsf_t;
	for (VI::iterator itr = _mdlIds.begin(); itr != _mdlIds.end(); itr++)
	{
		pAct->SetAnimate(*itr, trsf_t);
		pAct->SetLocalAnimate(*itr, trsf_local);
	}
}

void AnmVerConsItem::save( wostream& os ) const
{
	AnimationItem::save(os);
}

void AnmVerConsItem::end( ACTInfo* pAct )
{
	if (!isActive())
	{
		return;
	}
	for (VI::const_iterator itr = _mdlIdsPar.begin(); itr != _mdlIdsPar.end(); itr++)
	{
		pAct->SetLocalAnimateBase(*itr, pAct->GetLocalAnimate(*itr));
	}
	AnimationItem::end(pAct);
}

AnimationItem* AnmVerConsItem::copy() const
{
	AnmVerConsItem* item = new AnmVerConsItem(*this);
	return static_cast<AnimationItem*>(item);
}

AnmDumpSoilItem::AnmDumpSoilItem( unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 0.f*/, const line& axis /*= line()*/, float length /*= 1.f*/, float radius /*= 0.01f*/, float soil_radius /*= 0.005f*/, unsigned long color /*= RGB(128, 128, 128) */ )
: AnimationItem(DUMPSOIL, asmId, mdlIds, duration)
, _axis(axis)
, _length(length)
, _radius(radius)
, _soil_radius(soil_radius)
, _color(color)
, _pDumpSoil(NULL)
{
}

AnmDumpSoilItem::AnmDumpSoilItem( wistream& is )
: AnimationItem(DUMPSOIL, is)
, _axis()
, _length(0.f)
, _radius(0.f)
, _soil_radius(0.f)
, _color()
, _pDumpSoil(NULL)
{
	_axis.Load(is);
	is>>_length;
	is>>_radius;
	is>>_soil_radius;
	is>>_color;
}

void AnmDumpSoilItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_axis<<endl;
	os<<_length<<endl;
	os<<_radius<<endl;
	os<<_soil_radius<<endl;
	os<<_color<<endl;
}

void AnmDumpSoilItem::start( ACTInfo* pAct /*= NULL*/, AnmDepInfo* pDeps /*= NULL*/, AnmRotAxisInfo* pAxis /*= NULL*/ )
{
	AnimationItem::start();
	if (NULL == _pDumpSoil)
	{
		delete _pDumpSoil;
	}
	_pDumpSoil = new DumpSoilAnm(_length, _radius, _soil_radius);
}

void AnmDumpSoilItem::draw(ACTInfo* pAct)
{
	AnimationItem::draw(pAct);
	matrix4f mat = pAct->GetAnimate(_mdlIds[0]);
	nv::matrix4f mat_n;
	mat_n.set_value(const_cast<float*>(mat.get_value()));
	nv::vec3f pos(reinterpret_cast<float*>(&_axis._org));
	nv::vec3f dir(reinterpret_cast<float*>(&_axis._dir));
	_pDumpSoil->Draw(mat_n, pos, dir);
}

bool AnmDumpSoilItem::update( float dt )
{
	AnimationItem::update(dt);
	_pDumpSoil->update(dt);
	if (isEnd())
	{
		_pDumpSoil->setEnd();
		if (!_pDumpSoil->isEmpty())
		{
			_end = false;
			_playedTime -= dt;
		}
	}
	return isEnd();
}

AnimationItem* AnmDumpSoilItem::copy() const
{
	AnmDumpSoilItem* item = new AnmDumpSoilItem(*this);
	return static_cast<AnimationItem*>(item);
}

AnmDrillEarthItem::AnmDrillEarthItem( unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 0.f*/, const line& axis /*= line()*/, float radius /*= 0.05f*/, float outer_radius /*= 0.1f*/, float height /*= 0.05f*/, float soil_radius /*= 0.005f*/, float acc_rate /*= 0.005f*/, unsigned long color /*= RGB(128, 128, 128) */ )
: AnimationItem(DRILLEARTH, asmId, mdlIds, duration)
, _axis(axis)
, _radius(radius)
, _outer_radius(outer_radius)
, _height(height)
, _soil_radius(soil_radius)
, _acc_rate(acc_rate)
, _color(color)
, _pDigSoil(NULL)
{
}

AnmDrillEarthItem::AnmDrillEarthItem( wistream& is )
: AnimationItem(DRILLEARTH, is)
, _axis()
, _radius(0.f)
, _outer_radius(0.f)
, _height(0.f)
, _soil_radius(0.f)
, _acc_rate(0.f)
, _color()
, _pDigSoil(NULL)
{
	_axis.Load(is);
	is>>_radius;
	is>>_outer_radius;
	is>>_height;
	is>>_soil_radius;
	is>>_acc_rate;
	is>>_color;
}

void AnmDrillEarthItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_axis<<endl;
	os<<_radius<<endl;
	os<<_outer_radius<<endl;
	os<<_height<<endl;
	os<<_soil_radius<<endl;
	os<<_acc_rate<<endl;
	os<<_color<<endl;
}

void AnmDrillEarthItem::start( ACTInfo* pAct /*= NULL*/, AnmDepInfo* pDeps /*= NULL*/, AnmRotAxisInfo* pAxis /*= NULL */ )
{
	AnimationItem::start();
	if (NULL != _pDigSoil)
	{
		delete _pDigSoil;
	}
	_pDigSoil = new DigSoilAnm(_radius, _radius+_outer_radius, _height/(_radius+_outer_radius), _soil_radius, _acc_rate);

	matrix4f mat = pAct->GetAnimate(_mdlIds[0]);
	mat.mult_matrix_vec(_axis._org, _pos);
	mat.mult_matrix_dir(_axis._dir, _dir);
}

bool AnmDrillEarthItem::update( float dt )
{
	AnimationItem::update(dt);
	_pDigSoil->update(dt);
	return isEnd();
}

void AnmDrillEarthItem::draw( ACTInfo* pAct /*= NULL*/ )
{
	AnimationItem::draw(pAct);

	nv::vec3f pos(reinterpret_cast<float*>(&_pos));
	nv::vec3f dir(reinterpret_cast<float*>(&_dir));
	_pDigSoil->Draw(nv::matrix4f(), pos, dir);
}

AnimationItem* AnmDrillEarthItem::copy() const
{
	AnmDrillEarthItem* item = new AnmDrillEarthItem(*this);
	return static_cast<AnimationItem*>(item);
}

AnmGlowItem::AnmGlowItem( unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 0.f*/, unsigned long color /*= RGB(128, 128, 128) */ )
: AnimationItem(GLOW, asmId, mdlIds, duration)
, _color()
{
}

AnmGlowItem::AnmGlowItem( wistream& is )
: AnimationItem(GLOW, is)
, _color()
{
	is>>_color;
}

void AnmGlowItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_color<<endl<<endl;
}

AnimationItem* AnmGlowItem::copy() const
{
	AnmGlowItem* item = new AnmGlowItem(*this);
	return static_cast<AnimationItem*>(item);
}

/************************************************************************/
/* 跟随动画                                                             */
/************************************************************************/
AnmFollowItem::AnmFollowItem( unsigned int asmId /*= -1*/, const VI& mdlIds /*= VI()*/, float duration /*= 60.f*/, unsigned int targetMdlId /*= -1 */ )
: AnimationItem(FOLLOW, asmId, mdlIds, duration)
, _targetMdlId(targetMdlId)
{
	_pEnd = new AnmEndTrsf(this);
}

AnmFollowItem::AnmFollowItem( wistream& is )
: AnimationItem(FOLLOW, is)
{
	is>>_targetMdlId;

	_pEnd = new AnmEndTrsf(this);
}

void AnmFollowItem::save( wostream& os ) const
{
	AnimationItem::save(os);
	os<<_targetMdlId<<endl<<endl;
}

void AnmFollowItem::start( ACTInfo* pAct, AnmDepInfo* pDeps /*= NULL*/, AnmRotAxisInfo* pAxis /*= NULL*/ )
{
	AnimationItem::start();
	assert(pAct != NULL);
	matrix4f matInit = pAct->GetAnimate(_targetMdlId);
	_matInit_inv = matInit.inverse();
}

void AnmFollowItem::apply( ACTInfo* pAct )
{
	matrix4f mat = pAct->GetAnimate(_targetMdlId);
	mat = mat * _matInit_inv;
	for (VI::iterator itr = _mdlIds.begin(); itr != _mdlIds.end(); itr++)
	{
		pAct->SetLocalAnimate(*itr, mat * pAct->GetLocalAnimateBase(*itr));
		pAct->SetAnimate(*itr, pAct->GetLocalAnimate(*itr));
	}
}

AnimationItem* AnmFollowItem::copy() const
{
	AnmFollowItem* item = new AnmFollowItem(*this);
	return static_cast<AnimationItem*>(item);
}

/************************************************************************/
/* 动画集                                                               */
/************************************************************************/
AnimationSet::AnimationSet( wistream& is )
: _active(false)
, _end(false)
{
	int n;
	is>>n;
	while (n--)
	{
		int type;
		is>>type;
		switch (type)
		{
		case AnimationItem::TRANSLATE:
			{
				push_back(new AnmTransItem(is));
			}
			break;
		case AnimationItem::PROJECTILE:
			{
				push_back(new AnmProjectileItem(is));
			}
			break;
		case AnimationItem::ROTATE:
			{
				push_back(new AnmRotItem(is));
			}
			break;
		case AnimationItem::SCREW:
			{
				push_back(new AnmScrewItem(is));
			}
			break;
		case AnimationItem::VERCONS:
			{
				push_back(new AnmVerConsItem(is));
			}
			break;
		case AnimationItem::DUMPSOIL:
			{
				push_back(new AnmDumpSoilItem(is));
			}
			break;
		case AnimationItem::DRILLEARTH:
			{
				push_back(new AnmDrillEarthItem(is));
			}
			break;
		case AnimationItem::GLOW:
			{
				push_back(new AnmGlowItem(is));
			}
			break;
		case AnimationItem::FOLLOW:
			{
				push_back(new AnmFollowItem(is));
			}
			break;
		default:
			break;
		}
	}
}

AnimationSet::AnimationSet( const AnimationSet& rhd )
: _active(false)
, _end(false)
{
	for (const_iterator itr = rhd.begin(); itr != rhd.end(); itr++)
	{
		push_back((*itr)->copy());
	}
}

bool AnimationSet::includeAsm( unsigned int id )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if (id == (*itr)->getAsmId())
		{
			return true;
		}
	}
	return false;
}

// 更新动画组内的所有动画
bool AnimationSet::update(AnimationInfo* pAnms, ACTInfo* pAct, AnmDepInfo* pDeps, float dt, bool updateSoil /* = true */)
{
	pDeps->reset(pAct);

	bool isEnd = true;
	vector<AnmVerConsItem*> pConses;
	vector<AnmFollowItem*> pFollows;
	// 更新每一个动画
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if ((*itr)->isEnd())
		{
			continue;
		}

		// 收集需特殊处理的动画
		AnimationItem::Type type = (*itr)->getType();
		if (type == AnimationItem::VERCONS)
		{
			AnmVerConsItem* pCons = dynamic_cast<AnmVerConsItem*>(*itr);
			assert(pCons != NULL);
			pConses.push_back(pCons);
			continue;
		}
		if (type == AnimationItem::FOLLOW)
		{
			AnmFollowItem* pFollow= dynamic_cast<AnmFollowItem*>(*itr);
			assert(pFollow != NULL);
			pFollows.push_back(pFollow);
			continue;
		}
		if (!updateSoil && (AnimationItem::DUMPSOIL == type || AnimationItem::DRILLEARTH == type))
		{
			continue;
		}

		// 更新并应用动画单元
		(*itr)->update(dt);
		(*itr)->apply(pAct);

		// 动画单元结束处理
		if (!(*itr)->isEnd())
		{
			isEnd = false;
		}
		else
		{
			(*itr)->end(pAct);
			if (AnimationItem::DRILLEARTH == (*itr)->getType())
			{
				AnmDrillEarthItem* pAnmDE = dynamic_cast<AnmDrillEarthItem*>(*itr);
				assert(pAnmDE != NULL);
				pAnms->addDigSoil(pAnmDE);
			}
		}
	}
	_end = isEnd;
	
	// 应用动画逻辑依赖关系
	pDeps->apply(pAct);

	// 应用竖直约束，需要在逻辑依赖关系应用后进行
	if (!pConses.empty())
	{
		for (vector<AnmVerConsItem*>::iterator itr = pConses.begin(); itr != pConses.end(); itr++)
		{
			(*itr)->apply(pAct);
			if (isEnd || (*itr)->isEnd())
			{
				(*itr)->end(pAct);
			}
		}
		// 再次应用动画逻辑依赖关系
		pDeps->apply(pAct);
	}
	

	// 应用跟随
	for (vector<AnmFollowItem*>::iterator itr = pFollows.begin(); itr != pFollows.end(); itr++)
	{
		(*itr)->apply(pAct);
		if (isEnd || (*itr)->isEnd())
		{
			(*itr)->end(pAct);
		}
	}

	return isEnd;
}

void AnimationSet::start(ACTInfo* pAct, AnmDepInfo* pDeps, AnmRotAxisInfo* pAxis)
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		(*itr)->start(pAct, pDeps, pAxis);
	}
}

void AnimationSet::setEnd(ACTInfo* pAct, AnimationInfo* pAnms)
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		(*itr)->end(pAct);
	}
}

void AnimationSet::save( wostream& os ) const
{
	os<<size()<<endl;
	for (const_iterator itr = begin(); itr != end(); itr++)
	{
		os<<*itr<<endl;
	}
}

void AnimationSet::delItem( AnimationItem* pItem )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if (pItem == *itr)
		{
			delete *itr;
			erase(itr);
			return;
		}
	}
}

void AnimationSet::draw(ACTInfo* pAct)
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		(*itr)->draw(pAct);
	}
}


wostream& operator<<( wostream& os, const AnimationSet& rhd )
{
	rhd.save(os);
	return os;
}


AnimationInfo::AnimationInfo()
: AnmSetPtrVec()
, _pAct(NULL)
, _curItr(begin())
{

}

AnimationInfo::AnimationInfo( wistream& is )
: AnmSetPtrVec()
, _anmDeps(is)
, _anmRotAxises(is)
, _pAct(NULL)
, _curItr(begin())
{
	int n;
	is>>n;
	for (int i = 0; i < n; i++)
	{
		push_back(new AnimationSet(is));
	}
}

// 更新动画
bool AnimationInfo::update( float dt )
{
	if (_curItr == end())
	{
		return true;
	}

	AnimationSet* pCurAnmSet = *_curItr;

	// 更新当前动画组，返回值为真表示动画最后一次更新，随后切换到下个动画组
	bool isEnd = pCurAnmSet->update(this, _pAct, &_anmDeps, dt);

	// 动画结束或设置下一个动画组
	if (isEnd)
	{
		_curItr++;
		if (_curItr != end())
		{
			pCurAnmSet = *_curItr;
			pCurAnmSet->start(_pAct, &_anmDeps, &_anmRotAxises);
		}
		else
		{
			return true;
		}
	}

	return false;
}

void AnimationInfo::updateOneByOne()
{
	if (_curItr == end())
	{
		return;
	}
	AnimationSet* pCurAnmSet = *_curItr;

	while (!pCurAnmSet->update(this, _pAct, &_anmDeps, 0.1f, false)); //-- 不断更新直至结束

	_curItr++;
	if (_curItr != end())
	{
		pCurAnmSet = *_curItr;
		pCurAnmSet->start(_pAct, &_anmDeps, &_anmRotAxises);
	}
}

// 动画播放初始化
void AnimationInfo::start()
{
	if (NULL == _pAct)
	{
		return;
	}

	// 模型恢复到初始位置
	unsigned int count = _pAct->GetModelCount();
	for (unsigned int i = 0; i < count; i++)
	{
		_pAct->SetAnimate(i, matrix4f());
		_pAct->SetLocalAnimate(i, matrix4f());
		_pAct->SetLocalAnimateBase(i, matrix4f());
	}

	// 设定第一个动画组为当前组，并进行初始化
	_curItr = begin();
	if (_curItr != end())
	{
		AnimationSet* pCurAnmSet = *_curItr;
		pCurAnmSet->start(_pAct, &_anmDeps, &_anmRotAxises);
	}
	
	_drillEarthAnms.clear();
}

void AnimationInfo::save( wostream& os ) const
{
	os<<_anmDeps<<endl;
	os<<_anmRotAxises<<endl;
	os<<size()<<endl;
	for (const_iterator itr = begin(); itr != end(); itr++)
	{
		os<<**itr<<endl;
	}
}

void AnimationInfo::delAnmSet( AnimationSetPtr pAnmSet )
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		if (pAnmSet == *itr)
		{
			delete *itr;
			erase(itr);
			return;
		}
	}
}

void AnimationInfo::setEnd()
{
	if (NULL == _pAct)
	{
		return;
	}

	_curItr = begin();
	unsigned int count = _pAct->GetModelCount();
	for (unsigned int i = 0; i < count; i++)
	{
		_pAct->SetAnimate(i, matrix4f());
		_pAct->SetLocalAnimateBase(i, matrix4f());
	}
}

void AnimationInfo::playToSet( AnimationSetPtr pAnmSet )
{
	if (NULL == _pAct)
	{
		return;
	}

	if (find(begin(), end(), pAnmSet) == end())
	{
		setEnd();
		return;
	}

	start();
	for (iterator itr = begin(); itr != end(); itr++)
	{
		updateOneByOne();
		if (*itr == pAnmSet)
		{
			break;
		}
	}
}

void AnimationInfo::draw()
{
	for (vector<AnmDrillEarthItem*>::iterator itr = _drillEarthAnms.begin(); itr != _drillEarthAnms.end(); itr++)
	{
		(*itr)->draw(_pAct);
	}

	if (_curItr == end())
	{
		return;
	}
	AnimationSet* pCurAnmSet = *_curItr;
	pCurAnmSet->draw(_pAct);
}

wostream& operator<<( wostream& os, const AnimationInfo& rhd )
{
	rhd.save(os);
	return os;
}
