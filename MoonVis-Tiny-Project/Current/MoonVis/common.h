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

/************************************************************************/
/* 自定义字符串转换函数                                                 */
/************************************************************************/
inline void WideCharToMultiByte_M(const CString& wstr, char** mstr)
{
	int wlen = wstr.GetLength();
	int mlen = WideCharToMultiByte(CP_ACP, 0, wstr, wlen, NULL, 0, NULL, NULL);
	*mstr = new char[mlen + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr, wlen + 1, *mstr, mlen + 1, NULL, NULL);
	(*mstr)[mlen] = '\0';
}

#include <nvMath.h>
/************************************************************************/
/* 齐次坐标转非齐次                                                     */
/************************************************************************/
inline nv::vec3f transHomoToNon(const nv::vec4f& v)
{
	return nv::vec3f(v.x/v.w, v.y/v.w, v.z/v.w);
}