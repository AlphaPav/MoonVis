#include "stdafx.h"
#include "AVIRecorder.h"
#include <gl/GL.h>

bool CAVIRecorder::Snap()
{
	glReadPixels(0,0,lpbih->biWidth,lpbih->biHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,bmBits); 
	// send to avi engine
	HRESULT hr=AviGen.AddFrame(bmBits);
	if (FAILED(hr))
		return false;
	else
		return true;
}

bool CAVIRecorder::Begin(int fps, CWnd *pView, const wchar_t *fname)
{
	// set 15fps
	AviGen.SetRate(fps);
	
	// give info about bitmap
	AviGen.SetBitmapHeader(pView);		

	// set filename, extension ".avi" is appended if necessary
	AviGen.SetFileName(fname);

	// retreiving size of image
	lpbih=AviGen.GetBitmapHeader();

	// allocating memory
	bmBits=new BYTE[lpbih->biSizeImage];

	HRESULT hr=AviGen.InitEngine();
	if (FAILED(hr))
		return false;
	else
		return true;
}

bool CAVIRecorder::End()
{
	AviGen.ReleaseEngine();
	delete[] bmBits;
	return true;
}

bool CBmpCapturer::Output(const wchar_t *fname)
{
	FILE *pFile = _wfopen(fname, L"wb");;                  // Our file pointer.
	if(!pFile)
		return false;

	BITMAPFILEHEADER fileHeader;  // file header.
	// Define whats going in the file header.
	fileHeader.bfSize       = sizeof(BITMAPFILEHEADER);
	fileHeader.bfType       = 0x4D42;
	fileHeader.bfReserved1  = 0;
	fileHeader.bfReserved2  = 0;
	fileHeader.bfOffBits    = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Now lets write to the file.
	// Write file header.
	fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), pFile);

	// Write info header.
	fwrite(lpbih, 1, sizeof(BITMAPINFOHEADER), pFile);

	// Write image.
	fwrite(bmBits, 1, lpbih->biSizeImage, pFile);

	// Last but not least close the file.
	fclose(pFile);
	return true;
}

bool CBmpCapturer::Capture(int x, int y, int width, int height, const wchar_t *fname)
{
	// give info about bitmap
	AviGen.SetBitmapHeader(width, height);
	// retreiving size of image
	lpbih=AviGen.GetBitmapHeader();

	// allocating memory
	bmBits=new BYTE[lpbih->biSizeImage];
	
	glReadPixels(x,y,lpbih->biWidth,lpbih->biHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE, bmBits);

	bool ret = Output(fname);

	delete[] bmBits;
	return ret;
}
