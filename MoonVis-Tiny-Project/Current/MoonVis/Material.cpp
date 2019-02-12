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
#include "Material.h"
#include <atlstr.h>


#pragma warning(disable: 4996)


CString CharToCstring(char *pChar)
{
	CString ret;
	int charLen = strlen(pChar);
	int len = MultiByteToWideChar(CP_ACP, 0, pChar, charLen, NULL, 0);
	TCHAR *buf = new TCHAR[len+1];
	MultiByteToWideChar(CP_ACP, 0, pChar, charLen, buf, len);
	buf[len] = '\0';
	ret.Append(buf);
	delete buf;
	return ret;
}
void CstringToChar(CString s, char pChar[])
{
	int i;
	char *tmp;
	int wlen = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL);
	tmp = new char[wlen + 1];
	WideCharToMultiByte(CP_ACP, 0, s, -1, tmp, wlen, NULL, NULL);

	for(i = 0; tmp[i]; ++i)
		pChar[i]=tmp[i];
	pChar[i]='\0';
	delete tmp;
}
CMaterial::CMaterial()
{
	_mat = NULL;
	num = 0;
	textureMode = 0;

	matr = NULL;
	textureID = NULL;
	normalMapID = NULL;
}

void
CMaterial::Load(FILE *fp)
{
	_ftscanf(fp, _T("Material Numbers:%d\n"), &num);
	if (feof(fp) != 0)
		return;
	else {
		CString tmp;
		int r, g, b;
		_ftscanf(fp, _T("TextureMode: %d\n"), &textureMode);
		//修改了char数组的长度为1024 by lzh 2015/4/13
		_ftscanf(fp, _T("%s\n"), tmp.GetBuffer(1024));
		tmp.ReleaseBuffer();
		_mat = new MatItem[num];
		for(int i=0; i<num; ++i)
		{
			//一般来说CString要像这样使用，即采用GetBuffer转换成LPTSTR进行
			//使用，然后在使用ReleaseBuffer()进行更新CString中的成员变量。comment by lzh 2014/10/6
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("Material Name: %s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
			CstringToChar(tmp, _mat[i].matName);
			_ftscanf(fp, _T("Diffuse: %d %d %d\n"), &r, &g, &b);
			_mat[i].diffuse = RGB(r, g, b);
			_ftscanf(fp, _T("Ambitient: %d %d %d\n"), &r, &g, &b);
			_mat[i].ambitient = RGB(r, g, b);
			_ftscanf(fp, _T("Specular:  %d %d %d\n"), &r, &g, &b);
			_mat[i].specular = RGB(r, g, b);
			
			_ftscanf(fp, _T("Shininess: %f\n"), &_mat[i].shininess);

			//关于使用_fgets的时候要注意它会把换行符也读取在字符串中，并且在vs的调试中你不能明显看到是包含了换行符的
			//所以tmp = tmp.Left(tmp.GetLength() - 1);是用来去掉最后的换行符的。comment by lzh 2014/10/06
			/*_fgetts(tmp.GetBuffer(100), 99, fp);
			tmp.ReleaseBuffer();
			tmp.Trim(_T(" "));
			tmp = tmp.Left(tmp.GetLength() - 1);
			tmp = tmp.Right(tmp.GetLength() - 13);*/
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("Texture Path: %s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
			tmp.Trim(_T(" "));
			CstringToChar(tmp, _mat[i].texPath);
			if(tmp == "\"\""){
				strcpy(_mat[i].texPath, "");
			}else{
				int ttt = tmp.GetLength();
				CString re;
				re = tmp.Right(ttt - 1);
				tmp = re.Left(re.GetLength() - 1);
				CstringToChar(tmp, _mat[i].texPath);
			}
			/*_fgetts(tmp.GetBuffer(100), 99, fp);
			tmp.ReleaseBuffer();
			tmp.Trim(_T(" "));
			tmp = tmp.Left(tmp.GetLength() - 1);
			tmp = tmp.Right(tmp.GetLength() - 12);*/
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("Normal Path: %s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
			tmp.Trim(_T(" "));
			CstringToChar(tmp, _mat[i].norPath);
			
			if(tmp == "\"\""){
				strcpy(_mat[i].norPath, "");
			}else{
				tmp = tmp.Right(tmp.GetLength() - 3);
				CstringToChar(tmp, _mat[i].norPath);
			}
			_mat[i].textureData = NULL;
			_mat[i].textureID = 0;
			_mat[i].normalMapData = NULL;
			_mat[i].normalMapID = 0;
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("%s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
		}
	}
}

//add by lzh 2014/10/12
void
CMaterial::Load(FILE *fp, wchar_t * const path){
	_ftscanf(fp, _T("Material Numbers:%d\n"), &num);
	if(feof(fp) != 0)
	{
		_mat = NULL;
		num = 0;
		textureMode = 0;
	}
	else
	{
		CString tmp;
		CString pathTemp(path);
		int r, g, b;
		_ftscanf(fp, _T("TextureMode: %d\n"), &textureMode);
		//修改了char数组的长度为1024 by lzh 2015/4/13
		_ftscanf(fp, _T("%s\n"), tmp.GetBuffer(1024));
		tmp.ReleaseBuffer();
		_mat = new MatItem[num];
		for(int i=0; i<num; ++i)
		{
			//一般来说CString要像这样使用，即采用GetBu                                                                               ffer转换成LPTSTR进行
			//使用，然后在使用ReleaseBuffer()进行更新CString中的成员变量。comment by lzh 2014/10/6
			
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("Material Name: %s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
			CstringToChar(tmp, _mat[i].matName);
			_ftscanf(fp, _T("Diffuse: %d %d %d\n"), &r, &g, &b);
			_mat[i].diffuse = RGB(r, g, b);
			_ftscanf(fp, _T("Ambitient: %d %d %d\n"), &r, &g, &b);
			_mat[i].ambitient = RGB(r, g, b);
			_ftscanf(fp, _T("Specular:  %d %d %d\n"), &r, &g, &b);
			_mat[i].specular = RGB(r, g, b);
			
			_ftscanf(fp, _T("Shininess: %f\n"), &_mat[i].shininess);

			//关于使用_fgets的时候要注意它会把换行符也读取在字符串中，并且在vs的调试中你不能明显看到是包含了换行符的
			//所以tmp = tmp.Left(tmp.GetLength() - 1);是用来去掉最后的换行符的。comment by lzh 2014/10/06
			
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("Texture Path: %s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
			tmp.Trim(_T(" "));
			CstringToChar(tmp, _mat[i].texPath);
			if(tmp == "\"\""){
				strcpy(_mat[i].texPath, "");
			}else{
				int ttt = tmp.GetLength();
				CString re;
				re = tmp.Right(ttt - 1);
				tmp = re.Left(re.GetLength() - 1);
				//暂时简单的判断第二个字符是不是':'，如果不是的话 ，则它是相对路径。
				if(tmp[1] != ':'){
					pathTemp = pathTemp.Left(pathTemp.ReverseFind(_T('\\')) + 1);
					tmp = pathTemp + tmp;
				}
				CstringToChar(tmp, _mat[i].texPath);
			}
			/*_fgetts(tmp.GetBuffer(100), 99, fp);
			tmp.ReleaseBuffer();
			tmp.Trim(_T(" "));
			tmp = tmp.Left(tmp.GetLength() - 1);
			tmp = tmp.Right(tmp.GetLength() - 12);*/
			
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("Normal Path: %s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
			tmp.Trim(_T(" "));
			CstringToChar(tmp, _mat[i].norPath);
			
			if(tmp == "\"\""){
				strcpy(_mat[i].norPath, "");
			}else{
				tmp = tmp.Right(tmp.GetLength() - 3);
				CstringToChar(tmp, _mat[i].norPath);
			}
			_mat[i].textureData = NULL;
			_mat[i].textureID = 0;
			_mat[i].normalMapData = NULL;
			_mat[i].normalMapID = 0;
			//修改了char数组的长度为1024 by lzh 2015/4/13
			_ftscanf(fp, _T("%s\n"), tmp.GetBuffer(1024));
			tmp.ReleaseBuffer();
		}
		//这句加了是不是应该呢？刘志华 2014.9.20
		//textureMode = 0;
	}
	matr = NULL;
	textureID = NULL;
	normalMapID = NULL;
}
//end add by lzh 2014/10/12


CMaterial::CMaterial(FILE *fp)
{
	Load(fp);
}

CMaterial::CMaterial(MatInfo *info, int n)
	:num(n)
{
	_mat = new MatItem[n];
	memset(_mat, 0, sizeof(MatItem)*n);
	for(int i=0; i<n; ++i)
	{
		_mat[i].ambitient = info[i].ambitient;
		_mat[i].diffuse = info[i].diffuse;
		_mat[i].specular = info[i].specular;
		_mat[i].shininess = info[i].shininess;
		strcpy(_mat[i].matName, info[i].matName);//添加材质名称的赋值。刘志华 2014.9.13.
		//修改了char数组的长度为1024 by lzh 2015/4/13
		memcpy(_mat[i].texPath, info[i].texPath, sizeof(char)*1024);
		memcpy(_mat[i].norPath, info[i].norPath, sizeof(char)*1024);

		//add by lzh 2015/3/27
		_mat[i].radation = info[i].radation;
		_mat[i].reflection = info[i].reflection;
		_mat[i].refraction = info[i].refraction;
		_mat[i].smooth = info[i].smooth;
		_mat[i].transparent = info[i].transparent;
		//end by lzh 2015/3/27

		_mat[i].textureData = NULL;
		_mat[i].textureID = 0;
		_mat[i].normalMapData = NULL;
		_mat[i].normalMapID = 0;
	}
	textureMode = 0;
	for(int i=0; i<n; ++i)
	{
		if(_mat[i].texPath[0] != 0)
			textureMode = 1;//材质贴图和凹凸贴图不可兼得？
		if(_mat[i].norPath[0] != 0)
		{
			textureMode = 2;
			break;
		}
	}
}
bool CMaterial::isloadMat(){
	return _mat != NULL;
}
void
CMaterial::Save(FILE *fp)
{
	_ftprintf(fp, _T("Material Numbers: %d\n"), num);
	_ftprintf(fp, _T("TextureMode: %d\n"), textureMode);
	_ftprintf(fp, _T("---------------------------------------------------------\n"));
	for(int i=0; i<num; ++i)
	{
		_ftprintf(fp, _T("Material Name: %s\n"), CharToCstring(_mat[i].matName));
		_ftprintf(fp, _T("Diffuse: %d %d %d\n"), GetRValue(_mat[i].diffuse), GetGValue(_mat[i].diffuse), GetBValue(_mat[i].diffuse));
		_ftprintf(fp, _T("Ambitient: %d %d %d\n"), GetRValue(_mat[i].ambitient), GetGValue(_mat[i].ambitient), GetBValue(_mat[i].ambitient));
		_ftprintf(fp, _T("Specular: %d %d %d\n"), GetRValue(_mat[i].specular), GetGValue(_mat[i].specular), GetBValue(_mat[i].specular));
		_ftprintf(fp, _T("Shininess: %.2f\n"), _mat[i].shininess);
		_ftprintf(fp, _T("Texture Path: \"%s\"\n"), CharToCstring(_mat[i].texPath));
		_ftprintf(fp, _T("Normal Path: \"%s\"\n"), CharToCstring(_mat[i].norPath));
		_ftprintf(fp, _T("---------------------------------------------------------\n"));
	}

	/*old version, been commented out in 2014.9.12 by lzh
	****************************************************************************
	fwrite(&num, sizeof(int), 1, fp);
	fwrite(&textureMode, sizeof(int), 1, fp);
	for(int i=0; i<num; ++i)
	{
		fwrite( &_mat[i], sizeof(COLORREF), 3, fp);
		fwrite( &_mat[i].shininess, sizeof(float), 1, fp);
		fwrite( _mat[i].texPath, sizeof(char)*100, 1, fp);
		fwrite( _mat[i].norPath, sizeof(char)*100, 1, fp);
	}
	*********************************************************************************************
	*/
}
//add by zlh 2014/10/13
//添加路径判断的保存。
void
CMaterial::Save(FILE *fp, wchar_t *const path){
	_ftprintf(fp, _T("Material Numbers: %d\n"), num);
	_ftprintf(fp, _T("TextureMode: %d\n"), textureMode);
	_ftprintf(fp, _T("---------------------------------------------------------\n"));
	CString pathTemp(path);//保存mat文件路径的CString，便于之后操作。
	pathTemp = pathTemp.Left(pathTemp.ReverseFind(_T('\\')) + 1);
	CString cstringPath;//之后操作texPath和norPath使用。
	for(int i=0; i<num; ++i)
	{
		_ftprintf(fp, _T("Material Name: %s\n"), CharToCstring(_mat[i].matName));
		_ftprintf(fp, _T("Diffuse: %d %d %d\n"), GetRValue(_mat[i].diffuse), GetGValue(_mat[i].diffuse), GetBValue(_mat[i].diffuse));
		_ftprintf(fp, _T("Ambitient: %d %d %d\n"), GetRValue(_mat[i].ambitient), GetGValue(_mat[i].ambitient), GetBValue(_mat[i].ambitient));
		_ftprintf(fp, _T("Specular: %d %d %d\n"), GetRValue(_mat[i].specular), GetGValue(_mat[i].specular), GetBValue(_mat[i].specular));
		_ftprintf(fp, _T("Shininess: %.2f\n"), _mat[i].shininess);
		
		cstringPath = _mat[i].texPath;
		if(cstringPath.Find(pathTemp) == 0){
			cstringPath = cstringPath.Right(cstringPath.GetLength() - pathTemp.GetLength());
		}
		_ftprintf(fp, _T("Texture Path: \"%s\"\n"), cstringPath);
		cstringPath = _mat[i].norPath;
		if(cstringPath.Find(pathTemp) == 0){
			cstringPath = cstringPath.Right(cstringPath.GetLength() - pathTemp.GetLength());
			//MessageBox(NULL, cstringPath, _T("tttt"), MB_OK);
		}
		_ftprintf(fp, _T("Normal Path: \"%s\"\n"), cstringPath);
		_ftprintf(fp, _T("---------------------------------------------------------\n"));
	}
}
//end add by lzh 2014/10/13

CMaterial::~CMaterial(void)
{
	for(int i=0; i<num; ++i)
	{
		if(_mat[i].textureData != NULL)
		{
			free(_mat[i].textureData);
			_mat[i].textureData = NULL;
		}
			
		if(_mat[i].normalMapData != NULL)
		{
			free(_mat[i].normalMapData);
			_mat[i].normalMapData = NULL;
		}
			
	}
	//2015/6/7 发现这里有的是new出来的却是使用free掉的 by lzh
	//free(_mat);
	delete[] _mat;
	if(textureID){
		delete[] textureID;
		textureID = NULL;
	}
		
	if(normalMapID){
		delete[] normalMapID;
		normalMapID = NULL;
	}
		
	if(matr){
		delete matr;
		matr = NULL;
	}
		
}

void
CMaterial::GenerateMaterialInfo()
{
	if(_mat == NULL){
		matr = NULL;
		return;
	}
	matr = new MatInfo[num];
	memset(matr, 0, sizeof(MatInfo)*num);

	for(int i=0; i<num; ++i)
	{
		matr[i].ambitient = _mat[i].ambitient;
		matr[i].diffuse = _mat[i].diffuse;
		matr[i].shininess = _mat[i].shininess;
		matr[i].specular = _mat[i].specular;
		//修改了char数组的长度为1024 by lzh 2015/4/13
		memcpy(matr[i].texPath, _mat[i].texPath, sizeof(char)*1024);
		memcpy(matr[i].norPath, _mat[i].norPath, sizeof(char)*1024);
		strcpy(matr[i].matName, _mat[i].matName);
		//add by lzh 2015/3/27
		matr[i].radation = _mat[i].radation;
		matr[i].smooth = _mat[i].smooth;
		matr[i].reflection = _mat[i].reflection;
		matr[i].refraction = _mat[i].refraction;
		matr[i].transparent = _mat[i].transparent;
		//end by lzh 2015/3/27
	}
}

int
CMaterial::GetMaterialNum()
{
	return num;
}

void 
CMaterial::GenerateTextureID()
{
	if(_mat == NULL){
		textureID = NULL;
		normalMapID = NULL;
		return ;
	}
	textureID = new GLuint[num];
	for(int i=0; i<num; ++i)
	{
		textureID[i] = _mat[i].textureID;
	}
		
	normalMapID = new GLuint[num];
	for(int i=0; i<num; ++i){
		normalMapID[i] = _mat[i].normalMapID;
	}
		
}
//读取对应字符串地址的bmp纹理文件并装载入内存。by lzh
void
CMaterial::GenerateTexture()
{
	for(int i=0; i<num; ++i)
	{
		if(_mat[i].textureData)
		{
			free(_mat[i].textureData);
			_mat[i].textureData = NULL;
		}
		if(_mat[i].normalMapData)
		{
			free(_mat[i].normalMapData);
			_mat[i].normalMapData = NULL;
		}

		if(_mat[i].texPath[0] == 0 )
		{
			_mat[i].textureID = 0;
			_mat[i].textureData = NULL;
			//修改了char数组的长度为1024 by lzh 2015/4/13
			memset(_mat[i].norPath, 0, sizeof(char)*1024);
			_mat[i].normalMapID = 0;
			_mat[i].normalMapData = NULL;
			continue;
		}
		BITMAPINFOHEADER biHeader;    // This will hold the info header of the bitmap.
		_mat[i].textureData = LoadBitmap(_mat[i].texPath, &biHeader);

		if (_mat[i].textureData == NULL)
		{
			//		MessageBox(NULL, L"texture generate failed", L"Error", MB_OK);
			_mat[i].textureID = 0;
			continue;
		}

		int imageWidth = biHeader.biWidth;
		int imageHeight = biHeader.biHeight;

		// Generate the texture and text the id to the images id.

		glGenTextures(1, &_mat[i].textureID);

		//*bind texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		// Here we bind the texture and set up the filtering.
		glBindTexture(GL_TEXTURE_2D, _mat[i].textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//重复
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageWidth, imageHeight,
			GL_RGB, GL_UNSIGNED_BYTE, _mat[i].textureData);//对纹理进行缩放？
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 
			0, GL_RGB, GL_UNSIGNED_BYTE, _mat[i].textureData);

		if (_mat[i].norPath[0] == 0)
		{
			//		MessageBox(NULL, L"texture generate failed", L"Error", MB_OK);
			_mat[i].normalMapID = 0;
			continue;
		}

		//normal map texture
		_mat[i].normalMapData = LoadBitmap(_mat[i].norPath, &biHeader);
		
		if (_mat[i].normalMapData == NULL)
		{
			//		MessageBox(NULL, L"texture generate failed", L"Error", MB_OK);
			_mat[i].normalMapID = 0;
			continue;
		}

		imageWidth = biHeader.biWidth;
		imageHeight = biHeader.biHeight;

		// Generate the texture and text the id to the images id.

		glGenTextures(1, &_mat[i].normalMapID);

		//*bind texture in Texture Unit 1
		glActiveTexture(GL_TEXTURE1);//双重纹理

		// Here we bind the texture and set up the filtering.
		glBindTexture(GL_TEXTURE_2D, _mat[i].normalMapID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageWidth, imageHeight,
			GL_RGB, GL_UNSIGNED_BYTE, _mat[i].normalMapData);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 
			0, GL_RGB, GL_UNSIGNED_BYTE, _mat[i].normalMapData);
	}
}

unsigned char* CMaterial::LoadBitmap(char *file, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *pFile;                  // Need this to open a file.
	BITMAPFILEHEADER	header;     // This will hold the bitmap header information.

	unsigned char *textureData;   // This will hold the bitmap image itself.

	// This will be used to swap the image colors from BGR to RGB.
	unsigned char textureColors;

	pFile = fopen(file, "rb");    // Open the file.

	if(pFile == 0) return 0;      // Check and make sure there are no errors.

	// Read in the bitmap header info into the BITMAPFILEHEADER variable.
	fread(&header, sizeof(BITMAPFILEHEADER), 1, pFile);

	// Make sure this is a real bitmap by checking the ID.
	if(header.bfType != BITMAP_ID)
	{
		fclose(pFile);
		return 0;
	}

	// Read in the second header info into the BITMAPINFOHEADER variable.
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

	if(bitmapInfoHeader->biSizeImage == 0)
		bitmapInfoHeader->biSizeImage = bitmapInfoHeader->biWidth * bitmapInfoHeader->biHeight * 3;

	// Place the pointer in front of where the image data starts.
	fseek(pFile, header.bfOffBits, SEEK_SET);

	// Dynamically create enough memory for the image.
	//textureData = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage + 1);
	//为了防止free的时候出错，我在这里将上面那句代码，修改成了下面的样子，防止了free的时候出错。
	//lzh todo 这里标记上todo意思是需要进行修改的意思。
	textureData = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage + 10);


	// Error checking.  Make sure the memory was allocated.
	if(!textureData)
	{
		free(textureData);
		fclose(pFile);
		return 0;
	}

	// Read in the image.
	fread(textureData, 1, bitmapInfoHeader->biSizeImage, pFile);

	// Error checking.  Make sure an image was loaded.
	if(textureData == 0)
	{
		fclose(pFile);
		return 0;
	}

	// Bitmaps are saved in BGR format so we will make the image RGB by...
	for(int index = 0; index < (int)bitmapInfoHeader->biSizeImage; index+=3)
	{
		textureColors = textureData[index];
		textureData[index] = textureData[index + 2];
		textureData[index + 2] = textureColors;
	}

	fclose(pFile);     // We are done with the file so close it.
	return textureData;  // Send the image to the function that called this.
}