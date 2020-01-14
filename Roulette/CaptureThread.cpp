// CaptureThread.cpp : implementation file
//

#include "stdafx.h"
#include "Roulette.h"
#include "CaptureThread.h"
#include "opencv/ml.h"
#include "opencv/cvaux.h"

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
CMutex mutex;
#include <mutex>
#include <iostream>
using namespace std;
// CaptureThread


IMPLEMENT_DYNCREATE(CaptureThread, CWinThread)

#define WM_USER_RERESH WM_USER+1


CaptureThread::CaptureThread()
{
	
}

CaptureThread::CaptureThread(CRouletteDlg* pDlg)
{
	m_pMainDlg = pDlg;
	m_media = &m_pMainDlg->m_media;
}

CaptureThread::~CaptureThread()
{
	if (m_capture != NULL) {
		m_curFrame.release();
		m_capture->release();
	}
}

BOOL CaptureThread::InitInstance()
{
	return TRUE;
}

int CaptureThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CaptureThread, CWinThread)
END_MESSAGE_MAP()

// CaptureThread message handlers
void CaptureThread::updateDatabase()
{
	m_capture = m_pMainDlg->m_videoCapture;
}

std::mutex mtxCam1;
int CaptureThread::Run()
{
	int delay = 0;
	int wd = m_capture->get(CV_CAP_PROP_FRAME_WIDTH) / 2;
	int ht = m_capture->get(CV_CAP_PROP_FRAME_HEIGHT) / 2;
	delay = (int) (1000.0 / (double)m_capture->get(CV_CAP_PROP_FPS)) - 13;
	while (true)
	{
		if (m_pMainDlg->threadstate == 1 && m_pMainDlg->m_curState == PAI_READY)
		{
			::SendMessage(AfxGetMainWnd()->m_hWnd, CUS_TERMINATE, 0, 0);
			return 1;
		}
		if (m_pMainDlg->threadstate == 1)
		{
			*m_capture>> m_curFrame;
			if (!m_curFrame.empty()) {
				cv::resize(m_curFrame, m_curFrame, cv::Size(wd, ht));
			}
			frametime = m_capture->get(CAP_PROP_POS_MSEC);
			if (m_curFrame.empty() || m_curFrame.cols <= 10 || m_curFrame.rows <= 10)
			{
				::SendMessage(AfxGetMainWnd()->m_hWnd, CUS_TERMINATE, 0, 0);
				return 1;
			}
			mtxCam1.lock();
			m_pMainDlg->curframetime = frametime;
			m_curFrame.copyTo(m_pMainDlg->m_curFrame);			
			mtxCam1.unlock();
			::PostMessage(AfxGetMainWnd()->m_hWnd, WM_USER_RERESH, 0, 0);	
		}
		Sleep(delay);
	}
	
	::SendMessage(AfxGetMainWnd()->m_hWnd, CUS_TERMINATE, 0, 0);
	return 1;
}

int CaptureThread::Release()
{
	return 0;
}