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
// MoonVisDoc.cpp : CMoonVisDoc 类的实现
//

#include "stdafx.h"
#include "MoonVis.h"

#include "MoonVisDoc.h"

#include "../CSM-src/terrain.h"
#include "gl/GL.h"
#include "../CSM-src/camera.h"
#include <vector>
#include <locale>

#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern char* depth_tex_filename;
extern char* inf_filename;
extern wchar_t* explorer_filename;
extern wchar_t* explorer_animation_filename;
extern wchar_t* material_filename;
extern wchar_t* point_filename;

extern float light_dir[4];
extern int cameraID;
extern std::vector<camera> cameraList;
extern bool isLoaded;
extern std::vector<vec3f> selectp;
extern std::vector<vec3f> selectpn;

// CMoonVisDoc

IMPLEMENT_DYNCREATE(CMoonVisDoc, CDocument)

BEGIN_MESSAGE_MAP(CMoonVisDoc, CDocument)
END_MESSAGE_MAP()


// CMoonVisDoc 构造/析构

CMoonVisDoc::CMoonVisDoc()
{
	// TODO: 在此添加一次性构造代码

}

CMoonVisDoc::~CMoonVisDoc()
{
}

BOOL CMoonVisDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	m_bIsLoaded = FALSE;
	isLoaded = false;
	cameraList.clear();
	cameraID = 0;

	vec3f tmp(light_dir[0], light_dir[1], light_dir[2]);
	tmp.normalize();

	light_dir[0] = tmp.x;
	light_dir[1] = tmp.y;
	light_dir[2] = tmp.z;

	loadCameraWithModel();

	return TRUE;
}

// CMoonVisDoc 序列化

void CMoonVisDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CMoonVisDoc 诊断

#ifdef _DEBUG
void CMoonVisDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMoonVisDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMoonVisDoc 命令

BOOL CMoonVisDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	char* old_local = _strdup(setlocale(LC_CTYPE,NULL));
	setlocale( LC_CTYPE, "chs");
	CStdioFile fin(lpszPathName, CFile::modeCreate | CFile::modeWrite);
	CString buff;
	fin.WriteString(_T("[explorer path]\n"));
	fin.WriteString(CString(explorer_filename) + _T("\n"));
	fin.WriteString(_T("[terrain path]\n"));
	fin.WriteString(CString(depth_tex_filename) + _T("\n"));
	fin.WriteString(_T("[light direction]\n"));
	buff.Format(_T("%f %f %f\n"), light_dir[0], light_dir[1], light_dir[2]);
	fin.WriteString(buff);
	
	/*fin.WriteString(_T("[camera parameters]\n"));
	buff.Format(_T("%d\n"), cameraList.size());
	fin.WriteString(buff);
	buff.Empty();
	for (int i = 0; i < cameraList.size(); i++)
	{
		CString buff2;
		camera& cam = cameraList[i];
		float v[3];
		cam.getInitPos(v);
		buff2.Format(_T("%f %f %f\n"), v[0], v[1], v[2]);
		buff += buff2;
		cam.getInitView(v);
		buff2.Format(_T("%f %f %f\n"), v[0], v[1], v[2]);
		buff += buff2;
		buff2.Format(_T("%f %f\n"), cam.getNear(), cam.getFar());
		buff += buff2;
		buff2.Format(_T("%f %f\n"), cam.getFov(), cam.getRatio());
		buff += buff2;
		buff2.Format(_T("%d %d\n"), cam.getXRes(), cam.getYRes());
		buff += buff2;
		buff2.Format(_T("%d\n"), cam.getFreq());
		buff += buff2;
		buff += CString(cam.getName());
		buff += CString(_T("\n"));
		buff2.Format(_T("%d\n"), cam.getMdlId());
		buff += buff2;
	}
	fin.WriteString(buff);
	buff.Empty();*/

	/*fin.WriteString(_T("[point information]\n"));
	int num = selectp.size();
	buff.Format(_T("%d\n"), num);
	fin.WriteString(buff);
	buff.Empty();
	for (int i = 0; i < num; i++)
	{
		CString buff2;
		vec3f p, n;
		p = selectp[i];
		n = selectpn[i];
		buff2.Format(_T("v %f %f %f\n"), p[0], p[1], p[2]);
		buff += buff2;
		buff2.Format(_T("n %f %f %f\n"), n[0], n[1], n[2]);
		buff += buff2;
	}
	fin.WriteString(buff);*/

	fin.Close();

	CString path(explorer_filename);
	path = path.Left(path.GetLength() - 3);
	path += _T("mrk");
	CStdioFile ffin(path, CFile::modeCreate | CFile::modeWrite);
	buff.Empty();
	ffin.WriteString(_T("[camera parameters]\n"));
	buff.Format(_T("%d\n"), cameraList.size());
	ffin.WriteString(buff);
	buff.Empty();
	for (int i = 0; i < cameraList.size(); i++)
	{
		CString buff2;
		camera& cam = cameraList[i];
		float v[3];
		cam.getInitPos(v);
		buff2.Format(_T("%f %f %f\n"), v[0], v[1], v[2]);
		buff += buff2;
		cam.getInitView(v);
		buff2.Format(_T("%f %f %f\n"), v[0], v[1], v[2]);
		buff += buff2;
		buff2.Format(_T("%f %f\n"), cam.getNear(), cam.getFar());
		buff += buff2;
		buff2.Format(_T("%f %f\n"), cam.getFov(), cam.getRatio());
		buff += buff2;
		buff2.Format(_T("%d %d\n"), cam.getXRes(), cam.getYRes());
		buff += buff2;
		buff2.Format(_T("%d\n"), cam.getFreq());
		buff += buff2;
		buff += CString(cam.getName());
		buff += CString(_T("\n"));
		buff2.Format(_T("%d\n"), cam.getMdlId());
		buff += buff2;
	}
	ffin.WriteString(buff);
	buff.Empty();
	ffin.Close();

	setlocale(LC_CTYPE, old_local);
	return TRUE;
}

