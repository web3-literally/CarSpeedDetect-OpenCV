
// Roulette.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRouletteApp:
// See Roulette.cpp for the implementation of this class
//

class CRouletteApp : public CWinApp
{
public:
	CRouletteApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRouletteApp theApp;