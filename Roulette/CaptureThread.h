#pragma once


#include "RouletteDlg.h"

#include "afxwin.h"
#include "gdiplustypes.h"


#define MaxScreenNum 50

class CaptureThread : public CWinThread
{
	DECLARE_DYNCREATE(CaptureThread)

public:
	CaptureThread();
	CaptureThread(CRouletteDlg*);
	virtual ~CaptureThread();

public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual int		Run();
	int				Release();
	void			updateDatabase();
protected:
	DECLARE_MESSAGE_MAP()
public:
	CRouletteDlg	*m_pMainDlg;
	VideoCapture	*m_capture;
	CStatic			*m_media;
	Mat				m_curFrame;
	double			frametime ;
};