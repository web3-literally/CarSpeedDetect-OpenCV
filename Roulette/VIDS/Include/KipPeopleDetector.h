#pragma once
/**************************************************************************\
*
* Copyright (c) 2010-2012, Kulak Image Processing Team, RIC.  All Rights Reserved.
*
* Module Name:
*
*   KipFaceDetector.h
*
* Abstract:
*
*   Kip people detector APIs.
*
\**************************************************************************/
#ifndef _KIPPEOPLEDETECTOR_H
#define _KIPPEOPLEDETECTOR_H

#include "KipTypes.h"

#define KIP_FD_PARAM_HISTORY		"history"
#define KIP_FD_PARAM_NOISE			"noise"
#define KIP_FD_PARAM_TWINKLE		"twinkle"
#define KIP_FD_PARAM_THRESHOLD1		"threshold1"
#define KIP_FD_PARAM_THRESHOLD2		"threshold2"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct KipPeopleInfo
	{
		KipRecti	bound;
	} KipPeopleInfo;

	typedef struct KipAlarmArea
	{
		KipCoord2f * points;
		int			count;
	} KipAlarmArea;
	//--------------------------------------------------------------------------
	// Create and destroy a people detector engine
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	// Create people detector engine and load database.
	// Return Values :
	//		if successfully create the people engine, return the handle to the engine, otherwise return 0
	// Require database files :
	//		none.
	KipEngineHandle	KipPD_Create();
	void			KipPD_Destroy(KipEngineHandle hKipEngine);
	void			KipPD_InitBackground(KipEngineHandle hKipEngine);
	void			KipPD_SetParam(KipEngineHandle hKipEngine, const char * name, int value);
	int				KipPD_GetParam(KipEngineHandle hKipEngine, const char * name);
	//--------------------------------------------------------------------------
	// detect people on the image.
	// Parameters :
	//		hKipEngine : 
	//			[in] Handle to the people detector engine from a previous call to the KipFD_Create function.
	//		pImageInfo :
	//			[in] A pointer to the KipImageInfo struct will detect people.
	// Return Values :
	//		Returns the count of the people detected if detect successfully or a negative value(-1) otherwise.
	//		To get extended error information, call KipPD_GetLastError.
	int				KipPD_Detect(KipEngineHandle hKipEngine, const KipImageInfo * pImageInfo);

	//--------------------------------------------------------------------------
	// Get people bound rect on the image predetected people from a previous call to the KipPD_Detect.
	// Parameters :
	//		hKipEngine : 
	//			[in] Handle to the people detector engine from a previous call to the KipPD_Create function.
	//		iPeople :
	//			[i] A index of people to receive people info. The index number must is between 0 to people count - 1,
	//				the people count is return value from a previous call to the KipFD_Detect function.
	//		pPeopleInfo :
	//			[out] A pointer to receive the people info.
	// Return Values :
	//		Returns zero if successfully.
	//		To get extended error information, call KipPD_GetLastError.
	int				KipPD_GetPeopleInfo(KipEngineHandle hKipEngine, int iPeople, KipPeopleInfo * pPeopleInfo);

	//--------------------------------------------------------------------------
	// Determines whether the specified people lies within the specified alarm area.
	// Parameters :
	//		hKipEngine : 
	//			[in] Handle to the people detector engine from a previous call to the KipPD_Create function.
	//		pPeopleInfo :
	//			[i] A pointer to the KipPeopleInfo struct that contains the specified people info.
	//		pAlarmArea :
	//			[i] A pointer to the KipAlarmArea struct that contains the specified alarm area.
	//			
	// Return Values :
	//		If the specified people lies within the alarm area, the return value is zero.
	//		If the specified people does not lie within the alarm area, the return value is nonzero. 
	//		To get extended error information, call KipPD_GetLastError.
	int				KipPD_PeopleInAlarmArea(KipEngineHandle hKipEngine, const KipPeopleInfo * pPeopleInfo, const KipAlarmArea * pAlarmArea);

	//--------------------------------------------------------------------------
	void			KipPD_SetDetectMask(KipEngineHandle hKipEngine, const KipAlarmArea* pAlarmArea, int nAreaCount, int w, int h);

	//--------------------------------------------------------------------------
	// retrieves the calling engine's last-error code value.
	// Parameters :
	//		hKipEngine : 
	//			[in] Handle to the people detector engine from a previous call to the KipPD_Create function.
	//		errorMsg :
	//			[out] Pointer to a buffer that receives a null-terminated string that specifies the error info of the engine. 
	//				If the length of the error info exceeds the size that the nSize parameter specifies, the function succeeds, and the string is truncated to nSize characters and cannot be null terminated.
	//		nSize 
	//			[in] Size of the errorMsg buffer, in chars. 
	//	Return Values
	//		The return value is the calling engine's last-error code value. 
	KipResultCode	KipPD_GetLastError(KipEngineHandle hKipEngine, char * errorMsg = 0, int nSize = 0);

	//--------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // !_KIPPEOPLEDETECTOR_H