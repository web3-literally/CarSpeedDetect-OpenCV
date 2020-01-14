#pragma once
/**************************************************************************\
*
* Copyright (c) 2010-2012, Kulak Image Processing Team, RIC.  All Rights Reserved.
*
* Module Name:
*
*   KipTypes.h
*
* Abstract:
*
*   Kip geometric and image Types
*
\**************************************************************************/

#ifndef _KIPIMAGEUTIL_H
#define _KIPIMAGEUTIL_H
#include "KipTypes.h"
#include <GdiPlus.h>
using namespace Gdiplus;
#ifdef __cplusplus
extern "C"
{
#endif

__inline bool KipImageIsSameInfo(const KipImageInfo * pImage1, const KipImageInfo * pImage2)
{
	return pImage1->width == pImage2->width &&
		pImage1->height == pImage2->height &&
		pImage1->format == pImage2->format &&
		pImage1->step_bytes == pImage2->step_bytes;
}

__inline void KipImageFree(KipImageInfo * pImage)
{
	if(pImage && pImage->pixel_addr)
	{
		delete [] pImage->pixel_addr;
		pImage->pixel_addr = 0;
	}
}

__inline KipImageInfo KipImageCreate(const KipImageInfo * pImage)
{
	KipImageInfo Result;
	Result = *pImage;
	Result.pixel_addr = new uint8[KipImageDataSize(pImage)];
	return Result;
}

__inline Bitmap * KipImage2GdiplusBitmap(const KipImageInfo * pImage)
{
	const KipImageInfo & kipInfo = *pImage;
	BITMAPINFO info = {0};
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = kipInfo.width;
	info.bmiHeader.biHeight = kipInfo.height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = KipImageBitsPerPixel(pImage);
	info.bmiHeader.biCompression = BI_RGB;

	if(KipImageIsTopDown(pImage))
		info.bmiHeader.biHeight = -kipInfo.height;

	return new Bitmap(&info, pImage->pixel_addr);
}
#ifdef __cplusplus
}
#endif
#endif // !_KIPIMAGEUTIL_H