BOOL CMoonVisDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here
	m_bIsLoaded = TRUE;
	//isLoaded = true;

	char* old_local = _strdup(setlocale(LC_CTYPE,NULL));
	setlocale( LC_CTYPE, "chs");
	CStdioFile fin(lpszPathName, CFile::modeRead);
	CString buff;
	while (fin.ReadString(buff))
	{
		if (_T("[explorer path]") == buff)
		{
			fin.ReadString(buff);
			if (NULL != explorer_filename)
				delete[] explorer_filename;
			explorer_filename = new wchar_t[buff.GetLength()+1];
			memcpy(explorer_filename, buff.GetBuffer(), buff.GetLength()*sizeof(wchar_t));
			explorer_filename[buff.GetLength()] = _T('\0');

			buff = buff.Left(buff.GetLength() - 3);
			buff += _T("anm");
			if (NULL != explorer_animation_filename)
				delete[] explorer_animation_filename;
			explorer_animation_filename = new wchar_t[buff.GetLength()+1];
			memcpy(explorer_animation_filename, buff.GetBuffer(), buff.GetLength()*sizeof(wchar_t));
			explorer_animation_filename[buff.GetLength()] = _T('\0');
		
			buff = buff.Left(buff.GetLength() - 3);
			buff += _T("mat");
			if (NULL != material_filename)
				delete[] material_filename;
			material_filename = new wchar_t[buff.GetLength()+1];
			memcpy(material_filename, buff.GetBuffer(), buff.GetLength()*sizeof(wchar_t));
			material_filename[buff.GetLength()] = _T('\0');

			buff = buff.Left(buff.GetLength() - 3);
			buff += _T("vtx");
			if (NULL != point_filename)
				delete[] point_filename;
			point_filename = new wchar_t[buff.GetLength() + 1];
			memcpy(point_filename, buff.GetBuffer(), buff.GetLength() * sizeof(wchar_t));
			point_filename[buff.GetLength()] = _T('\0');

			loadCameraWithModel();
		}
		else if (_T("[terrain path]") == buff)
		{
			fin.ReadString(buff);
			if (NULL != depth_tex_filename)
				delete[] depth_tex_filename;
			WideCharToMultiByte_M(buff, &depth_tex_filename);

			buff = buff.Left(buff.GetLength() - 3);
			buff += _T("inf");
			if (NULL != inf_filename)
				delete[] inf_filename;
			WideCharToMultiByte_M(buff, &inf_filename);
		}
		else if (_T("[light direction]") == buff)
		{
			fin.ReadString(buff);
			char* cbuff;
			WideCharToMultiByte_M(buff, &cbuff);
			sscanf(cbuff, "%f%f%f", &light_dir[0], &light_dir[1], &light_dir[2]);
			delete[] cbuff;
		}
		/*else if (_T("[camera parameters]") == buff)
		{
			fin.ReadString(buff);
			char* cbuff = NULL;
			WideCharToMultiByte_M(buff, &cbuff);
			int n;
			sscanf(cbuff, "%d", &n);
			buff.Empty();
			delete[] cbuff;
			cameraList.clear();
			cameraID = 0;
			for (int i = 0; i < n; i++)
			{
				float pos[3], view[3];
				float neard, fard, fov, ratio;
				int xres, yres;
				int freq;
				CString buff2;
				buff.Empty();
				for (int l = 0; l < 6; l++)
				{
					fin.ReadString(buff2);
					buff += buff2 + _T(" ");
				}
				WideCharToMultiByte_M(buff, &cbuff);

				sscanf(
					cbuff,
					"%f%f%f"
					"%f%f%f"
					"%f%f"
					"%f%f"
					"%d%d"
					"%d",
					&pos[0], &pos[1], &pos[2],
					&view[0], &view[1], &view[2],
					&neard, &fard,
					&fov, &ratio,
					&xres, &yres,
					&freq
					);
				delete[] cbuff;

				CString cam_name;
				fin.ReadString(cam_name);

				fin.ReadString(buff);
				WideCharToMultiByte_M(buff, &cbuff);
				int mdl_id;
				sscanf(cbuff, "%d", &mdl_id);
				delete[] cbuff;

				cameraList.push_back(
					camera(
					pos, view,
					neard, fard,
					fov, ratio,
					xres, yres,
					freq
					)					
					);
				++cameraID;

				WideCharToMultiByte_M(cam_name, &cbuff);
				cameraList[cameraID-1].setName(cbuff);
				delete[] cbuff;
				cameraList[cameraID-1].setMdlId(mdl_id);

				buff.Empty();
			}
		}*/
		/*else if (_T("[point information]") == buff)
		{
			fin.ReadString(buff);
			char* cbuff = NULL;
			WideCharToMultiByte_M(buff, &cbuff);
			int n;
			sscanf(cbuff, "%d", &n);
			buff.Empty();
			delete[] cbuff;
			ClearPointInfo();
			for (int i = 0; i < n; i++) {
				float x, y, z;
				char c;
				buff.Empty();
				fin.ReadString(buff);
				WideCharToMultiByte_M(buff, &cbuff);
				sscanf(cbuff, "%c%f%f%f", &c, &x, &y, &z);
				buff.Empty();
				SetPointValue(i, x, y, z);

				fin.ReadString(buff);
				WideCharToMultiByte_M(buff, &cbuff);
				sscanf(cbuff, "%c%f%f%f", &c, &x, &y, &z);
				buff.Empty();
				SetPointNormalValue(i, x, y, z);
			}
		}*/
	}

	setlocale( LC_CTYPE, old_local);
	return TRUE;
}

