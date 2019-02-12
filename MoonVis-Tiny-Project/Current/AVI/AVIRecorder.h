#pragma once

#include "AVIGenerator.h"

class CAVIRecorder {
	CAVIGenerator AviGen;
	BYTE*bmBits;
	LPBITMAPINFOHEADER lpbih;

public:
	bool Begin(int fps, CWnd *pView, const wchar_t *fname);
	bool End();
	bool Snap();
};

class CBmpCapturer {
	CAVIGenerator AviGen;
	BYTE*bmBits;
	LPBITMAPINFOHEADER lpbih;

	bool Output(const wchar_t *fname);

public:
	bool Capture(int x, int y, int width, int height, const wchar_t *fname);
};