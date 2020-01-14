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

#ifndef _KIPTYPES_H
#define _KIPTYPES_H
#include <math.h>
#ifdef __cplusplus
extern "C"
{
#endif

//--------------------------------------------------------------------------
// Primitive data types
//--------------------------------------------------------------------------
typedef unsigned __int8		uint8;
typedef void *				KipEngineHandle;

//--------------------------------------------------------------------------
// Geometric data structs
//--------------------------------------------------------------------------

typedef struct KipCoord2d
{
	double x;
	double y;
} KipCoord2d;

typedef struct KipCoord3d
{
	double x;
	double y;
	double z;
} KipCoord3d;

typedef struct KipCoord2f
{
	float x;
	float y;
} KipCoord2f;

typedef struct KipCoord3f
{
	float x;
	float y;
	float z;
} KipCoord3f;

typedef struct KipCoord2i
{
	int x;
	int y;
} KipCoord2i;

typedef struct KipCoord3i
{
	int x;
	int y;
	int z;
} KipCoord3i;

typedef struct KipSize2i
{
	int w;
	int h;
} KipSize2i;

typedef struct KipRecti{
	int    left;
	int    top;
	int    right;
	int    bottom;
} KipRecti;

#define KIP_CN_MAX     256
#define KIP_CN_SHIFT   4
#define KIP_DEPTH_MAX  (1 << KIP_CN_SHIFT)

#define KIP_DEPTH_TYPE_U	0
#define KIP_DEPTH_TYPE_S	1
#define KIP_DEPTH_TYPE_F	2
#define KIP_DEPTH_TYPE_N	2

#define KIP_MAKE_DEPTH(type, bits)		((((type) & 3) << 2) | (bits) & 3)
#define KIP_8U   KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_U, 0)
#define KIP_8S   KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_S, 0)
#define KIP_16U  KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_U, 1)
#define KIP_16S  KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_S, 1)
#define KIP_32U  KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_U, 2)
#define KIP_32S  KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_S, 2)
#define KIP_32F  KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_F, 2)
#define KIP_64F	 KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_F, 3)
#define KIP_USRTYPE1 KIP_MAKE_DEPTH(KIP_DEPTH_TYPE_N, 2)

#define KIP_TYPE_DEPTH_MASK       (KIP_DEPTH_MAX - 1)
#define KIP_TYPE_DEPTH(flags)     ((flags) & KIP_TYPE_DEPTH_MASK)
#define KIP_TYPE_DEPTH_BYTES(flags)     (1 << (((flags) & KIP_TYPE_DEPTH_MASK) & 3))

#define KIP_MAKETYPE(depth,cn) (KIP_TYPE_DEPTH(depth) + (((cn)-1) << KIP_CN_SHIFT))
#define KIP_MAKE_TYPE KIP_MAKETYPE

#define KIP_8UC1 KIP_MAKETYPE(KIP_8U,1)
#define KIP_8UC2 KIP_MAKETYPE(KIP_8U,2)
#define KIP_8UC3 KIP_MAKETYPE(KIP_8U,3)
#define KIP_8UC4 KIP_MAKETYPE(KIP_8U,4)
#define KIP_8UC(n) KIP_MAKETYPE(KIP_8U,(n))

#define KIP_8SC1 KIP_MAKETYPE(KIP_8S,1)
#define KIP_8SC2 KIP_MAKETYPE(KIP_8S,2)
#define KIP_8SC3 KIP_MAKETYPE(KIP_8S,3)
#define KIP_8SC4 KIP_MAKETYPE(KIP_8S,4)
#define KIP_8SC(n) KIP_MAKETYPE(KIP_8S,(n))

#define KIP_16UC1 KIP_MAKETYPE(KIP_16U,1)
#define KIP_16UC2 KIP_MAKETYPE(KIP_16U,2)
#define KIP_16UC3 KIP_MAKETYPE(KIP_16U,3)
#define KIP_16UC4 KIP_MAKETYPE(KIP_16U,4)
#define KIP_16UC(n) KIP_MAKETYPE(KIP_16U,(n))

#define KIP_16SC1 KIP_MAKETYPE(KIP_16S,1)
#define KIP_16SC2 KIP_MAKETYPE(KIP_16S,2)
#define KIP_16SC3 KIP_MAKETYPE(KIP_16S,3)
#define KIP_16SC4 KIP_MAKETYPE(KIP_16S,4)
#define KIP_16SC(n) KIP_MAKETYPE(KIP_16S,(n))

#define KIP_32SC1 KIP_MAKETYPE(KIP_32S,1)
#define KIP_32SC2 KIP_MAKETYPE(KIP_32S,2)
#define KIP_32SC3 KIP_MAKETYPE(KIP_32S,3)
#define KIP_32SC4 KIP_MAKETYPE(KIP_32S,4)
#define KIP_32SC(n) KIP_MAKETYPE(KIP_32S,(n))

#define KIP_32FC1 KIP_MAKETYPE(KIP_32F,1)
#define KIP_32FC2 KIP_MAKETYPE(KIP_32F,2)
#define KIP_32FC3 KIP_MAKETYPE(KIP_32F,3)
#define KIP_32FC4 KIP_MAKETYPE(KIP_32F,4)
#define KIP_32FC(n) KIP_MAKETYPE(KIP_32F,(n))

#define KIP_64FC1 KIP_MAKETYPE(KIP_64F,1)
#define KIP_64FC2 KIP_MAKETYPE(KIP_64F,2)
#define KIP_64FC3 KIP_MAKETYPE(KIP_64F,3)
#define KIP_64FC4 KIP_MAKETYPE(KIP_64F,4)
#define KIP_64FC(n) KIP_MAKETYPE(KIP_64F,(n))

#define KIP_IMAGE_CN_MASK          ((KIP_CN_MAX - 1) << KIP_CN_SHIFT)
#define KIP_IMAGE_TYPE_MASK        (KIP_DEPTH_MAX*KIP_CN_MAX - 1)

#define KIP_IMAGE_FORMAT_MAX		16
#define KIP_IMAGE_FORMAT_SHIFT		12
#define KIP_IMAGE_FORMAT_MASK		((KIP_IMAGE_FORMAT_MAX - 1) << KIP_IMAGE_FORMAT_SHIFT)

#define KIP_IMAGE_FORMAT_GRAY		0
#define KIP_IMAGE_FORMAT_YUV		1
#define KIP_IMAGE_FORMAT_BGR		2
#define KIP_IMAGE_FORMAT_RGB		3
#define KIP_IMAGE_FORMAT_BGRA		4
#define KIP_IMAGE_FORMAT_ARGB		5

#define KIP_IMAGE_MAKE_CODE_DEPTH(format, depth, cn)	((format) << KIP_IMAGE_FORMAT_SHIFT | KIP_MAKETYPE(depth,cn))
#define KIP_IMAGE_MAKE_CODE(format, type)	(((format) << KIP_IMAGE_FORMAT_SHIFT) | (type))

#define KIP_IMAGE_BYTES_PER_PIXEL(flags)	((KIP_TYPE_DEPTH_BYTES(flags)) * (KIP_IMAGE_CN(flags)))
#define KIP_IMAGE_BITS_PER_PIXEL(flags)	((KIP_IMAGE_BYTES_PER_PIXEL(flags)) << 3)
#define KIP_IMAGE_TYPE(flags)      ((flags) & KIP_IMAGE_TYPE_MASK)
#define KIP_IMAGE_CN(flags)        ((((flags) & KIP_IMAGE_CN_MASK) >> KIP_CN_SHIFT) + 1)

enum   KipImageCode { 
	KipImageCodeGray8u = KIP_IMAGE_MAKE_CODE(KIP_IMAGE_FORMAT_GRAY, KIP_8UC1),	// 8-bit per-pixel gray scale.
	KipImageCodeGray16u =KIP_IMAGE_MAKE_CODE(KIP_IMAGE_FORMAT_GRAY, KIP_16UC1),	// 16-bit per-pixel gray scale.
	KipImageCodeYUY2 = KIP_IMAGE_MAKE_CODE(KIP_IMAGE_FORMAT_YUV, KIP_8UC2),		// 16-bit per-pixel packed YUV 4:2:2 format. 
	KipImageCodeBGRA32u = KIP_IMAGE_MAKE_CODE(KIP_IMAGE_FORMAT_BGRA, KIP_8UC4),	// 32-bit per-pixel BGRA. Byte ordering compatible with Windows 32-bit RGB bitmaps.  
	KipImageCodeARGB32u = KIP_IMAGE_MAKE_CODE(KIP_IMAGE_FORMAT_ARGB, KIP_8UC4),	// 32-bit per-pixel ARGB.  
	KipImageCodeBGR24u = KIP_IMAGE_MAKE_CODE(KIP_IMAGE_FORMAT_BGR, KIP_8UC3),	// 24-bit per-pixel BGR. Byte ordering compatible with Windows 24-bit RGB bitmaps.  
	KipImageCodeRGB24u = KIP_IMAGE_MAKE_CODE(KIP_IMAGE_FORMAT_RGB, KIP_8UC3)	// 24-bit per-pixel RGB.
};

// bottom-up image
// if top-down image, step_bytes < 0.
typedef struct KipImageInfo
{
	KipImageCode format;
	int			width;
	int			height;
	int			step_bytes;
	uint8 *		pixel_addr;
} KipImageInfo;

__inline bool KipImageIsBottomUp(const KipImageInfo * pInfo)
{
	return pInfo->step_bytes > 0;
}

__inline bool KipImageIsTopDown(const KipImageInfo * pInfo)
{
	return pInfo->step_bytes < 0;
}

__inline uint8 * KipImageScanLine(const KipImageInfo * pInfo, int row = 0)
{
	if(KipImageIsTopDown(pInfo))
		return pInfo->pixel_addr - pInfo->step_bytes * row;
	return pInfo->pixel_addr + (pInfo->height - 1 - row) * pInfo->step_bytes;
}

__inline uint8 * KipImageScanLine0(const KipImageInfo * pInfo)
{
	if(KipImageIsTopDown(pInfo))
		return pInfo->pixel_addr;
	return pInfo->pixel_addr + (pInfo->height - 1) * pInfo->step_bytes;
}

__inline int KipImageStride(const KipImageInfo * pInfo)
{
	return -pInfo->step_bytes;
}

__inline int KipImageChanels(const KipImageInfo * pInfo)
{
	return KIP_IMAGE_CN(pInfo->format);
}

__inline int KipImageValueType(const KipImageInfo * pInfo)
{
	return KIP_IMAGE_TYPE(pInfo->format);
}

__inline int KipImageBytesPerPixel(const KipImageInfo * pInfo)
{
	return KIP_IMAGE_BYTES_PER_PIXEL(pInfo->format);
}

__inline int KipImageBitsPerPixel(const KipImageInfo * pInfo)
{
	return KIP_IMAGE_BITS_PER_PIXEL(pInfo->format);
}

__inline int KipImageDataSize(const KipImageInfo * pInfo)
{
	return pInfo->height * abs(pInfo->step_bytes);
}

enum KipResultCode
{
	KipResultSuccess = 0,
	KipResultNone = -1,
	KipResultError = 1,
};

#ifdef __cplusplus
}
#endif
#endif // !_KIPTYPES_H