inline void set_pt2(nv::vec3f &dst, nv::vec3f &src)
{
	dst.x = src.z;
	dst.y = src.x;
	dst.z = src.y;
}

void CMoonVisDoc::loadCameraWithModel()
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
		if (_T("[camera parameters]") == buff) {
			isLoaded = true;
			fin.ReadString(buff);
			char* cbuff = NULL;
			WideCharToMultiByte_M(buff, &cbuff);
			int n;
			sscanf(cbuff, "%d", &n);
			buff.Empty();
			delete[] cbuff;
			cameraList.clear();
			cameraID = 0;
			for (int i = 0; i < n; i++)
			{
				float pos[3], view[3];
				float neard, fard, fov, ratio;
				int xres, yres;
				int freq;
				CString buff2;
				buff.Empty();
				for (int l = 0; l < 6; l++)
				{
					fin.ReadString(buff2);
					buff += buff2 + _T(" ");
				}
				WideCharToMultiByte_M(buff, &cbuff);

				sscanf(
					cbuff,
					"%f%f%f"
					"%f%f%f"
					"%f%f"
					"%f%f"
					"%d%d"
					"%d",
					&pos[0], &pos[1], &pos[2],
					&view[0], &view[1], &view[2],
					&neard, &fard,
					&fov, &ratio,
					&xres, &yres,
					&freq
				);
				delete[] cbuff;

				CString cam_name;
				fin.ReadString(cam_name);

				fin.ReadString(buff);
				WideCharToMultiByte_M(buff, &cbuff);
				int mdl_id;
				sscanf(cbuff, "%d", &mdl_id);
				delete[] cbuff;

				cameraList.push_back(
					camera(
						pos, view,
						neard, fard,
						fov, ratio,
						xres, yres,
						freq
					)
				);
				++cameraID;

				WideCharToMultiByte_M(cam_name, &cbuff);
				cameraList[cameraID - 1].setName(cbuff);
				delete[] cbuff;
				cameraList[cameraID - 1].setMdlId(mdl_id);

				buff.Empty();
			}
		}
		else {
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

				CString cam_nam;
				fin.ReadString(cam_nam);

				fin.ReadString(buff);
				WideCharToMultiByte_M(buff, &cbuff);
				int mdl_id, type;
				sscanf(cbuff, "%d, %d", &mdl_id, &type);
				delete[] cbuff;
				if (1 != type) //-- not camera
				{
					continue;
				}

				nv::vec3f pos_t, dir_t;
				set_pt2(pos_t, pos);
				set_pt2(dir_t, dir);

				cameraList.push_back(camera(pos_t, dir_t));
				++cameraID;
				WideCharToMultiByte_M(cam_nam, &cbuff);
				cameraList[cameraID - 1].setName(cbuff);
				delete[] cbuff;
				cameraList[cameraID - 1].setMdlId(mdl_id);
			}
		}
	}
	CATCH (CFileException, e)
	{
		return;
	}
	END_CATCH

	setlocale( LC_CTYPE, old_local);
}